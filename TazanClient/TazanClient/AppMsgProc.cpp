#include "App.h"
#include "AppMsgProc.h"
#include "Display.h"

// 메인 윈도우의 메시지 프로시저입니다.
LRESULT CALLBACK AppMsgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_CREATE:
		{
			return 0;
		}

	case WM_ERASEBKGND:
		// WM_PAINT 메시지가 왔을 때 배경을 지우지 않도록합니다.
		// 깜박임을 없애기 위해서입니다.
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