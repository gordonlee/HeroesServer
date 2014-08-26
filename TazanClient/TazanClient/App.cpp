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

	// �̹������� �ҷ��ɴϴ�.
	if(!Images::Loading())
	{
		MessageBox(g_hWnd, L"Loading Fail", L"Error", MB_OK);
		return 1;
	}

	// ���÷��� Ŭ������ �ν��Ͻ��� ���ɴϴ�.
	Display *Ds = Ds->GetInstance();
	// ���콺 ��ǥ�� ������ ������ �����մϴ�.
	POINT mousePos;

	// �������� ������ ������ �����մϴ�.
	DWORD startTime = timeGetTime();

	// ������ �����մϴ�.
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

				// ���콺 ��ǥ�� ���մϴ�.
				GetCursorPos(&mousePos);
				ScreenToClient(g_hWnd, &mousePos);

				// ȭ�鿡 �׸��⸦ �����մϴ�.
				Ds->BeginDraw();

				///////////////////////////////////////////////////////////////////////////
				// �ٴ� Ÿ���� �׸��ϴ�.
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
				// �� ĳ���͸� �׸��ϴ�.
				///////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////
				// �ٸ� ĳ���͸� �׸��ϴ�.
				///////////////////////////////////////////////////////////////////////////

				// ȭ�鿡 �׸��⸦ �����մϴ�.
				Ds->EndDraw();
			}
		}
		
		Sleep(1);
	}

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