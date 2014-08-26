#include "App.h"
#include "Display.h"
#include "Images.h"

// 디스플레이 인스턴스를 초기화해줍니다.
Display *Display::m_pDisplay = NULL;


// 그리기를 시작합니다.
void Display::BeginDraw(Color &color)
{
	Graphics G(g_hWnd);

	RECT Client_Rect;
	GetClientRect(g_hWnd, &Client_Rect);

	pBackBuffer_Bitmap = new Bitmap(Client_Rect.right, Client_Rect.bottom, &G);
	pBackBuffer = new Graphics(pBackBuffer_Bitmap);
	pBackBuffer->FillRectangle(&SolidBrush(color),0,0,Client_Rect.right,Client_Rect.bottom);
}

// 그리기를 종료합니다.
void Display::EndDraw(void)
{
	Graphics G(g_hWnd);

	if(pForPaintBuffer)
		delete pForPaintBuffer;
	pForPaintBuffer = new CachedBitmap(pBackBuffer_Bitmap, &G);
	delete pBackBuffer_Bitmap;
	delete pBackBuffer;

	G.DrawCachedBitmap(pForPaintBuffer, 0, 0);

	InvalidateRect(g_hWnd, NULL, false);
}