#include <winsock2.h>
#include "Core/types.h"
#include "Core/logger.h"
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
TCHAR szTitle[] = "StressClnt";
TCHAR szWindowClass[] = "StressClnt";

const int INPUT_BOX_HEIGHT = 20; // 하단의 입력창 세로 크기
const int CONSOLE_FONT_HEIGHT = 14;	// 콘솔 폰트 세로 크기
const char* CONSOLE_FONT = "Lucida Console";

int g_workerCount = 32;
int g_framePerSec = 20;
int g_frameCount = 0;
int g_closeRatio = 15;
int g_sendPerFrame = 1;
bool g_bWork = false;

Addr g_addr( GAME_SERVER_IP, GAME_SERVER_PORT );

int g_viewMode = 0; // 0:main, 1:status
long g_connNumber = 0;

class StressWorker : public SyncStreamConnection
{
public:
	IQueue* m_pReadQueue;
	zedu::byte* m_pSendBuf;
	int m_sendLen;

	bool m_bPacketResolved;

	int m_checksumErrorCount;
	int m_memcmpErrorCount;
	long m_connNumber;

public:
	StressWorker()
	{
		m_pReadQueue = IQueue::CreateQueue( 8192 );
		m_pReadQueue->Reserve( 65536 );

		m_sendLen = 0;
		m_pSendBuf = new char[65536];
		m_bPacketResolved = true;

		m_checksumErrorCount = 0;
		m_memcmpErrorCount = 0;
	}

	virtual ~StressWorker()
	{
		delete m_pSendBuf ;
		delete m_pReadQueue;
	}

	virtual bool Connect( const Addr& addr )
	{
		InterlockedIncrement( &g_connNumber );
		m_connNumber = g_connNumber;
		return SyncStreamConnection::Connect( addr );
	}
	virtual int Write( const void* pBuf, size_t len )
	{
		m_bPacketResolved = false;
		m_sendLen = len;
		memcpy( m_pSendBuf, pBuf, len );

		int rtn = SyncStreamConnection::Write( m_pSendBuf, len );
		//m_pSendBuf[len-1] = '0xFE';
		return rtn;
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

void output( const char* str )
{
	Console::DefaultConsole().PrintStr( str );
	OutputDebugStringA( str );
}


void StartClient()
{
	Logger::SetOutputHandler( output );
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

void ReadFromNetwork()
{
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

				cprint( "[%s](#%d, %d) >> recv %d bytes \n", peerAddr, pWorker->m_connNumber, sock, readBytes );
				pWorker->m_pReadQueue->Write( buf, readBytes );

				if( readBytes < sizeof(MSG_Value) ) 
					continue;

				MSG_Value msg;
				pWorker->m_pReadQueue->Peek( &msg, sizeof(MSG_Value) );
				int packetSize = sizeof(MSG_Value)+msg.size;

				if( pWorker->m_pReadQueue->Size() < packetSize )
					continue;

				cprint( "[%s](#%d, %d) >> recv packet(%d bytes)\n", peerAddr, pWorker->m_connNumber, sock, packetSize );
				

				// 유효성 체크
				bool bChecksumOk = msg.checksum == 0x55;
				bool bMemcmpOk = ::memcmp( pWorker->m_pSendBuf, pWorker->m_pReadQueue->GetBuf(), packetSize ) == 0;

				if( !bChecksumOk ) pWorker->m_checksumErrorCount++;
				if( !bMemcmpOk ) pWorker->m_memcmpErrorCount++;

				cprint( "[%s](#%d, %d) >> checksum %s, memcmp %s \n", peerAddr, pWorker->m_connNumber, sock, 
					bChecksumOk ? "ok" : "FAILED", bMemcmpOk ? "ok" : "FAILED" );

				pWorker->m_pReadQueue->Read( NULL, packetSize );
				pWorker->m_bPacketResolved = true;
			}
			else
			{
				(*it)->Close();

				const char* peerAddr = (*it)->GetPeerAddress().GetAddr();
				SOCKET sock = (*it)->GetSocketHandle();
				GetMainConsole().Printf( "[%s](#%d, %d) connectio closed(from server)\n", peerAddr, pWorker->m_connNumber, sock );
			}

			SyncStreamConnection::Select( connectionList );
		}
	}
}

void UpdateClient()
{
	if( !g_bWork )
		return;

	// PerFrame, connect & close
	bool bPerFrameConnect = false;

	for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
		it != connectionList.end(); it++ )
	{
		if( (*it)->IsConnected() )
		{
		}
		else if( !bPerFrameConnect )
		{
			if( (*it)->Connect( g_addr ) )
			{
				const char* peerAddr = (*it)->GetPeerAddress().GetAddr();
				SOCKET sock = (*it)->GetSocketHandle();
				cprint( "[%s](%d) connection accpeted \n", peerAddr, sock );
			}
			bPerFrameConnect = true;
		}
	}
}

void SendPacket( ISocketConnection* pConn, zedu::byte* pBuf, int len )
{
	StressWorker* pWorker = dynamic_cast<StressWorker*>(pConn);
	if( !pWorker->m_bPacketResolved )
		return;

	zedu::byte* pSendBuf = (zedu::byte*)new char[len+sizeof(MSG_Value)];

	MSG_Value* pMsg = reinterpret_cast<MSG_Value*>(pSendBuf);
	new( pMsg ) MSG_Value();

	pMsg->size = len;
	pMsg->flag = 0x01/*(rand() % 2)+1*/;
	pMsg->checksum = 0x55;

	memcpy( pMsg+1, pBuf, len );

	const char* peerAddr = pConn->GetPeerAddress().GetAddr();
	SOCKET sock = pConn->GetSocketHandle();
	int packetSize = sizeof(MSG_Value)+pMsg->size;

	pConn->Write( pSendBuf, pMsg->size + sizeof(MSG_Value) );

	cprint( "[%s](%d) << send packet(%d bytes), header=[%d][%d][%d] data=[] \n", 
		peerAddr, sock, packetSize, pMsg->size, pMsg->flag, pMsg->checksum );

	delete pSendBuf;
}

void DisconnectPerSec()
{
	if( !g_bWork )
		return;

	for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
		it != connectionList.end(); it++ )
	{
		if( (*it)->IsConnected() )
		{
			int v = rand() % 100;
			if( v < g_closeRatio )
			{
				(*it)->Close();

				const char* peerAddr = (*it)->GetPeerAddress().GetAddr();
				SOCKET sock = (*it)->GetSocketHandle();
				cprint( "[%s](%d) connection closed(from client)\n", peerAddr, sock );
			}
		}
	}
}

void SendPerSec()
{
	if( !g_bWork )
		return;

	for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
		it != connectionList.end(); it++ )
	{
		SyncStreamConnection* pConn = *it;

		if( pConn->IsConnected() )
		{
			int dataLen = rand() % 65532;
			char* pData = new char[dataLen];
			SendPacket( pConn, (zedu::byte*)pData, dataLen );
			delete pData;
		}
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
		else if( tokens[0] == "!connect" )
		{
			SyncStreamConnection* pConn = NULL;
			for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
				it != connectionList.end(); it++ )
			{
				if( !(*it)->IsConnected() )
				{
					pConn = *it;
					break;
				}
			}

			if( pConn == NULL )
			{
				pConn = new SyncStreamConnection();
			}
			pConn->Connect( g_addr );
		}
		else if( tokens[0] == "!disconnect" )
		{
			SyncStreamConnection* pConn = NULL;
			for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
				it != connectionList.end(); it++ )
			{
				if( (*it)->IsConnected() )
				{
					pConn = *it;
					break;
				}
			}

			if( pConn )
			{
				pConn->Close();
			}
		}
		else if( tokens[0] == "!send" )
		{
			SyncStreamConnection* pConn = NULL;
			for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
				it != connectionList.end(); it++ )
			{
				if( (*it)->IsConnected() )
				{
					pConn = *it;
					break;
				}
			}
			if( pConn )
			{
				int dataLen = 65532;
				char* pData = new char[dataLen];
				SendPacket( pConn, (zedu::byte*)pData, dataLen );
				delete pData;
			}
		}
		else if( tokens[0] == "!start" )
		{
			g_bWork = true;

			for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
					it != connectionList.end(); it++ )
			{
				StressWorker* pWorker = dynamic_cast<StressWorker* >(*it);
				pWorker->m_bPacketResolved = true;
			}
		}
		else if( tokens[0] == "!stop" )
		{
			g_bWork = false;
		}
		else if( tokens[0] == "!close" )
		{
			SyncStreamConnection* pConn = NULL;
			for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
				it != connectionList.end(); it++ )
			{
				if( (*it)->IsConnected() )
				{
					pConn = *it;
					break;
				}
			}
			if( pConn )
			{
				int dataLen = 65532;
				char* pData = new char[dataLen];
				SendPacket( pConn, (zedu::byte*)pData, dataLen );
				delete pData;

				pConn->Close();
			}
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

			// 네트웍 데이터 읽어온다
			ReadFromNetwork();

			// 업데이트로직은 FramePerSec 만큼만 돌린다
			if( g_frameCount++ < g_framePerSec )
			{
				UpdateClient();

				if( g_frameCount % 2 > 0 )
				{
					for( int i = 0; i < g_sendPerFrame; i++ )
					{
						SendPerSec();
					}
				}
			}

			if( GetMiliSeconds() - elapsedTime > 1000 )
			{
				DisconnectPerSec();
				// PerSec, send 
				
				elapsedTime = GetMiliSeconds();
				g_frameCount = 0;
				InvalidateRect( hWnd, NULL, false );
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

void DrawStatus(HDC hdc)
{
	static Console statusConsole;
	statusConsole.Clear();

	int i = 1;
	for( std::vector<SyncStreamConnection*>::iterator it = connectionList.begin(); 
		it != connectionList.end(); it++ )
	{
		SyncStreamConnection* pConn = *it;
		StressWorker* pWorker = dynamic_cast<StressWorker*>(pConn);

		statusConsole.Printf("%d.(#%d)(%d)=%s total recv/send=(%d/%d), error checksum/memcmp(%d/%d) \n", 
			i++, pWorker->m_connNumber, pConn->GetSocketHandle(), 
			pConn->IsConnected() ? "connected" : "disconnected", 
			pConn->GetTotalRecvBytes(), pConn->GetTotalSendBytes(),
			pWorker->m_checksumErrorCount, pWorker->m_memcmpErrorCount
			);
	}
	DrawConsole( hdc, &statusConsole );
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

		if( g_viewMode == 0 )		DrawConsole( hdc, &GetMainConsole() );
		else if( g_viewMode == 1)	DrawStatus( hdc );
		
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
	case VK_F1:		g_viewMode = 0; break;
	case VK_F2:		g_viewMode = 1; break;
	}

	bool bEraseFlag = false;
	InvalidateRect( hWnd, NULL, bEraseFlag );
	return 0;
}