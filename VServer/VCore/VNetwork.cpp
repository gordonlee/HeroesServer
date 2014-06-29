#include "VNetwork.h"
#include "VClientSession.h"
#include "VWorkerThread.h"

namespace VCore
{
	VNetwork::VNetwork(int port)
	{
		m_Port = port;
		m_CurrentState = ListenSocketState::DISABLE;
		m_CurrentThreadID = 0;
	}

	ErrorMessage VNetwork::Initialize()
	{
		printf_s("소켓 초기화\n");
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("WInSock 초기화 실패\n");
			return ErrorMessage::WSAInitializeFail;
		}

		/// TCP소켓을 RIO모드로 초기화한다
		m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_REGISTERED_IO);
		if (m_ListenSocket == INVALID_SOCKET)
		{
			printf("Listen 소켓 초기화 실패\n");
			return ErrorMessage::MakeListenSocketFail;
		}

		// TODO : 옵션이 뭐뭐있는지 알아보자
		int opt = 1;
		setsockopt(m_ListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));

		/// bind
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(m_Port);
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (SOCKET_ERROR == bind(m_ListenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr)))
		{
			printf("Bind Listen 소켓 실패\n");
			return ErrorMessage::BindListenSocketFail;
		}

		// RIO FunctionTbale 초기화
		GUID functionTableId = WSAID_MULTIPLE_RIO;
		DWORD dwBytes = 0;
		if (WSAIoctl(m_ListenSocket, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &functionTableId, sizeof(GUID), (void**)&m_RioFunctionTable, sizeof(m_RioFunctionTable), &dwBytes, NULL, NULL))
		{
			printf("RIO FunctionTable 초기화 실패\n");
			return ErrorMessage::AllocFunctionTableFail;
		}

		m_CurrentState = ListenSocketState::READY;
	}

	ErrorMessage VNetwork::Run()
	{
		printf_s("Begin thread\n");
		for (int threadIndex = 0; threadIndex < MAX_RIO_THREAD; ++threadIndex)
		{
			DWORD dwThreadId;

			HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, IoWorkerThread, (LPVOID)(threadIndex), 0, (unsigned int*)&dwThreadId);
			if (hThread == NULL)
				return ErrorMessage::Unknowm;
		}

		printf_s("Start Listen Server\n");
		if (SOCKET_ERROR == listen(m_ListenSocket, SOMAXCONN))
			return ErrorMessage::ListenSocketFail;

		m_CurrentState = ListenSocketState::LISTENING;

		while (true)
		{
			if (Accept() != ErrorMessage::Success)
			{
				printf_s("AcceptUser Fail.\n");
				continue;
			}
		}
	}

	ErrorMessage VNetwork::Accept()
	{
		SOCKET acceptedSock = accept(m_ListenSocket, NULL, NULL);
		if (acceptedSock == INVALID_SOCKET)
		{
			printf_s("[DEBUG] accept: invalid socket\n");
			return ErrorMessage::Unknowm;
		}

		printf_s("accept\n");

		ClientSession* client = ConnetClientSession(acceptedSock);

		RioIoContext* recvContext = new RioIoContext(client, IO_RECV);

		recvContext->BufferId = client->GetBufferID();
		recvContext->Length = SESSION_BUFFER_SIZE;
		recvContext->Offset = 0;

		DWORD recvbytes = 0;
		DWORD flags = 0;

		/// start async recv
		if (!m_RioFunctionTable.RIOReceive(client->GetRequestQueue(), (PRIO_BUF)recvContext, 1, flags, recvContext))
		{
			printf_s("[DEBUG] RIOReceive error: %d\n", GetLastError());
			//ReleaseContext(recvContext);
			delete recvContext;
			return ErrorMessage::Unknowm;
		}

	}

	ClientSession* VNetwork::ConnetClientSession(SOCKET acceptedSock)
	{
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(acceptedSock, (SOCKADDR*)&clientaddr, &addrlen);

		/// new client session (should not be under any session locks)
		//ClientSession* client = mFreeSessionList[currentThreadID % MAX_RIO_THREAD + 1].back();
		//mFreeSessionList[currentThreadID % MAX_RIO_THREAD + 1].pop_back();
		//mOccupiedSessionList.push_back(client);
		ClientSession* client = new ClientSession(acceptedSock, new char[SESSION_BUFFER_SIZE]);
		memset(client->GetBuffer(), 0x00, SESSION_BUFFER_SIZE);
		// RIO 버퍼 등록 & 클라이언트에 생성된 버퍼ID 저장
		RIO_BUFFERID bufferID = m_RioFunctionTable.RIORegisterBuffer(client->GetBuffer(), SESSION_BUFFER_SIZE);

		if (bufferID == RIO_INVALID_BUFFERID)
		{
			printf_s("[DEBUG] RIORegisterBuffer Error: %d\n", GetLastError());
			return NULL;
		}

		client->SetBufferID(bufferID);

		// Non-Blocking I/O 세팅
		u_long arg = 1;
		ioctlsocket(client->GetClientSocket(), FIONBIO, &arg);

		// Nagle OFF
		int opt = 1;
		setsockopt(client->GetClientSocket(), IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));

		// RQ 생성
		//client->mRequestQueue =
		RIO_RQ requestQueue =
			m_RioFunctionTable.RIOCreateRequestQueue(
			client->GetClientSocket(),
			MAX_RECV_RQ_SIZE_PER_SOCKET,
			1,
			MAX_SEND_RQ_SIZE_PER_SOCKET,
			1,
			m_RioCompletionQueue[m_CurrentThreadID % MAX_RIO_THREAD],
			m_RioCompletionQueue[m_CurrentThreadID % MAX_RIO_THREAD],
			NULL);

		client->SetRequestQueue(requestQueue);

		if (client->GetRequestQueue() == RIO_INVALID_RQ)
		{
			printf_s("[DEBUG] RIOCreateRequestQueue Error: %d\n", GetLastError());
			return NULL;
		}

		client->SetAddress(clientaddr);

		m_CurrentThreadID++;

		return client;
	}
}