#include "VNetwork.h"
#include "VClientSession.h"
#include "VWorkerThread.h"

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
		printf_s("소켓 초기화\n");
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("WInSock 초기화 실패\n");
			return false;
		}

		/// TCP소켓을 RIO모드로 초기화한다
		listenSocket_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_REGISTERED_IO);
		if (listenSocket_ == INVALID_SOCKET)
		{
			printf("Listen 소켓 초기화 실패\n");
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
			printf("Bind Listen 소켓 실패\n");
			return false;
		}

		// RIO FunctionTbale 초기화
		GUID functionTableId = WSAID_MULTIPLE_RIO;
		DWORD dwBytes = 0;
		if (WSAIoctl(listenSocket_, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &functionTableId, sizeof(GUID), (void**)&m_RioFunctionTable, sizeof(m_RioFunctionTable), &dwBytes, NULL, NULL))
		{
			printf("RIO FunctionTable 초기화 실패\n");
			return false;
		}

		return true;
	}

	bool VNetwork::Run()
	{
		printf_s("Begin thread\n");
		for (int threadIndex = 0; threadIndex < MAX_RIO_THREAD; ++threadIndex)
		{
			DWORD dwThreadId;

			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, IoWorkerThread, (LPVOID)(threadIndex), 0, (unsigned int*)&dwThreadId);
			if (hThread == NULL)
				return false;
		}

		printf_s("Start Listen Server\n");
		if (SOCKET_ERROR == listen(listenSocket_, SOMAXCONN))
			return false;

		while (true)
		{
			if (!Accept())
			{
				printf_s("AcceptUser Fail.\n");
				continue;
			}
		}
	}

	bool VNetwork::Accept()
	{
		SOCKET acceptedSock = accept(listenSocket_, NULL, NULL);
		if (acceptedSock == INVALID_SOCKET)
		{
			printf_s("[DEBUG] accept: invalid socket\n");
			return false;
		}

		printf_s("accept\n");

		ClientSession* client = sessionManager_.MakeSession(sessionID_ ,acceptedSock);
		client->SetRequestQueue(currentThreadID_);

		RioIoContext* mainContext = new RioIoContext(client, IO_RECV, true);
		DWORD flags = 0;

		/// start async recv
		if (!m_RioFunctionTable.RIOReceive(client->requestQueue_, client->GetRecieveBuffer(), 1, flags, mainContext))
		{
			printf_s("[DEBUG] RIOReceive error: %d\n", GetLastError());
			delete mainContext;
			return false;
		}
		currentThreadID_ = (currentThreadID_ + 1) % MAX_RIO_THREAD;

		return true;
	}
	
}