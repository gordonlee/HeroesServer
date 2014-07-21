#pragma once

#include <WinSock2.h>
#include <MSWSock.h>

#include "RIOBase.h"
#include "VBufferController.h"

namespace VCore
{
	class ClientSession : public VBufferController
	{
	public:
		ClientSession()
		{
		}

		bool Initialize(int currentThreadID, SOCKET socket)
		{
			socket_ = socket;
			SOCKADDR_IN clientaddr;
			int addrlen = sizeof(clientaddr);
			getpeername(socket, (SOCKADDR*)&clientaddr, &addrlen);

			addr_ = clientaddr;

			// Non-Blocking I/O 세팅
			u_long arg = 1;
			ioctlsocket(socket_, FIONBIO, &arg);

			// Nagle OFF
			int opt = 1;
			setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));

			return RegisterRequestQueue(currentThreadID);

		}

		bool RegisterRequestQueue(int currentThreadID)
		{
			// RQ 생성
			// TODO : 이건 나중에 rio_base로 빼버리자
			requestQueue_ =
			m_RioFunctionTable.RIOCreateRequestQueue(
			socket_,
			MAX_RECV_RQ_SIZE_PER_SOCKET,
			1,
			MAX_SEND_RQ_SIZE_PER_SOCKET,
			1,
			m_RioCompletionQueue[currentThreadID % MAX_RIO_THREAD],
			m_RioCompletionQueue[currentThreadID % MAX_RIO_THREAD],
			NULL);

			if (requestQueue_ == RIO_INVALID_RQ)
			{
				printf_s("[DEBUG] RIOCreateRequestQueue Error: %d\n", GetLastError());
				return false;
			}

			return true;
		}
		
		SOCKET			socket_;
		RIO_BUFFERID	bufferID_;

		RIO_RQ			requestQueue_;
		SOCKADDR_IN		addr_;
	};

	class RioIoContext
	{
	public:
		RioIoContext(ClientSession* client, IOType ioType)
			: RioIoContext(client, ioType, false)
		{
		}

		RioIoContext(ClientSession* client, IOType ioType, bool isMainContext)
			: clientSession_(client)
			, ioType_(ioType)
			, isMainContext_(isMainContext)
			, bufferID_(RIO_INVALID_BUFFERID)
		{
		}

		bool isMainContext_;
		ClientSession*	clientSession_;
		VCore::IOType	ioType_;
		RIO_BUFFERID	bufferID_;
	};
}

