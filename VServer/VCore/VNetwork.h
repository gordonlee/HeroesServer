#pragma once

#include <string>
#include <WinSock2.h>
#include <MSWSock.h>
#include <process.h>

#include "RIOBase.h"


namespace VCore
{
	class VNetwork
	{

	public:
		__declspec(dllexport) VNetwork(int port);
		__declspec(dllexport) ErrorMessage Initialize();
		__declspec(dllexport) ErrorMessage SetMaximumUser();
		__declspec(dllexport) ErrorMessage Run();
		__declspec(dllexport) ErrorMessage Pause();
		__declspec(dllexport) ErrorMessage Stop();
		__declspec(dllexport) ErrorMessage Accept();
		__declspec(dllexport) ClientSession* ConnetClientSession(SOCKET socket);

		RIO_CQ GetCompletionQueue(int index)
		{
			return m_RioCompletionQueue[index];
		}

		void SetCompletionQueue(int index, RIO_CQ cq)
		{
			m_RioCompletionQueue[index] = cq;
		}

	private:
		int m_CurrentThreadID;

		ListenSocketState m_CurrentState;

		int		m_Port;
		SOCKET	m_ListenSocket;
		
	};


}
