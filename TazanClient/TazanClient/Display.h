#pragma once

class Display
{
private:
	Display(void) : pForPaintBuffer(NULL), pBackBuffer_Bitmap(NULL), pBackBuffer(NULL) { }
	~Display(void) { }

	// ���÷��� �ν��Ͻ��Դϴ�.
	static Display *m_pDisplay;

	// ������۸��� �Ҷ� �ʿ��մϴ�.
	CachedBitmap *pForPaintBuffer;
	Bitmap *pBackBuffer_Bitmap;

public:
	// ���⿡ �׸��ϴ�. BeginDraw() �ϰ� �׷����մϴ�.
	Graphics *pBackBuffer;

	// ���÷��� �ν��Ͻ��� ���ɴϴ�.
	static Display *GetInstance(void)
	{
		if(!m_pDisplay)
			m_pDisplay = new Display;

		return m_pDisplay;
	}

	// ���÷��� �ν��Ͻ��� ��ȯ�մϴ�.
	static void Release(void)
	{
		if(m_pDisplay)
		{
			delete m_pDisplay;
			m_pDisplay = NULL;
		}
	}

public:
	// �׸��⸦ �����մϴ�.
	void BeginDraw(Color &color=Color(0,0,0));
	// �׸��⸦ �����մϴ�.
	void EndDraw(void);
};