#include "App.h"
#include "AppMsgProc.h"
#include "Display.h"
#include "Images.h"
#include "UserInfo.h"

HWND g_hWnd;
HINSTANCE g_hInst;

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

	// 이미지들을 불러옵니다.
	if(!Images::Loading())
	{
		MessageBox(g_hWnd, L"Loading Fail", L"Error", MB_OK);
		return 1;
	}

	// 디스플레이 클래스의 인스턴스를 얻어옵니다.
	Display *Ds = Ds->GetInstance();
	// 마우스 좌표를 저장할 변수를 생성합니다.
	POINT mousePos;

	// 프레임을 측정할 변수를 생성합니다.
	DWORD startTime = timeGetTime();

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
				///////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////
				// 다른 캐릭터를 그립니다.
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