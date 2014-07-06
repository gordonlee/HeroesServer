#include <winsock2.h>
#include "Core/logger.h"
#include "Core/object_remover.h"
#include "Core/types.h"
#include "Game/game_env.h"
#include "Game/game_message.h"
#include <list>
#include "Network/acceptor.h"
#include "Network/console.h"
#include "Network/connection.h"
#include "Network/iocp.h"
#include "Thread/thread.h"
#include "Util/gdi_helper.h"
#include "Util/string_util.h"
#include <vector>
#include <Windows.h>
#include <windowsx.h>

using namespace zedu;
using namespace std;

HWND hWnd;
HWND hEdit;
HINSTANCE hInst;
TCHAR szTitle[] = "RoguelikeSrv";
TCHAR szWindowClass[] = "RoguelikeSrv";

const int INPUT_BOX_HEIGHT = 20; // 하단의 입력창 세로 크기
const int CONSOLE_FONT_HEIGHT = 14;	// 콘솔 폰트 세로 크기
const char* CONSOLE_FONT = "Lucida Console";

long g_connectionNumber = 0;
int g_gameServerViewMode = 0; // 0:main, 1:status
std::list<IOCPConnection*> g_connList;

ObjectRemover<IOCPConnection*> g_objectRemover;
CriticalSection g_logCS;
CriticalSection g_connectionCS;


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

void SendPacket( ISocketConnection* pConnection, MSG_Value* pMsg )
{
	IOCPConnection* pConn = static_cast<IOCPConnection*>( pConnection );
	const char* peerAddr = pConn->GetPeerAddress().GetAddr();
	SOCKET sock = pConn->GetSocketHandle();
	int packetSize = pMsg->size+sizeof(MSG_Value);
	
	cprint( "(%s)[%s](%d) send packet(%d bytes), header=[%d,%d,%d] data=[] \n", 
			GetThreadName(), peerAddr, sock, packetSize, pMsg->size, pMsg->flag, pMsg->checksum );

	pConn->Write( pMsg, packetSize );
}



// 이벤트 핸들러
struct MyEventReceiver : public INetworkEventReceiver
{
	virtual IConnection* CreateConnection(const Socket& sock )
	{
		THREAD_SYNC( g_connectionCS );
		InterlockedIncrement( &g_connectionNumber );

		IOCPConnection* pConn = new IOCPConnection( NULL, sock );
		pConn->SetConnNumber( g_connectionNumber );

		g_connList.push_back( pConn );
		return pConn;

		//IOCPConnection* pConn = NULL;
		//for( list<IOCPConnection*>::iterator it = g_connList.begin(); it != g_connList.end(); it++ )
		//{
		//	if( (*it)->IsConnected() )
		//		continue;
		//	
		//	pConn = (*it);
		//	break;
		//}
		//
		//if( pConn == NULL )
		//{
		//	
		//}
		//else
		//{
		//	
		//	pConn->SetSocket( sock );
		//	pConn->Init( NULL );
		//}
		//return pConn;
	}

	virtual void OnDisconnect( int threadId, IConnection* pConnection )
	{
		THREAD_SYNC( g_connectionCS );

		IOCPConnection* pConn = static_cast<IOCPConnection*>( pConnection );
		const char* peerAddr = pConnection->GetPeerAddress().GetAddr();
		SOCKET sock = pConn->GetSocketHandle();

		cprint( "(%s)[%s](#%d, %d) connection closed(%d) \n", GetThreadName(), peerAddr, pConn->GetConnNumber(), sock, pConn->GetDisconnectReason() );
		InvalidateRect( hWnd, NULL, false );
		
		g_connList.remove( pConn );
		g_objectRemover.AddObject( pConn );
	}

	virtual bool OnAccept( int threadId, IAcceptor* pAcceptor, IConnection* pConnection )
	{
		IOCPConnection* pConn = static_cast<IOCPConnection*>( pConnection );
		const char* peerAddr = pConnection->GetPeerAddress().GetAddr();
		SOCKET sock = pConn->GetSocketHandle();

		cprint( "(%s)[%s](#%d, %d) connection accepted() \n", GetThreadName(), peerAddr, pConn->GetConnNumber(), sock );
		InvalidateRect( hWnd, NULL, false );
		return true;
	}

	virtual void OnRead( int threadId, IConnection* pConnection, uint32 size )
	{
		IOCPConnection* pConn = static_cast<IOCPConnection*>( pConnection );
		const char* peerAddr = pConnection->GetPeerAddress().GetAddr();
		SOCKET sock = pConn->GetSocketHandle();
		const char* threadName = GetThreadName();
		cprint( "(%s)[%s](#%d, %d) >> recv %d bytes \n", threadName, peerAddr,  pConn->GetConnNumber(), sock, size );

		MSG_Value msg;
		int readBytes = pConn->Peek( &msg, sizeof(MSG_Value) );
		if( readBytes < sizeof(MSG_Value) )
			return;

		int packetSize = sizeof(MSG_Value)+msg.size;
		if( pConn->Reserve( packetSize ) )
		{
			cprint( "(%s)[%s](#%d, %d) reserve %d bytes \n", threadName, peerAddr, pConn->GetConnNumber(), sock, packetSize );
		}

		if( pConn->Size() < msg.size )
			return;

		zedu::byte* pBuf = pConn->GetBuf();
		MSG_Value* pMsg = reinterpret_cast<MSG_Value*>( pBuf );
		zedu::byte* pData = reinterpret_cast<zedu::byte*>(pMsg+1);
		

		std::string str("..");
		if( pMsg->size <= 64 )
		{
			str = pData;
			zedu::Replace( str, "\n", "" );
		}

		if( pMsg->size != 65532 || pMsg->flag != 1 || pMsg->checksum != 0x55 )
		{
			int nil = 0;
		}
		
		cprint( "(%s)[%s](#%d, %d) recv packet(%d bytes), header=[%d,%d,%d] data=[] \n", 
			GetThreadName(), peerAddr, pConn->GetConnNumber(), sock, pConn->Size(), pMsg->size, pMsg->flag, pMsg->checksum );

		SendPacket( pConn, pMsg );

		pConn->Read( NULL, pConn->Size() );

		InvalidateRect( hWnd, NULL, false );
	}

	virtual void OnWrite( int threadId, IConnection* pConnection, uint32 size )
	{
		IOCPConnection* pConn = static_cast<IOCPConnection*>( pConnection );
		const char* peerAddr = pConnection->GetPeerAddress().GetAddr();
		SOCKET sock = pConn->GetSocketHandle();
		cprint( "(%s)[%s](#%d, %d) << send %d bytes \n", GetThreadName(), peerAddr, pConn->GetConnNumber(), sock, size );

		InvalidateRect( hWnd, NULL, false );
	}

} g_eventReceiver;

// IOCP 객체
IOCP			g_iocp( &g_eventReceiver );
IOCPAcceptor*	g_pAcceptor;


void output( const char* str )
{
	THREAD_SYNC( g_logCS );
	
	Console::DefaultConsole().PrintStr( str );
	OutputDebugStringA( str );
}

void StartServer()
{
	Logger::SetOutputHandler( output );
	cprint("Starting server ... \n");

	WSADATA wsa;
	if( WSAStartup( MAKEWORD(2,2), &wsa) )
	{
		return;
	}

	g_pAcceptor = new IOCPAcceptor( 100 );
	cprint( "Initializing IOCP ... ", g_iocp.Create() ? "ok" : "FAILED" );

	g_iocp.StartThreadPool( GAME_SERVER_THREAD_NUMBER );

	g_pAcceptor->StartAccept( GAME_SERVER_PORT );
	g_iocp.AddObject( g_pAcceptor );

	cprint("ok\n");

	cprint("\n");
	cprint( "F1 : main screen \n" );
	cprint( "F2 : status screen \n" );
}

void ShutdownServer()
{
	cprint("Shutdown ... \n");

	for( list<IOCPConnection*>::iterator it = g_connList.begin(); it != g_connList.end(); it++ )
	{
		(*it)->Close();
	}
	g_connList.clear();

	g_iocp.EndThreadPool();
	g_pAcceptor->EndAccept();

	PostMessage( hWnd, WM_CLOSE, 0, 0 );
	cprint("ok\n");
}

void UpdateServer()
{
	if( g_objectRemover.Count() > 256 )
	{
		g_objectRemover.DelObject();
	}
}

void ProcCommand( const char* buf )
{
	cprint( buf );

	std::vector<std::string> tokens;
	Split( buf, tokens, " \n", true );

	if( !tokens.empty() )
	{
		if( tokens[0] == "!exit" )			{ ShutdownServer(); }
		else if( tokens[0] == "!clear" )	{ GetMainConsole().Clear(); }
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

		   UpdateServer();
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
	static Canvas_W32 canvas( hDC, 1600, 1200, CONSOLE_FONT_HEIGHT, RGB(255,255,255), RGB(68,68,68) );
	
	RECT rc;
	GetClientRect( hWnd, &rc );
	rc.bottom -= (INPUT_BOX_HEIGHT + 2);

	pConsole->Draw<Canvas_W32>( canvas, hDC, &rc );
}

void DrawStatus(HDC hDC)
{
	static Console statConsole;
	statConsole.Clear();

	statConsole.Printf( "Current thread count = %d\n", g_iocp.GetCurrentThreadCount() );
	statConsole.Printf( "Working thread count = %d\n", g_iocp.GetWorkingThreadCount() );

	statConsole.Printf( "\n" );
	int i = 1;
	for( list<IOCPConnection*>::iterator it = g_connList.begin(); it != g_connList.end(); it++ )
	{
		IOCPConnection* pConn = *it;
		statConsole.Printf("%d. #%d conn[%d]=%s, total bytes(r/w)=(%d/%d), pending r/w=(%d/%d) \n", 
			i++, pConn->GetConnNumber(), pConn->GetSocketHandle(),
			pConn->IsConnected() ? "connected" : "disconnected",
			pConn->GetTotalRecvBytes(), pConn->GetTotalSendBytes(),
			pConn->GetPendingRecvCount(), pConn->GetPendingSendCount()
			);
	}
	
	DrawConsole( hDC, &statConsole );
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

		if( g_gameServerViewMode == 0 )
		{
			DrawConsole( hdc, &GetMainConsole() );
		}
		else if( g_gameServerViewMode == 1 )
		{
			DrawStatus( hdc );
		}
		
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
		case VK_F1:		g_gameServerViewMode = 0; break;
		case VK_F2:		g_gameServerViewMode = 1; break;
	}

	bool bEraseFlag = false;
	InvalidateRect( hWnd, NULL, bEraseFlag );

	return 0;
}