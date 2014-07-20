#include "VNetwork.h"
#include "VClientSession.h"
#include "VWorkerThread.h"
#include "VLogger.h"

namespace VCore
{
	VNetwork::VNetwork(int port)
		: port_(port)
		, sessionManager_()
		, currentThreadID_(0)
		, sessionID_(0)
	{
	}

	bool VNetwork::Initialize()
	{
		Logger<VNetwork>::Info("Start Initialize.");

		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			Logger<VNetwork>::Fatal("WSA Initialize fail.");
			return false;
		}

		/// TCP소켓을 RIO모드로 초기화한다
		listenSocket_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_REGISTERED_IO);
		if (listenSocket_ == INVALID_SOCKET)
		{
			Logger<VNetwork>::Fatal("Listen soket Initialize fail.");
			return false;
		}

		// TODO : 옵션이 뭐뭐있는지 알아보자
		int opt = 1;
		setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));

		/// bind
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(port_);
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(listenSocket_, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
		{
			Logger<VNetwork>::Fatal("Listen soket bind fail.");
			return false;
		}

		if (RIOBase::GetInstance()->Initialize(listenSocket_) == SOCKET_ERROR)
		{
			Logger<VNetwork>::Fatal("RIO Function table initialize fail.");
			return false;
		}

		if (RIOBase::GetInstance()->MakeCompletionQueue(MAX_RIO_THREAD) == FALSE)
		{
			Logger<VNetwork>::Fatal("Make completion queue fail.");
			return false;
		}

		sessionManager_.MakeSession(MAX_SESSION);

		return true;
	}

	bool VNetwork::Run()
	{
		Logger<VNetwork>::Info("Execute woker thread.");
		for (int threadIndex = 0; threadIndex < MAX_RIO_THREAD; ++threadIndex)
		{
			workerThread[threadIndex] = std::thread(&VNetwork::Worker, this, threadIndex);
			workerThread[threadIndex].joinable();
		}

		if (SOCKET_ERROR == listen(listenSocket_, SOMAXCONN))
		{
			Logger<VNetwork>::Fatal("Listen fail.");
			return false;
		}
			

		Logger<VNetwork>::Info("Run Server.");

		while (true)
		{
			if (!Accept())
			{
				Logger<VNetwork>::Fatal("Accept fail.");
				continue;
			}
		}

		for (int threadIndex = 0; threadIndex < MAX_RIO_THREAD; ++threadIndex)
		{
			workerThread[threadIndex].join();
		}
	}

	bool VNetwork::Accept()
	{
		SOCKET acceptedSock = accept(listenSocket_, NULL, NULL);
		if (acceptedSock == INVALID_SOCKET)
		{
			Logger<VNetwork>::Fatal("Accept : Invalid socket.");
			return false;
		}

		ClientSession* client = sessionManager_.GetSession(sessionID_);
		client->Initialize(acceptedSock);
		client->requestQueue_ = RIOBase::GetInstance()->GetRequestQueue(acceptedSock);

		RioIoContext* mainContext = new RioIoContext(client, IO_RECV, true);
		DWORD flags = 0;

		if (!RIOBase::GetInstance()->RIOReceive(client->requestQueue_, client->GetRecieveBuffer(), 1, flags, mainContext))
		{
			//TODO :  Logger 포멧 적용하기 GetLastError()
			Logger<VNetwork>::Fatal("RIOReceive error.");
			delete mainContext;
			return false;
		}

		sessionID_++;

		return true;
	}
	
}