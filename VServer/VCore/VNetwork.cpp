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

		if (SOCKET_ERROR == bind(listenSocket_, (SOCKADDR*)&serveraddr, sizeof(serveraddr)))
		{
			Logger<VNetwork>::Fatal("Listen soket bind fail.");
			return false;
		}

		// RIO FunctionTbale 초기화
		GUID functionTableId = WSAID_MULTIPLE_RIO;
		DWORD dwBytes = 0;
		if (WSAIoctl(listenSocket_, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &functionTableId, sizeof(GUID), (void**)&m_RioFunctionTable, sizeof(m_RioFunctionTable), &dwBytes, NULL, NULL))
		{
			Logger<VNetwork>::Fatal("RIO Function table initialize fail.");
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
			DWORD dwThreadId;

			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, IoWorkerThread, (LPVOID)(threadIndex), 0, (unsigned int*)&dwThreadId);
			if (hThread == NULL)
				return false;
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
		client->Initialize(currentThreadID_, acceptedSock);

		RioIoContext* mainContext = new RioIoContext(client, IO_RECV, true);
		DWORD flags = 0;

		/// start async recv
		if (!m_RioFunctionTable.RIOReceive(client->requestQueue_, client->GetRecieveBuffer(), 1, flags, mainContext))
		{
			//TODO :  Logger 포멧 적용하기 GetLastError()
			Logger<VNetwork>::Fatal("RIOReceive error.");
			delete mainContext;
			return false;
		}
		currentThreadID_ = (currentThreadID_ + 1) % MAX_RIO_THREAD;
		sessionID_++;

		return true;
	}
	
}