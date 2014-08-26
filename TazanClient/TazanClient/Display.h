#pragma once

class Display
{
private:
	Display(void) : pForPaintBuffer(NULL), pBackBuffer_Bitmap(NULL), pBackBuffer(NULL) { }
	~Display(void) { }

	// 디스플레이 인스턴스입니다.
	static Display *m_pDisplay;

	// 더블버퍼링을 할때 필요합니다.
	CachedBitmap *pForPaintBuffer;
	Bitmap *pBackBuffer_Bitmap;

public:
	// 여기에 그립니다. BeginDraw() 하고 그려야합니다.
	Graphics *pBackBuffer;

	// 디스플레이 인스턴스를 얻어옵니다.
	static Display *GetInstance(void)
	{
		if(!m_pDisplay)
			m_pDisplay = new Display;

		return m_pDisplay;
	}

	// 디스플레이 인스턴스를 반환합니다.
	static void Release(void)
	{
		if(m_pDisplay)
		{
			delete m_pDisplay;
			m_pDisplay = NULL;
		}
	}

public:
	// 그리기를 시작합니다.
	void BeginDraw(Color &color=Color(0,0,0));
	// 그리기를 종료합니다.
	void EndDraw(void);
};