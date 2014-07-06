#include <winsock2.h>
#include "Core/logger.h"
#include "Core/types.h"
#include "Game/game_message.h"
#include "Game/game_env.h"
#include "Network/console.h"
#include "Network/queue.h"
#include "Network/syncstream_connection.h"
#include <string>
#include "Util/string_util.h"
#include "Util/timer.h"
#include "Util/w32_util.h"
#include <vector>
#include <Windows.h>

using namespace zedu;

HWND hWnd;
HWND hEdit;
HINSTANCE hInst;
TCHAR szTitle[] = "RoguelikeClnt";
TCHAR szWindowClass[] = "RoguelikeClnt";

const int INPUT_BOX_HEIGHT = 20; // 하단의 입력창 세로 크기
const int CONSOLE_FONT_HEIGHT = 14;	// 콘솔 폰트 세로 크기
const char* CONSOLE_FONT = "Lucida Console";

int g_workerCount = 10;
int g_framePerSec = 20;
int g_frameCount = 0;
int g_closeRatio = 10;
int g_sendPerFrame = 1;
Addr g_addr( GAME_SERVER_IP, GAME_SERVER_PORT );


class StressWorker : public SyncStreamConnection
{
public:
	IQueue* m_pReadQueue;

public:
	StressWorker()
	{
		m_pReadQueue = IQueue::CreateQueue( 8192 );
		m_pReadQueue->Reserve( 65536 );
	}

	virtual ~StressWorker()
	{
		delete m_pReadQueue;
	}
};



std::vector<SyncStreamConnection*> connectionList;



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc( int code, WPARAM wParam, LPARAM lParam );

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

void __print( const char* str )
{
	Console::DefaultConsole().PrintStr( str );
	OutputDebugStringA( str );
}

void StartClient()
{
	Logger::SetOutputHandler( __print );
	cprint("Starting client ... \n");

	StartMiliSeconds();

	WSADATA wsa;
	if( WSAStartup( MAKEWORD(2,2), &wsa) )
	{
		return;
	}

	for(int i = 0; i < g_workerCount; i++)
	{
		StressWorker* pConnection = new StressWorker();
		connectionList.push_back( pConnection );
	}
	cprint("ok\n");
}

void ShutdownClient()
{
	cprint("Shutdown ... \n");

	for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
		it != connectionList.end(); it++ )
	{
		if( (*it)->IsConnected() )
		{
			(*it)->Close();
		}
		delete (*it);
	}
	connectionList.clear();

	PostMessage( hWnd, WM_CLOSE, 0, 0 );
	cprint("ok\n");
}

void UpdateClient()
{
	// PerFrame, connect & close
	bool bPerFrameConnect = false;

	for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
			it != connectionList.end(); it++ )
	{
		if( (*it)->IsConnected() )
		{
			if( (rand() % 100) > g_closeRatio )
			{
				(*it)->Close();

				const char* peerAddr = (*it)->GetPeerAddress().GetAddr();
				SOCKET sock = (*it)->GetSocketHandle();
				GetMainConsole().Printf( "[%s](%d) connectio closed(from client)\n", peerAddr, sock );
			}
		}
		else if( !bPerFrameConnect )
		{
			if( (*it)->Connect( g_addr ) )
			{
				const char* peerAddr = (*it)->GetPeerAddress().GetAddr();
				SOCKET sock = (*it)->GetSocketHandle();
				GetMainConsole().Printf( "[%s](%d) connection accpeted \n", peerAddr, sock );
			}
			bPerFrameConnect = true;
		}
	}

	// frame read
	SyncStreamConnection::Select( connectionList );
	for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
			it != connectionList.end(); it++ )
	{
		while( (*it)->IsReadable() )
		{
			zedu::byte buf[65536];

			StressWorker* pWorker = dynamic_cast<StressWorker*>(*it);
			if( pWorker == NULL )
				continue;

			int readBytes = (*it)->Read( buf, 65536 );
			
			if( readBytes > 0 )
			{
				//buf[readBytes] = '\0';
				MSG_Value* pMsg = reinterpret_cast<MSG_Value*>( buf );
				zedu::byte* pData = reinterpret_cast<zedu::byte*>( pMsg+1 );
				const char* peerAddr = (*it)->GetPeerAddress().GetAddr();
				SOCKET sock = (*it)->GetSocketHandle();

				GetMainConsole().Printf( "[%s](%d) >> recv %d bytes \n", peerAddr, sock, readBytes );
				pWorker->m_pReadQueue->Write( buf, readBytes );

				if( readBytes < sizeof(MSG_Value) ) 
					continue;

				MSG_Value msg;
				pWorker->m_pReadQueue->Peek( &msg, sizeof(MSG_Value) );
				int packetSize = sizeof(MSG_Value)+msg.size;

				if( pWorker->m_pReadQueue->Size() < packetSize )
					continue;

				GetMainConsole().Printf( "[%s](%d) >> recv packet(%d bytes)\n", peerAddr, sock, packetSize );
				pWorker->m_pReadQueue->Read( NULL, packetSize );

				//GetMainConsole().Printf( "[%s](%d) >> recv packet(%d bytes), header=[%d][%d][%d] data=[] \n", 
				//	peerAddr, sock, pMsg->size, pMsg->flag, pMsg->checksum );
			}
			else
			{
				(*it)->Close();

				const char* peerAddr = (*it)->GetPeerAddress().GetAddr();
				SOCKET sock = (*it)->GetSocketHandle();
				GetMainConsole().Printf( "[%s](%d) connectio closed(from server)\n", peerAddr, sock );

				//ShutdownClient();
			}

			SyncStreamConnection::Select( connectionList );
		}
	}
}

void SendPacket( ISocketConnection* pConn, zedu::byte* pBuf, int len )
{
	zedu::byte* pSendBuf = (zedu::byte*)new char[len+sizeof(MSG_Value)];

	MSG_Value* pMsg = reinterpret_cast<MSG_Value*>(pSendBuf);
	new( pMsg ) MSG_Value();

	pMsg->size = len;
	pMsg->flag = 0x01;
	pMsg->checksum = 0x55;

	memcpy( pMsg+1, pBuf, len );

	const char* peerAddr = pConn->GetPeerAddress().GetAddr();
	SOCKET sock = pConn->GetSocketHandle();
	int packetSize = sizeof(MSG_Value)+pMsg->size;

	pConn->Write( pSendBuf, pMsg->size + sizeof(MSG_Value) );

	GetMainConsole().Printf( "[%s](%d) << send packet(%d bytes), header=[%d][%d][%d] data=[] \n", 
		peerAddr, sock, packetSize, pMsg->size, pMsg->flag, pMsg->checksum );

	//GetMainConsole().Printf( "<< [%d][%d][%d] %s", 
	//	pMsg->size, pMsg->flag, pMsg->checksum, pBuf );

	delete pSendBuf;
}

void SendPerSec()
{
	for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
			it != connectionList.end(); it++ )
	{
		SyncStreamConnection* pConn = *(connectionList.begin());

		int dataLen = 65532;
		char* pData = new char[dataLen];
		SendPacket( pConn, (zedu::byte*)pData, dataLen );
		delete pData;
	}

}

void ProcCommand( const char* buf )
{
	GetMainConsole().Printf( buf );

	std::vector<std::string> tokens;
	Split( buf, tokens, " \n", true );
	
	if( !tokens.empty() )
	{
		if( tokens[0] == "!exit" )			{ ShutdownClient(); }
		else if( tokens[0] == "!clear" )	{ GetMainConsole().Clear(); }
		else if( tokens[0] == "!packet" )
		{
			SyncStreamConnection* pConn = *(connectionList.begin());

			int dataLen = 65532;
			char* pData = new char[dataLen];
			SendPacket( pConn, (zedu::byte*)pData, dataLen );

			delete pData;
		}
		else
		{
			SyncStreamConnection* pConn = *(connectionList.begin());
			SendPacket( pConn, (zedu::byte*)buf, strlen(buf) );
		}
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

		StartClient();
		InvalidateRect( hWnd, NULL, false );

		ulong elapsedTime = GetMiliSeconds();

		// 메세지루프
		MSG msg;
		while( GetMessage( &msg, NULL, 0, 0 ) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// 업데이트로직은 FramePerSec 만큼만 돌린다
			if( g_frameCount++ < g_framePerSec )
			{
				UpdateClient();
			}

			if( GetMiliSeconds() - elapsedTime > 1000 )
			{
				elapsedTime = GetMiliSeconds();
				g_frameCount = 0;
			}

			// PerSec, send 
			for( int i = 0; i < g_sendPerFrame; i++ )
			{
				SendPerSec();
			}
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
	//RGB(26,58,105)
	//RGB(255,255,255), RGB(68,68,68)
	static Canvas_W32 canvas( hDC, 1600, 1200, CONSOLE_FONT_HEIGHT, RGB(255,255,255), RGB(26,58,105) );
	
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
		case VK_HOME:	if( bCtrl ) pConsole->ScreenHome(); break;
		case VK_END :	if( bCtrl ) pConsole->ScreenEnd(); break;
		case VK_PRIOR:	pConsole->ScreenPageUp(); break;
		case VK_NEXT:	pConsole->ScreenPageDown(); break;
		case VK_UP :	if( bCtrl ) pConsole->ScreenUp(); break;
		case VK_DOWN:	if( bCtrl ) pConsole->ScreenDown(); break;
	}

	bool bEraseFlag = false;
	InvalidateRect( hWnd, NULL, bEraseFlag );
	return 0;
}