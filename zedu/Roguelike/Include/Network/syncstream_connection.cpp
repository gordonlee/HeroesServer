#include "Core/logger.h"
#include "Core/types.h"
#include "Game/game_env.h"
#include "syncstream_connection.h"
#include "queue.h"

namespace zedu {

	fd_set SyncStreamConnection::m_readSet;
	fd_set SyncStreamConnection::m_writeSet;
	fd_set SyncStreamConnection::m_exSet;

	SyncStreamConnection::SyncStreamConnection() : ISocketConnection(true)
	{
		Init();
	}

	SyncStreamConnection::SyncStreamConnection( const Socket& sock ) : ISocketConnection(sock)
	{
		Init();
	}
	
	SyncStreamConnection::~SyncStreamConnection()
	{
		delete m_pSendQueue;
	}

	void SyncStreamConnection::Init()
	{
		m_pSendQueue = IQueue::CreateQueue( GAME_QUEUE_SIZE );

		m_recvBytes = 0;
		m_sendBytes = 0;
	}

	bool SyncStreamConnection::Connect( const Addr& addr )
	{
		if( IsConnected() )
			return false;

		if( !m_socket.IsValidSocket() )
		{
			if( !m_socket.CreateStreamSocket( true ) )
				return false;
		}

		struct sockaddr_in addr_in;
		if( !Addr::Convert( addr, addr_in ) )
		{
			return false;
		}

		int nRtn = ::connect( m_socket.GetSocketHandle(), 
								reinterpret_cast<const struct sockaddr*>(&addr_in),
								sizeof(struct sockaddr_in) );

		if( !nRtn ) 
		{
			m_bConnected = true;

			m_myAddr.SetAddr( inet_ntoa(addr_in.sin_addr) );
			m_myAddr.SetPort( ntohs(addr_in.sin_port) );
			m_peerAddr.SetAddr( inet_ntoa(addr_in.sin_addr) );
			m_peerAddr.SetPort( ntohs(addr_in.sin_port) );
		}

		cprint( "[%s](%d) connection accepted \n", m_peerAddr.GetAddr(), GetSocketHandle() );

		return m_bConnected;
	}
	
	bool SyncStreamConnection::Close()
	{
		m_bConnected = false;
		bool bDestroy = m_socket.Destroy();
		if( bDestroy )
		{
			cprint( "[%s](%d) connection closed\n", m_peerAddr.GetAddr(), GetSocketHandle() );
		}
		
		return bDestroy;
	}

	int SyncStreamConnection::Read( void* pBuf, size_t len )
	{
		int ret = ::recv( m_socket, (byte*)pBuf, len, 0 );
		if( ret <= 0 )
		{
			m_bConnected = false;
		}

		m_recvBytes += ret;

		cprint( "[%s](%d) >> read %d bytes\n", m_peerAddr.GetAddr(), GetSocketHandle(), len );
		return ret;
	}
	
	const int STACKBUF_SIZE = 4096;
	int SyncStreamConnection::Write( const void* pBuf, size_t len )
	{
		THREAD_SYNC( m_writeCS );

		byte stackBuf[STACKBUF_SIZE];

		byte* pSendBuf = len < STACKBUF_SIZE ? stackBuf : new char[len];
		memcpy( pSendBuf, pBuf, len );

		int ret = ::send( m_socket, pSendBuf, len, 0 );

		if( pSendBuf != stackBuf )
			delete pSendBuf;

		m_sendBytes += len;
		cprint( "[%s](%d) << send %d bytes\n", m_peerAddr.GetAddr(), GetSocketHandle(), len );
		return ret;
	}
	
	int SyncStreamConnection::Peek( void* pBuf, size_t len )
	{
		//int readBytes = std::min<size_t>( len, m_pRecvQueue->Size() );
		//memcpy( pBuf, m_pRecvQueue->GetBuf(), readBytes );
		//return readBytes;
		return 0;
	}

	bool SyncStreamConnection::IsReadable()
	{
		return !!FD_ISSET( GetSocketHandle(), &m_readSet );
	}

	void SyncStreamConnection::Select(std::vector<SyncStreamConnection*>& sockets )
	{
		FD_ZERO( &m_readSet );
		FD_ZERO( &m_writeSet );
		FD_ZERO( &m_exSet );

		std::vector<SyncStreamConnection*>::iterator it = sockets.begin();
		for( ; it != sockets.end(); it++ )
		{
			if( (*it)->IsConnected() )
			{
				FD_SET( (*it)->GetSocketHandle(), &m_readSet );
				FD_SET( (*it)->GetSocketHandle(), &m_writeSet );
				FD_SET( (*it)->GetSocketHandle(), &m_exSet );
			}
		}

		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		if( ::select(0, &m_readSet, &m_writeSet, &m_exSet, &tv) == SOCKET_ERROR )
		{
			int errCode = ::WSAGetLastError();
		}
	}
}