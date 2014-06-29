#pragma once

#include <WinSock2.h>
#include <MSWSock.h>

#include "RIOBase.h"

namespace VCore
{
	class ClientSession
	{
	public:
		ClientSession(SOCKET socket, char* buffer)
		{
			m_socket = socket;
			m_buffer = buffer;
		}
		char* GetBuffer()
		{
			return m_buffer;
		}
		void SetBufferID(RIO_BUFFERID bufferID)
		{
			m_bufferID = bufferID;
		}

		RIO_BUFFERID GetBufferID()
		{
			return m_bufferID;
		}

		SOCKET GetClientSocket()
		{
			return m_socket;
		}

		void SetRequestQueue(RIO_RQ rq)
		{
			m_RequestQueue = rq;
		}

		RIO_RQ GetRequestQueue()
		{
			return m_RequestQueue;
		}

		void SetAddress(SOCKADDR_IN addr)
		{
			memcpy(&(m_Addr), &addr, sizeof(SOCKADDR_IN));
		}

		SOCKADDR_IN GetAddress()
		{
			return m_Addr;
		}

		bool IsConnected();
		bool DisConnect();
		
	private:
		SOCKET m_socket;
		char* m_buffer;
		RIO_BUFFERID m_bufferID;

		RIO_RQ m_RequestQueue;
		SOCKADDR_IN		m_Addr;
	};

	class RioIoContext : public RIO_BUF
	{
	public:
		RioIoContext(ClientSession* client, IOType ioType)
		{
			mClientSession = client;
			mIoType = ioType;
		}

		ClientSession* mClientSession;
		VCore::IOType	mIoType;
	};
}

