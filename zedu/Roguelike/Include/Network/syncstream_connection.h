#pragma once
#include "connection.h"
#include "Thread/lock.h"
#include <vector>

namespace zedu {


	class SyncStreamConnection : public ISocketConnection
	{
	private:
		class IQueue* m_pSendQueue;

		CriticalSection m_writeCS;

		int m_recvBytes;
		int m_sendBytes;

	public:
		SyncStreamConnection();
		SyncStreamConnection( const Socket& sock );
		virtual ~SyncStreamConnection();

		virtual bool Connect( const Addr& addr );
		virtual bool Close();;

		virtual int Read( void* pBuf, size_t len );
		virtual int Write( const void* pBuf, size_t len );
		virtual int Peek( void* pBuf, size_t len );

		int GetTotalRecvBytes() const { return m_recvBytes; }
		int GetTotalSendBytes() const { return m_sendBytes; }

		bool IsReadable();

		static void Select(std::vector<SyncStreamConnection*>& sockets );
		static fd_set m_readSet;
		static fd_set m_writeSet;
		static fd_set m_exSet;

	private:
		void Init();
	};
}