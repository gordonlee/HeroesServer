#pragma once

#include <string>
#include <WinSock2.h>
#include <MSWSock.h>
#include <process.h>
#include <thread>

#include "RIOBase.h"
#include "VSessionManager.h"


namespace VCore
{
	class VNetwork
	{

	public:
		__declspec(dllexport) VNetwork(int port);
		__declspec(dllexport) bool Initialize();
		__declspec(dllexport) bool SetMaximumUser();
		__declspec(dllexport) bool Run();
		__declspec(dllexport) bool Pause();
		__declspec(dllexport) bool Stop();
		__declspec(dllexport) bool Accept();

		VOID Worker(int threadID);

	private:
		int currentThreadID_;

		int		port_;
		SOCKET	listenSocket_;
		
		VSessionManager sessionManager_;

		std::thread workerThread[MAX_RIO_THREAD];

		ULONG sessionID_;
	};


}
