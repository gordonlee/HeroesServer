#include "App.h"
#include "AppMsgProc.h"

#include "Display.h"

#include "UserInfo.h"

#include "Packet.h"

HWND hListbox;
HFONT hFont;
char ReadBuffer[65536];
int CurrentReadLength = 0;
TCHAR buf[256];

const TCHAR* GetCurrentTimeToString()
{
	time_t t = time(0);   // get time now
	tm now;
	localtime_s(&now, &t);
	wsprintf(buf, TEXT("  %04d.%02d.%02d %02d:%02d:%02d"), now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);

	return buf;
}

// 메인 윈도우의 메시지 프로시저입니다.
LRESULT CALLBACK AppMsgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_CREATE:
	{
		hListbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
			600, 0, 200, 300, hWnd, NULL, g_hInst, NULL);
		hFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
		SendMessage(hListbox, WM_SETFONT, (WPARAM)hFont, 0);

		return 0;
	}

	case WM_SOCKET:
	{
		if (WSAGETSELECTERROR(lParam))
		{
			MessageBox(hWnd,
				TEXT("Connection to server failed"),
				TEXT("Error"),
				MB_OK | MB_ICONERROR);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}

		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
		{
			int inDataLength = recv(g_Socket,
				(char*)(ReadBuffer+CurrentReadLength),
				65536 - CurrentReadLength,
				0);

			CurrentReadLength += inDataLength;

			PacketHeader* packetHeader = (PacketHeader*)ReadBuffer;
			if (packetHeader->DataSize + 4 <= CurrentReadLength)
			{
				// 여기에서 패킷을 프로세싱한다.
				if (packetHeader->Checksum == 0x55)
				{
					switch (packetHeader->Flag)
					{
					case LoginResultFlag:
					{
						LoginResultMessage* msg = (LoginResultMessage*)packetHeader;
						g_MyUserInfo = msg->MyUserInfo;
						UserInfo* user = new UserInfo[msg->UserCount];
						memcpy(user, &msg->OtherUsersInfo, sizeof(UserInfo)* msg->UserCount);
						for (int i = 0; i < msg->UserCount; ++i)
						{
							g_UserInfoList.push_back(&user[i]);
						}

						SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)TEXT("<LoginResultMessage>"));
						SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)GetCurrentTimeToString());
						wsprintf(buf, TEXT("  MY ID[%d], POS[%d,%d] : %s"), g_MyUserInfo.UserID, g_MyUserInfo.X, g_MyUserInfo.Y, GetDirectionToString(g_MyUserInfo.UserDirection));
						SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)buf);
						for (int i = 0; i < msg->UserCount; ++i)
						{
							g_UserInfoList.push_back(&user[i]);
							wsprintf(buf, TEXT("  Other ID[%d], POS[%d,%d] : %s"), user[i].UserID, user[i].X, user[i].Y, GetDirectionToString(user[i].UserDirection));
							SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)buf);
						}

						break;
					}
					case JoinNewUserFlag:
					{
						JoinNewUserMessage* msg = (JoinNewUserMessage*)packetHeader;
						UserInfo* user = new UserInfo;
						*user = msg->NewUserInfo;
						g_UserInfoList.push_back(user);

						SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)TEXT("<JoinNewUserMessage>"));
						SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)GetCurrentTimeToString());
						wsprintf(buf, TEXT("  New ID[%d], POS[%d,%d] : %s"), user->UserID, user->X, user->Y, GetDirectionToString(user->UserDirection));
						SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)buf);

						break;
					}
					}
				}

				CurrentReadLength -= packetHeader->DataSize + sizeof(PacketHeader);
				memmove(ReadBuffer, ReadBuffer + packetHeader->DataSize + 4, CurrentReadLength);
			}

			break;
		}
		case FD_CLOSE:
		{
			MessageBox(hWnd,
				TEXT("Server closed connection"),
				TEXT("Connection closed!"),
				MB_OK | MB_ICONINFORMATION);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}
		}

		// 자동 스크롤을 위해
		int itemCount = SendMessage(hListbox, LB_GETCOUNT, 0, 0);
		SendMessage(hListbox, LB_SETCURSEL, itemCount - 1, 0);
		SendMessage(hListbox, LB_SETCURSEL, -1, 0);
		UpdateWindow(hListbox);

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

	return (DefWindowProc(hWnd,iMessage,wParam,lParam));
}