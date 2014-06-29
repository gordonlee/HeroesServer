#include <winsock2.h>
#include "Core/types.h"
#include "Network/acceptor.h"
#include "Network/console.h"
#include "Network/connection.h"
#include "Network/iocp.h"
#include "Util/gdi_helper.h"
#include "Util/string_util.h"
#include <Windows.h>
#include <windowsx.h>

using namespace zedu;

HWND hWnd;
HWND hEdit;
HINSTANCE hInst;
TCHAR szTitle[] = "RoguelikeSrv";
TCHAR szWindowClass[] = "RoguelikeSrv";

const int INPUT_BOX_HEIGHT = 20; // 하단의 입력창 세로 크기
const int CONSOLE_FONT_HEIGHT = 14;	// 콘솔 폰트 세로 크기
const char* CONSOLE_FONT = "Lucida Console";


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc( int code, WPARAM wParam, LPARAM lParam );

int Filter( EXCEPTION_POINTERS* ep )
{
	int code = ep->ExceptionRecord->ExceptionCode;

	return 1;
}

BOOL InitInstance(HINSTANCE hInstance)
{
	hInst = hInstance;

	WNDCLASS wc = {0};
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
	wc.hCursor = LoadCursor( 0, IDC_ARROW);
	wc.hIcon = LoadIcon( 0, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = szWindowClass;
	wc.lpszMenuName = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	
	RegisterClass( &wc ); 

	// 윈도우 초기화
	hWnd = CreateWindow( szWindowClass , szTitle, WS_OVERLAPPEDWINDOW,
						 CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);

	if( !hWnd )
		return FALSE;

	return TRUE;
}

Console& GetMainConsole()
{
	return Console::DefaultConsole();
}

// 이벤트 핸들러
struct MyEventReceiver : public INetworkEventReceiver
{
	virtual IConnection* CreateConnection(const Socket& sock )
	{
		return new IOCPConnection( NULL, sock );
	}

	virtual bool OnAccept( int threadId, IAcceptor* pAcceptor, IConnection* pConnection )
	{
		GetMainConsole().Printf( "Connection accepted from [%s]\n", pConnection->GetPeerAddress().GetAddr());
		InvalidateRect( hWnd, NULL, false );
		return true;
	}

	virtual void OnRead( int threadId, IConnection* pConnection )
	{
		IOCPConnection* pConn = static_cast<IOCPConnection*>( pConnection );

		zedu::byte buf[2048];
		int receivedBytes = pConn->Read( buf, 2048 );
		buf[receivedBytes] = '\0';

		std::string strRecv = buf;
		zedu::Replace( strRecv, "\n", "" );

		GetMainConsole().Printf( "[%s] >> %s \n", pConnection->GetPeerAddress().GetAddr(), strRecv.c_str() );
		pConn->Write( buf, receivedBytes );
		InvalidateRect( hWnd, NULL, false );
	}

	virtual void OnWrite( int threadId, IConnection* pConnection, uint32 size )
	{
		IOCPConnection* pConn = static_cast<IOCPConnection*>( pConnection );

		char buf[1024];
		pConn->PeekSendBuf((void*)buf, size);
		buf[size] = '\0';

		std::string strSend = buf;
		zedu::Replace( strSend, "\n", "" );

		GetMainConsole().Printf( "[%s] << %s \n", pConnection->GetPeerAddress().GetAddr(), strSend.c_str() );
		InvalidateRect( hWnd, NULL, false );
	}

	virtual void OnDisconnect( int threadId, IConnection* pConnection )
	{
		GetMainConsole().Printf( "Connection disconnected [%s]\n", pConnection->GetPeerAddress().GetAddr());
		InvalidateRect( hWnd, NULL, false );

		delete pConnection;
	}

} g_eventReceiver;

// IOCP 객체
IOCP			g_iocp( &g_eventReceiver );
IOCPAcceptor*	g_pAcceptor;

void StartServer()
{
	cprint("Starting server ... \n");

	WSADATA wsa;
	if( WSAStartup( MAKEWORD(2,2), &wsa) )
	{
		return;
	}

	g_pAcceptor = new IOCPAcceptor( 100 );
	cprint( "Initializing IOCP ... ", g_iocp.Create() ? "ok" : "FAILED" );

	g_iocp.StartThreadPool( 1 );
	
	g_pAcceptor->StartAccept( 9999 );
	g_iocp.AddObject( g_pAcceptor );

	cprint("ok\n");
}

void ShutdownServer()
{
	cprint("Shutdown ... \n");

	PostMessage( hWnd, WM_CLOSE, 0, 0 );
	cprint("ok\n");
}

void ProcCommand( const char* buf )
{
	GetMainConsole().Printf( buf );

	std::vector<std::string> tokens;
	Split( buf, tokens, " \n", true );

	if( !tokens.empty() )
	{
		if( tokens[0] == "exit" )			ShutdownServer();
		else if( tokens[0] == "clear" )		GetMainConsole().Clear();
	}
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)

{
	__try
	{
		// 윈도우 초기화
		if( !InitInstance( hInstance ) )
			return FALSE;

	   // 키보드 훅 설정
		HHOOK hKeyboardHook = SetWindowsHookEx( WH_KEYBOARD, KeyboardProc, NULL, GetCurrentThreadId() );

	   ShowWindow( hWnd, nCmdShow );
	   UpdateWindow( hWnd );

	   // 서비스 개시~
	   StartServer();
	   InvalidateRect( hWnd, NULL, false );

	   // 메세지루프
	   MSG msg;
	   while( GetMessage( &msg, NULL, 0, 0 ) )
	   {
		   TranslateMessage(&msg);
		   DispatchMessage(&msg);
	   }

		UnhookWindowsHookEx( hKeyboardHook );
		return (int)msg.wParam;
	}
	__except( GetExceptionCode() )
	{
		__asm 
		{
			int 3;
		}
		return 0;
	}
}

void DrawConsole(HDC hDC, Console* pConsole)
{
	struct Canvas_W32
	{
		int m_canvasWidth, m_canvasHeight;
		
		HFONT oldFont;
		HFONT m_font;
		HDC m_canvasDC;
		HBITMAP m_canvasBitmap;
		HBRUSH m_canvasBrush;
		HGDIOBJ oldObject;
		
		Canvas_W32( HDC hdc, int w, int h, int fontHeight )
		{
			m_canvasWidth = w;
			m_canvasHeight = h;

			m_canvasDC = CreateCompatibleDC( hdc );
			m_canvasBitmap = CreateCompatibleBitmap( hdc, 1600, 1200 );
			oldObject = SelectObject( m_canvasDC, m_canvasBitmap );

			m_font = zedu::CreateFont( "굴림체", fontHeight/2, fontHeight );
			oldFont = SelectFont( m_canvasDC, m_font );
			m_canvasBrush = CreateSolidBrush( RGB(0,0,0) );
			SetTextColor( m_canvasDC, RGB(255,255,255) );
			SetBkMode( m_canvasDC, TRANSPARENT );
		}
		~Canvas_W32()
		{
			SelectObject( m_canvasDC, oldObject );
			SelectFont( m_canvasDC, oldFont );
			DeleteDC( m_canvasDC );
			DeleteObject( m_canvasBitmap );
			DeleteObject( m_canvasBrush );
			DeleteObject( m_font );
		}

		void Clear()
		{
			RECT scRect;
			scRect.bottom = m_canvasWidth;
			scRect.top = 0;
			scRect.left = 0;
			scRect.right = m_canvasHeight;

			FillRect( m_canvasDC, &scRect, m_canvasBrush );
		}

		void Paint(Console* pConsole, HDC hdc, const RECT *pRect)
		{
			const RECT &rect = *pRect;

			Clear();

			int tall = rect.bottom;
			bool bAnchorBottom = false;

			// 화면 크기가 콘솔 내용을 표시하기 충분한가?
			if( pConsole->GetCount() * pConsole->GetLineHeight() > tall )
			{
				bAnchorBottom = true;
			}

			if( bAnchorBottom )
			{
				// 화면 하단정렬
				for(int i = pConsole->GetCount()-1, j=1; i >= 0; i--, j++)
				{
					const char* str = pConsole->GetScreenLine(i);
					int cy = tall - j*(pConsole->GetLineHeight());

					TextOut( m_canvasDC, 5, cy, str, strlen(str) );
				}
			}
			else
			{
				// 화면 상단정렬
				for(int i = 0; i < pConsole->GetCount()+1; i++)
				{
					const char* str = pConsole->GetScreenLine(i);

					TextOut( m_canvasDC, 5, i*(pConsole->GetLineHeight()), str, strlen(str) );
				}
			}

			BitBlt( hdc, 0, 0, m_canvasWidth, m_canvasHeight, m_canvasDC, 0, 0, SRCCOPY );
		}
	};
	static Canvas_W32 canvas( hDC, 1600, 1200, CONSOLE_FONT_HEIGHT );
	
	RECT rc;
	GetClientRect( hWnd, &rc );
	rc.bottom -= (INPUT_BOX_HEIGHT + 2);

	pConsole->Draw<Canvas_W32>( canvas, hDC, &rc );
}

void onWmSize( int width, int height )
{
	static bool bInit = false;
	if( !bInit )
	{
		static zedu::Font font( CONSOLE_FONT, CONSOLE_FONT_HEIGHT/2, CONSOLE_FONT_HEIGHT );

		hEdit = CreateWindow( "edit", NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL,
			0, height-INPUT_BOX_HEIGHT, width, INPUT_BOX_HEIGHT, hWnd, (HMENU)1, hInst, NULL );

		SendMessage( hEdit, WM_SETFONT, (WPARAM)(HFONT)font, 1 );
		ShowWindow( hEdit, SW_SHOW );
		SetFocus( hEdit );

		bInit = true;
	}

	GetMainConsole().SetScreenHeight( (height-INPUT_BOX_HEIGHT) / GetMainConsole().GetLineHeight() );
	SetWindowPos( hEdit, NULL, -2, height-INPUT_BOX_HEIGHT+2, width+6,INPUT_BOX_HEIGHT, 0 );
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
	case WM_ACTIVATE:
		SetFocus( hEdit );
		break;

	case WM_SIZE:
		onWmSize( LOWORD(lParam), HIWORD(lParam) );
		break;

	case WM_COMMAND:
		// 콘솔 입력처리
		if( HIWORD(wParam) == EN_UPDATE )
		{
			char buf[512];
			GetWindowText( hEdit, buf, sizeof(buf) );
			buf[sizeof(buf)-1] = 0;

			if( strstr( buf, "\r\n" ) )
			{
				SetWindowText( hEdit, "" );
				std::string strCommand( buf );
				zedu::Replace( strCommand, "\r\n", "\n" );
				ProcCommand( strCommand.c_str() );

				InvalidateRect( hWnd, NULL, false );
			}
		}
		break;

	case WM_CLOSE:
		PostQuitMessage( 0 );
		break;

	case WM_PAINT:
		hdc = BeginPaint( hWnd, &ps );
		DrawConsole( hdc, &GetMainConsole() );
		EndPaint( hWnd, &ps );
		break;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

LRESULT CALLBACK KeyboardProc( int code, WPARAM wParam, LPARAM lParam )
{
	static bool bShift, bAlt, bCtrl;

	if( HIWORD( lParam ) & KF_UP )
	{
		if( LOWORD( wParam ) == VK_SHIFT   )	{ bShift = false;	return 0L; }
		if( LOWORD( wParam ) == VK_MENU    )	{ bAlt   = false;	return 0L; }
		if( LOWORD( wParam ) == VK_CONTROL )	{ bCtrl  = false;	return 0L; }
	}
	else
	{
		if( LOWORD( wParam ) == VK_SHIFT   )	  bShift = true;
		if( LOWORD( wParam ) == VK_MENU    )	  bAlt   = true;
		if( LOWORD( wParam ) == VK_CONTROL )	  bCtrl  = true;	
	}

	if( ! ( HIWORD( lParam ) & KF_REPEAT ) ) return 0;

	Console* pConsole = &GetMainConsole();
	switch( LOWORD( wParam ) )
	{
		case VK_HOME:	if( bCtrl ) pConsole->ScreenHome();						break;
		case VK_END :	if( bCtrl ) pConsole->ScreenEnd();						break;
		case VK_PRIOR:	pConsole->ScreenPageUp();								break;
		case VK_NEXT:	pConsole->ScreenPageDown();								break;
		case VK_UP :	if( bCtrl ) pConsole->ScreenUp(); break;
		case VK_DOWN:	if( bCtrl ) pConsole->ScreenDown(); break;
	}

	bool bEraseFlag = false;
	InvalidateRect( hWnd, NULL, bEraseFlag );

	return 0;
}
