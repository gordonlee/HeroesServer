#include "App.h"
#include "AppMsgProc.h"

#include "Display.h"

#include "UserInfo.h"

#include "Packet.h"

char ReadBuffer[65536];
int CurrentReadLength = 0;

// 메인 윈도우의 메시지 프로시저입니다.
LRESULT CALLBACK AppMsgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_CREATE:
	{
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

						break;
					}
					case JoinNewUserFlag:
					{
						JoinNewUserMessage* msg = (JoinNewUserMessage*)packetHeader;
						UserInfo* user = new UserInfo;
						*user = msg->NewUserInfo;
						g_UserInfoList.push_back(user);

						break;
					}
					}
				}

				CurrentReadLength -= packetHeader->DataSize + 4;
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