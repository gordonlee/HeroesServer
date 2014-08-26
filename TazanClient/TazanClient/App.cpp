#include "App.h"
#include "AppMsgProc.h"
#include "Utility.h"

#include "Display.h"
#include "Images.h"

#include "UserInfo.h"

#include "Packet.h"

HWND g_hWnd;
HINSTANCE g_hInst;
SOCKET g_Socket;

UserInfo g_MyUserInfo;
list<UserInfo*> g_UserInfoList;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
		  , LPSTR lpszCmdParam, int nCmdShow)
{
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;

	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	if(::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput,NULL)!=Ok)
	{
		return 0;
	}
	
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(0,0,0));
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)AppMsgProc;
	WndClass.lpszClassName=App_ClassName;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	// ������ ũ�Ⱑ Ŭ���̾�Ʈ ������ �������� ����������� �Ѵ�.
	RECT appClientRect = { 0, 0, g_appWidth, g_appHeight };
	AdjustWindowRect(&appClientRect, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWnd=CreateWindow(App_ClassName, App_Name, WS_CAPTION | WS_SYSMENU,
		  CW_USEDEFAULT, CW_USEDEFAULT,
		  appClientRect.right - appClientRect.left, appClientRect.bottom - appClientRect.top,
		  NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(g_hWnd, nCmdShow);

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	{
		// Config ������ �н��ϴ�.
		const char* configFileName = "TazanClient.config";
		FILE* configFile = NULL;
		fopen_s(&configFile, configFileName, "r");

		if (configFile == NULL)
		{
			MessageBox(g_hWnd, TEXT("Config Open Failed"), TEXT("Critical Error"), MB_ICONERROR);
			return -1;
		}

		char server_ip[32];
		char server_port[32];

		fgets(server_ip, 31, configFile);
		fgets(server_port, 31, configFile);

		server_ip[strlen(server_ip)-1] = '\0';

		fclose(configFile);

		// ������ �����մϴ�.
		int nResult = 0;

		WSADATA wsadata;
		nResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (nResult != 0)
		{
			MessageBox(g_hWnd, TEXT("Winsock Initialization Failed!"), TEXT("Critical Error"), MB_ICONERROR);
			return -1;
		}

		g_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (g_Socket == INVALID_SOCKET)
		{
			MessageBox(g_hWnd, TEXT("Socket Creation Failed"), TEXT("Critical Error"), MB_ICONERROR);
			return -1;
		}

		nResult = WSAAsyncSelect(g_Socket, g_hWnd, WM_SOCKET, (FD_CLOSE | FD_READ));
		if (nResult != 0)
		{
			MessageBox(g_hWnd, TEXT("WSAAsyncSelect Failed"), TEXT("Critical Error"), MB_ICONERROR);
			return -1;
		}

		hostent* host;
		if ((host = gethostbyname(server_ip)) == NULL)
		{
			MessageBox(g_hWnd, TEXT("Unable to resolve host name"), TEXT("Critical Error"), MB_ICONERROR);
			return -1;
		}

		SOCKADDR_IN SockAddr;
		SockAddr.sin_port = htons(atoi(server_port));
		SockAddr.sin_family = AF_INET;
		SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

		nResult = connect(g_Socket, (LPSOCKADDR)(&SockAddr), sizeof(SockAddr));
		if (nResult == SOCKET_ERROR)
		{
			//MessageBox(g_hWnd, TEXT("Server Connect Failed"), TEXT("Critical Error"), MB_ICONERROR);
			//return -1;
		}
		else
		{
			LoginRequestMessage msg;
			send(g_Socket, (char*)&msg, msg.DataSize + 4, 0);
		}
		LoginRequestMessage msg;
		send(g_Socket, (char*)&msg, msg.DataSize + 4, 0);
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	// �̹������� �ҷ��ɴϴ�.
	if(!Images::Loading())
	{
		MessageBox(g_hWnd, TEXT("Loading Fail"), TEXT("Error"), MB_OK);
		return 1;
	}
	// �÷�Ű�� �����մϴ�.
	ImageAttributes* colorKey = new ImageAttributes;
	colorKey->SetColorKey(Color(255, 0, 255), Color(255, 0, 255), ColorAdjustType::ColorAdjustTypeBitmap);
	// ��Ʈ�� �����մϴ�.
	Font* font = new Font(TEXT("Arial"), 10);
	StringFormat* stringFormat = new StringFormat;
	stringFormat->SetAlignment(StringAlignment::StringAlignmentCenter);
	stringFormat->SetLineAlignment(StringAlignment::StringAlignmentCenter);
	SolidBrush* blackBrush = new SolidBrush(Color(100, 0, 0, 0));
	SolidBrush* whiteBrush = new SolidBrush(Color(255, 255, 255));

	// ���÷��� Ŭ������ �ν��Ͻ��� ���ɴϴ�.
	Display *Ds = Ds->GetInstance();
	// ���콺 ��ǥ�� ������ ������ �����մϴ�.
	POINT mousePos;

	// �������� ������ ������ �����մϴ�.
	DWORD startTime = timeGetTime();

	// Temp UserInfo Setting
	/*g_MyUserInfo.UserID = 1;
	g_MyUserInfo.X = 2;
	g_MyUserInfo.Y = 2;
	g_MyUserInfo.UserDirection = Direction::Down;

	g_UserInfoList.push_back(new UserInfo(2, 3, 3, Direction::Up));
	g_UserInfoList.push_back(new UserInfo(2, 4, 3, Direction::Left));
	g_UserInfoList.push_back(new UserInfo(2, 6, 3, Direction::Right));
	g_UserInfoList.push_back(new UserInfo(2, 7, 3, Direction::Down));*/

	// ������ �����մϴ�.
	ZeroMemory(&Message, sizeof(Message));
	while(Message.message != WM_QUIT)
	{
		// ���콺 ��ǥ�� ���մϴ�.
		GetCursorPos(&mousePos);
		ScreenToClient(g_hWnd, &mousePos);

		if(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);

			// ���콺 �Է�ó���� �մϴ�.
			switch (Message.message)
			{
			case WM_LBUTTONDOWN:
			{
				wchar_t buffer[256];
				wsprintf(buffer, TEXT("[X,Y] : [%d,%d]"), mousePos.x, mousePos.y);
				MessageBox(g_hWnd, buffer, TEXT("Click"), MB_OK);
				break;
			}
			}
		}

		DWORD endTime = timeGetTime();
		if (endTime - startTime >= g_fpsElapsed)
		{
			startTime = endTime;

			// ȭ�鿡 �׸��⸦ �����մϴ�.
			Ds->BeginDraw();

			///////////////////////////////////////////////////////////////////////////
			// �ٴ� Ÿ���� �׸��ϴ�.
			for (int i = 0; i < 30; ++i)
			{
				for (int j = 0; j < 30; ++j)
				{
					if (CheckPointInRect(mousePos.x, mousePos.y, i*20, j*20, 20, 20))
					{
						Ds->pBackBuffer->DrawImage(Images::pImgOveredTile, PointF(i * 20.f, j * 20.f));
					}
					else
					{
						Ds->pBackBuffer->DrawImage(Images::pImgTile, PointF(i * 20.f, j * 20.f));
					}
				}
			}
			///////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////
			// �� ĳ���͸� �׸��ϴ�.
			// Ÿ���� ũ��� 20*20�ε� ĳ���� ũ��� 18*20�̹Ƿ� ���� ��ǥ���� x��ǥ�� 1�� ���ؼ� �׷��ݴϴ�.
			Ds->pBackBuffer->DrawImage(Images::pImgCharacter,
				RectF(g_MyUserInfo.X * 20.f + 1, g_MyUserInfo.Y * 20.f, 18.f, 20.f),
				0.f, g_MyUserInfo.UserDirection * 20.f, 18.f, 20.f, Unit::UnitPixel, colorKey);
			///////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////
			// �ٸ� ĳ���͸� �׸��ϴ�.
			for (auto& it : g_UserInfoList)
			{
				Ds->pBackBuffer->DrawImage(Images::pImgCharacter,
					RectF(it->X * 20.f + 1, it->Y * 20.f, 18.f, 20.f),
					0.f, it->UserDirection * 20.f, 18.f, 20.f, Unit::UnitPixel, colorKey);
			}
			///////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////
			// ���콺�� �� ĳ���� ���� �ִٸ� �� ĳ���� ������ �����ݴϴ�.
			if (CheckPointInRect(mousePos.x, mousePos.y, g_MyUserInfo.X * 20, g_MyUserInfo.Y * 20, 20, 20))
			{
				wchar_t buf[256];
				wsprintf(buf, TEXT("[ID(X,Y) : Direction]\r\n[%d(%d,%d) : %s]"), g_MyUserInfo.UserID, g_MyUserInfo.X, g_MyUserInfo.Y, GetDirectionToString(g_MyUserInfo.UserDirection));
				Ds->pBackBuffer->FillRectangle(blackBrush, Rect(mousePos.x, mousePos.y, 150, 50));
				Ds->pBackBuffer->DrawString(buf, -1, font, RectF(mousePos.x, mousePos.y, 150, 50), stringFormat, whiteBrush);
			}
			///////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////
			// ���콺�� �ٸ� ĳ���� ���� �ִٸ� �ٸ� ĳ���� ������ �����ݴϴ�.
			else for (auto& it : g_UserInfoList)
			{
				if (CheckPointInRect(mousePos.x, mousePos.y, it->X * 20, it->Y * 20, 20, 20))
				{
					wchar_t buf[256];
					wsprintf(buf, TEXT("[ID(X,Y) : Direction]\r\n[%d(%d,%d) : %s]"), it->UserID, it->X, it->Y, GetDirectionToString(it->UserDirection));
					Ds->pBackBuffer->FillRectangle(blackBrush, Rect(mousePos.x, mousePos.y, 150, 50));
					Ds->pBackBuffer->DrawString(buf, -1, font, RectF(mousePos.x, mousePos.y, 150, 50), stringFormat, whiteBrush);
					break;
				}
			}
			///////////////////////////////////////////////////////////////////////////

			// ȭ�鿡 �׸��⸦ �����մϴ�.
			Ds->EndDraw();
		}
		
		Sleep(1);
	}

	// ���ҽ��� �����մϴ�.
	delete font;
	delete stringFormat;
	delete whiteBrush;
	delete blackBrush;
	delete colorKey;

	// ������ �����մϴ�.
	closesocket(g_Socket);

	// ���÷��� Ŭ������ �ν��Ͻ��� ��ȯ�մϴ�.
	Ds->Release();

	// �̹������� ��ȯ�մϴ�.
	Images::Release();

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////



	::GdiplusShutdown(gdiplusToken);

	return Message.wParam;
}