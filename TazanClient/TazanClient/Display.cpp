#include "App.h"
#include "Display.h"
#include "Images.h"

// ���÷��� �ν��Ͻ��� �ʱ�ȭ���ݴϴ�.
Display *Display::m_pDisplay = NULL;


// �׸��⸦ �����մϴ�.
void Display::BeginDraw(Color &color)
{
	Graphics G(g_hWnd);

	RECT Client_Rect;
	GetClientRect(g_hWnd, &Client_Rect);

	pBackBuffer_Bitmap = new Bitmap(Client_Rect.right, Client_Rect.bottom, &G);
	pBackBuffer = new Graphics(pBackBuffer_Bitmap);
	pBackBuffer->FillRectangle(&SolidBrush(color),0,0,Client_Rect.right,Client_Rect.bottom);
}

// �׸��⸦ �����մϴ�.
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