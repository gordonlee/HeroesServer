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

		bool Initialize(SOCKET socket)
		{
			socket_ = socket;
			SOCKADDR_IN clientaddr;
			int addrlen = sizeof(clientaddr);
			getpeername(socket, (SOCKADDR*)&clientaddr, &addrlen);

			addr_ = clientaddr;

			// Non-Blocking I/O ¼¼ÆÃ
			u_long arg = 1;
			ioctlsocket(socket_, FIONBIO, &arg);

			// Nagle OFF
			int opt = 1;
			setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));

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

