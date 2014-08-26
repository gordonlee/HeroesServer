#include "App.h"
#include "AppMsgProc.h"
#include "Display.h"
#include "Images.h"
#include "UserInfo.h"

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

	g_hWnd=CreateWindow(App_ClassName, App_Name, WS_CAPTION | WS_SYSMENU,
		  CW_USEDEFAULT, CW_USEDEFAULT,
		  g_appWidth, g_appHeight,
		  NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(g_hWnd, nCmdShow);

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	{
		// Config 파일을 읽습니다.
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

		// 서버에 연결합니다.
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

		connect(g_Socket, (LPSOCKADDR)(&SockAddr), sizeof(SockAddr));
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	// 이미지들을 불러옵니다.
	if(!Images::Loading())
	{
		MessageBox(g_hWnd, L"Loading Fail", L"Error", MB_OK);
		return 1;
	}
	// 컬러키를 설정합니다.
	ImageAttributes colorKey;
	colorKey.SetColorKey(Color(255, 0, 255), Color(255, 0, 255), ColorAdjustType::ColorAdjustTypeBitmap);

	// 디스플레이 클래스의 인스턴스를 얻어옵니다.
	Display *Ds = Ds->GetInstance();
	// 마우스 좌표를 저장할 변수를 생성합니다.
	POINT mousePos;

	// 프레임을 측정할 변수를 생성합니다.
	DWORD startTime = timeGetTime();

	// Temp UserInfo Setting
	g_MyUserInfo.UserID = 1;
	g_MyUserInfo.X = 2;
	g_MyUserInfo.Y = 2;
	g_MyUserInfo.UserDirection = Direction::Down;

	g_UserInfoList.push_back(new UserInfo(2, 3, 3, Direction::Up));
	g_UserInfoList.push_back(new UserInfo(2, 4, 3, Direction::Left));
	g_UserInfoList.push_back(new UserInfo(2, 6, 3, Direction::Right));
	g_UserInfoList.push_back(new UserInfo(2, 7, 3, Direction::Down));

	// 루프를 시작합니다.
	ZeroMemory(&Message, sizeof(Message));
	while(Message.message != WM_QUIT)
	{
        if(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
		}
		else
		{
			DWORD endTime = timeGetTime();
			if (endTime - startTime >= g_fpsElapsed)
			{
				startTime = endTime;

				// 마우스 좌표를 구합니다.
				GetCursorPos(&mousePos);
				ScreenToClient(g_hWnd, &mousePos);

				// 화면에 그리기를 시작합니다.
				Ds->BeginDraw();

				///////////////////////////////////////////////////////////////////////////
				// 바닥 타일을 그립니다.
				for (int i = 0; i < 30; ++i)
				{
					for (int j = 0; j < 30; ++j)
					{
						if (i * 20 <= mousePos.x && mousePos.x < (i + 1) * 20 &&
							j * 20 <= mousePos.y && mousePos.y < (j + 1) * 20)
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
				// 내 캐릭터를 그립니다.
				Ds->pBackBuffer->DrawImage(Images::pImgCharacter, RectF(g_MyUserInfo.X * 20.f, g_MyUserInfo.Y * 20.f, 18.f, 20.f),
					0.f, g_MyUserInfo.UserDirection * 20.f, 18.f, 20.f, Unit::UnitPixel, &colorKey);
				///////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////
				// 다른 캐릭터를 그립니다.
				for (auto& it : g_UserInfoList)
				{
					Ds->pBackBuffer->DrawImage(Images::pImgCharacter, RectF(it->X * 20.f, it->Y * 20.f, 18.f, 20.f),
						0.f, it->UserDirection * 20.f, 18.f, 20.f, Unit::UnitPixel, &colorKey);
				}
				///////////////////////////////////////////////////////////////////////////

				// 화면에 그리기를 종료합니다.
				Ds->EndDraw();
			}
		}
		
		Sleep(1);
	}

	// 디스플레이 클래스의 인스턴스를 반환합니다.
	Ds->Release();

	// 이미지들을 반환합니다.
	Images::Release();

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////



	::GdiplusShutdown(gdiplusToken);

	return Message.wParam;
}