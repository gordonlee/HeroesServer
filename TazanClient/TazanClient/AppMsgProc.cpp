#include "App.h"
#include "AppMsgProc.h"
#include "Display.h"

// ���� �������� �޽��� ���ν����Դϴ�.
LRESULT CALLBACK AppMsgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_CREATE:
		{
			return 0;
		}

	case WM_ERASEBKGND:
		// WM_PAINT �޽����� ���� �� ����� ������ �ʵ����մϴ�.
		// �������� ���ֱ� ���ؼ��Դϴ�.
		{
			return 1;
		}

	case WM_DESTROY:
		{
			PostQuitMessage(0);

			return 0;
		}
	}

	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}