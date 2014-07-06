#pragma once

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include "Util/gdi_helper.h"
#include "Core/types.h"
#include <windows.h>
#include <windowsx.h>

namespace zedu {

	class Console
	{
		int m_posX, m_posY;
		int m_consoleWidth, m_consoleHeight;
		char** m_pLines;
		bool m_isFull;
		int m_count;
		int m_screenAdjust;
		int m_screenHeight;
		int m_fontHeight, m_fontGap;

	public:
		Console(int width=120, int height=1024, int fontHeight=14)
		{
			m_consoleWidth = width;
			m_consoleHeight = height;
			m_screenHeight = height;
			m_screenAdjust = 0;
			m_posX = 0;
			m_posY = 0;
			m_isFull = false;
			m_count = 0;
			m_fontHeight = fontHeight;
			m_fontGap = 2;

			m_pLines = new char*[m_consoleHeight];
			for(int i = 0; i < m_consoleHeight; ++i)
			{
				m_pLines[i] = new char[m_consoleWidth+1];
				memset( m_pLines[i], 0, m_consoleWidth+1 );
			}
		}

		~Console()
		{
			for(int i = 0; i < m_consoleHeight; ++i)
			{
				delete [] m_pLines[i];
			}
			delete [] m_pLines;
		}

		void Clear()
		{
			m_posX = 0;
			m_posY = 0;
			for( int i = 0; i < m_consoleHeight; i++ )
			{
				m_pLines[i][0] = 0;
			}
			m_isFull = false;
			m_count = 0;
		}

		void Printf(const char* str, ...)
		{
			char buf[2048];
			va_list va;
			va_start( va, str );
			_vsnprintf( buf, sizeof(buf), str, va );
			va_end( va );

			PrintStr( buf );
		}

		void PrintStr(const char* str)
		{
			char* p = const_cast<char*>(str);
			char* sp = p;
			int feed = 0;
			
			for(;/**p*/;p++)
			{
				// 개행문자
				if( *p == '\n' )
				{
					feed = PutString( sp, p-sp );
					sp = p;
					AddLine();

					if( feed )
					{
						sp -= feed;
						PrintStr( sp );
						return;
					}
					sp++;
					continue;
				}
				
				// 문자열 끝
				if( !*p )
				{
					if( p - sp )
					{
						feed = PutString( sp, p-sp );
						sp = p;
						if( feed )
						{
							sp -= feed;
							AddLine();
							PrintStr( sp );
						}
					}
					break;
				}
			}
		}

		void AddLine()
		{
			m_posX = 0;
			if( ++m_posY >= m_consoleHeight )
			{
				m_posY = 0;
				m_isFull = true;
			}

			if( m_count < m_consoleHeight ) ++m_count;
		}

		// 화면을 넘어가서 출력못한 문자 갯수를 반환값으로 알려준다
		int PutString(const char* str, int len)
		{
			int newlen = len;
			if( m_posX + len >= m_consoleWidth )
			{
				newlen = m_consoleWidth - m_posX;
			}
			
			strncpy( &m_pLines[m_posY][m_posX], str, newlen );
			m_posX += newlen;

			assert(m_posX <= m_consoleWidth);
			m_pLines[m_posY][m_posX] = '\0';

			return len - newlen;
		}
		
		const char* GetLine(int line)
		{
			if( line > m_consoleHeight || line < 0 )
			{
				return "~";
			}

			int n = line;
			if( m_isFull )
			{
				n = m_posY + line;
				n %= m_consoleHeight;
			}
			return m_pLines[n];
		}

		int GetHeight() const				{ return m_consoleHeight; }
		int GetWidth() const				{ return m_consoleWidth; }
		int GetCount() const				{ return m_count; }
		void SetScreenHeight(int height)	{ m_screenHeight = height; }
		int GetScreenHeight() const			{ return m_screenHeight; }
		const char* GetScreenLine(int i)	{ return GetLine( i+m_screenAdjust ); }
		int GetLineHeight() const			{ return m_fontHeight + m_fontGap; }
		
		void onScreenPos(int newAdjust )
		{
			if( newAdjust > 0 )
				newAdjust = 0;

			if( m_count - m_screenHeight + newAdjust < 0 )
				newAdjust = 0 - m_count + m_screenHeight /*- 1*/;

			m_screenAdjust = newAdjust;
		}

		void ScreenUp()						{ onScreenPos( m_screenAdjust-1 ); }
		void ScreenDown()					{ onScreenPos( m_screenAdjust+1 ); }
		void ScreenPageUp()					{ onScreenPos( m_screenAdjust - m_screenHeight/3 ); }
		void ScreenPageDown()				{ onScreenPos( m_screenAdjust + m_screenHeight/3 ); }
		void ScreenHome()					{ m_screenAdjust = (0 - m_count + m_screenHeight - 1); }
		void ScreenEnd()					{ m_screenAdjust = 0; }

		template<typename Canvas>
		void Draw( Canvas& canvas, HDC hdc, const RECT* rect )
		{
			canvas.Paint( this, hdc, rect );
		}

		static Console& DefaultConsole()
		{
			static Console instance(120, 1024);
			return instance;
		}
	};

	struct Canvas_W32
	{
		int m_canvasWidth, m_canvasHeight;
		HFONT oldFont;
		HFONT m_font;
		HDC m_canvasDC;
		HBITMAP m_canvasBitmap;
		HBRUSH m_canvasBrush;
		HGDIOBJ oldObject;
		
		Canvas_W32( HDC hdc, int w, int h, int fontHeight, COLORREF clrText=RGB(255,255,255), COLORREF clrBrush=RGB(0,0,0) )
		{
			m_canvasWidth = w;
			m_canvasHeight = h;

			m_canvasDC = CreateCompatibleDC( hdc );
			m_canvasBitmap = CreateCompatibleBitmap( hdc, 1600, 1200 );
			oldObject = SelectObject( m_canvasDC, m_canvasBitmap );

			m_font = zedu::CreateFont( "굴림체", fontHeight/2, fontHeight );
			oldFont = SelectFont( m_canvasDC, m_font );
			m_canvasBrush = CreateSolidBrush( clrBrush );
			SetTextColor( m_canvasDC, clrText );
			SetBkMode( m_canvasDC, TRANSPARENT );
		}
		~Canvas_W32()
		{
			SelectObject( m_canvasDC, oldObject );
			SelectFont( m_canvasDC, oldFont );
			DeleteDC( m_canvasDC );
			DeleteObject( m_canvasBitmap );
			DeleteObject( m_canvasBrush );
			DeleteObject( m_font );
		}

		void Clear()
		{
			RECT scRect;
			scRect.bottom = m_canvasWidth;
			scRect.top = 0;
			scRect.left = 0;
			scRect.right = m_canvasHeight;

			FillRect( m_canvasDC, &scRect, m_canvasBrush );
		}

		void Paint(Console* pConsole, HDC hdc, const RECT *pRect)
		{
			const RECT &rect = *pRect;

			Clear();

			int tall = rect.bottom;
			bool bAnchorBottom = false;

			// 화면 크기가 콘솔 내용을 표시하기 충분한가?
			if( pConsole->GetCount() * pConsole->GetLineHeight() > tall )
			{
				bAnchorBottom = true;
			}

			if( bAnchorBottom )
			{
				// 화면 하단정렬
				for(int i = pConsole->GetCount()-1, j=1; i >= 0; i--, j++)
				{
					const char* str = pConsole->GetScreenLine(i);
					int cy = tall - j*(pConsole->GetLineHeight());

					TextOut( m_canvasDC, 5, cy, str, strlen(str) );
				}
			}
			else
			{
				// 화면 상단정렬
				for(int i = 0; i < pConsole->GetCount()+1; i++)
				{
					const char* str = pConsole->GetScreenLine(i);

					TextOut( m_canvasDC, 5, i*(pConsole->GetLineHeight()), str, strlen(str) );
				}
			}

			BitBlt( hdc, 0, 0, m_canvasWidth, m_canvasHeight, m_canvasDC, 0, 0, SRCCOPY );
		}
	};

	//#define cprint	Console::DefaultConsole().Printf
};