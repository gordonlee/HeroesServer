#include <winsock2.h>
#include <assert.h>
#include <algorithm>
#include "connection.h"
#include "iocp.h"
#include "iocp_struct.h"
#include "queue.h"
#include <mswsock.h>


namespace zedu {

	int IOCPConnection::TypeID = ITypeInfo::GenerateTypeID();

	volatile int g_connectionCount = 0;
	
	IOCPConnection::IOCPConnection( OverlappedAllocator* pAllocator, const Socket& sock )
		: ISocketConnection(sock)
	{
		Init( pAllocator );

		interlocked_increment( &g_connectionCount );
	}

	IOCPConnection::~IOCPConnection()
	{
		delete m_pSendQueue;
		delete m_pRecvQueue;

		delete [] m_pRecvOverlapped->pBuf;

		assert( m_pAllocator != NULL );

		{
			//THREAD_SYNC( m_pAllocator );
		}

		interlocked_decrement( &g_connectionCount );
	}

	void IOCPConnection::Init( OverlappedAllocator* pAllocator )
	{
		m_pRecvQueue = IQueue::CreateQueue( 8192 );
		m_pSendQueue = IQueue::CreateQueue( 8192 );

		m_pAllocator = pAllocator ? pAllocator : OverlappedAllocator::DefaultAllocator();
		{
			ScopedLock<ILock> SyncObj( *m_pAllocator );
			m_pRecvOverlapped = m_pAllocator->allocOverlapped();
			m_pSendOverlapped = m_pAllocator->allocOverlapped();
		}

		m_pRecvOverlapped->Init( reinterpret_cast<HANDLE>( m_socket.GetSocketHandle() ), this, new byte[IOCP::BUFFER_SIZE] );
		m_pRecvOverlapped->typeIO = OverlappedIO::IO_RECV;

		m_pSendOverlapped->Init( reinterpret_cast<HANDLE>( m_socket.GetSocketHandle() ), this );
		m_pSendOverlapped->typeIO = OverlappedIO::IO_SEND;
		
		m_bSendPending = false;
		m_sendingBytes = 0;

		m_pendingRecvQueryCount = 0;
		m_pendingSendQueryCount = 0;

		m_hIOCP = NULL;
		m_bAsyncCloseSignal = false;
	}

	bool IOCPConnection::Close()
	{
		if( IsConnected() ) 
			OnDisconnect( -999 );

		m_bConnected = false;

		//FIN날리기
		if( m_bAsyncCloseSignal ) ::shutdown( m_socket.GetSocketHandle(), SD_BOTH );

		m_socket.Destroy();

		return true;
	}

	void IOCPConnection::AsyncClose()
	{
		bool bClose = false;
		{
			THREAD_SYNC( m_sendCS );
			m_bAsyncCloseSignal = true;
			bClose = (m_pendingSendQueryCount== 0);
		}
		
		if( bClose ) Close();
	}


	void IOCPConnection::OnRecvCompletionEvent( int size )
	{
		InterlockedDecrement( &m_pendingRecvQueryCount );
		
		if( size < 1 )
		{
			m_recvCS.Unlock();
			return;
		}

		if( !m_pRecvQueue->Resize( m_pRecvQueue->Size() + size ) )
		{
			// TODO+ 에러!
		}
		m_recvCS.Unlock();

	}
	
	void IOCPConnection::OnSendCompletionEvent( int size )
	{
		THREAD_SYNC( m_sendCS );

		InterlockedDecrement( &m_pendingSendQueryCount );

		if( size < -1 )
			return;

		m_pSendQueue->Read( NULL, size );
		m_bSendPending = false;
		m_sendingBytes -= size;

		int restSize = m_pSendQueue->Size();

		bool bSendReq = false;
		// SendQueue에 있으나 WriteFile()되지 않은 부분이 있다면 처리
		if( m_pSendQueue->Size() - m_sendingBytes > 0 )
		{
			if( IsConnected() )
			{
				ProcWriteFile();
				bSendReq = true;
			}
		}

		if( m_bAsyncCloseSignal && !bSendReq )
			Close();
	}

	void IOCPConnection::OnConnect( void* pBuf )
	{
		struct sockaddr_in* pMyAddr = NULL;
		struct sockaddr_in* pPeerAddr = NULL;
		int myLen, peerLen;

		GetAcceptExSockaddrs( pBuf, 0, sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16,
							reinterpret_cast<sockaddr**>(&pMyAddr), &myLen, 
							reinterpret_cast<sockaddr**>(&pPeerAddr), &peerLen );

		m_myAddr.SetAddr( inet_ntoa(pMyAddr->sin_addr) );
		m_myAddr.SetPort( ntohs(pMyAddr->sin_port) );
		m_peerAddr.SetAddr( inet_ntoa(pPeerAddr->sin_addr) );
		m_peerAddr.SetPort( ntohs(pPeerAddr->sin_port) );
		
		m_bConnected = true;
	}
	
	void IOCPConnection::OnDisconnect( int flag )
	{
		bool bNotifyClosed = false;

		{
			THREAD_SYNC( m_closeCS );
		
			// 쿼리 갯수가 0이라면?
			if( GetPendingQueryCount() == 0 )
			{
				if( IsConnected() )
					m_bConnected = false;
			}
		}

		PostQueuedCompletionStatus( m_hIOCP, 0, IOCP::EVENT_CONNECTION_CLOSED, m_pRecvOverlapped );
	}

	bool IOCPConnection::PendRecvRequest()
	{
		if( !IsConnected() )
			return false;

		m_recvCS.Lock();

		bool bRtn = true;

		if( m_pRecvQueue->Size() + m_pRecvQueue->FreeSize() > m_pRecvQueue->GetReservedSize() )
		{
			// TODO+ 에러! 큐 Overflow!
		}

		if( m_pRecvQueue->FreeSize() == 0 )
		{
			m_pRecvQueue->Reserve( m_pRecvQueue->GetReservedSize() );
		}

		m_recvWSABUF.buf = const_cast<char*>( m_pRecvQueue->GetBuf() + m_pRecvQueue->Size() );
		m_recvWSABUF.len = m_pRecvQueue->FreeSize();

		m_dwRecvFlag = 0;
		int nRtn = WSARecv( GetSocketHandle(), &m_recvWSABUF, 1, &m_pRecvOverlapped->dwSize, &m_dwRecvFlag, m_pRecvOverlapped, NULL );

		int nErr = 0;
		if( nRtn == SOCKET_ERROR )
		{
			nErr = WSAGetLastError();
			if( nErr == WSA_IO_PENDING )
			{
			}
			else
			{
				bRtn = false;
				InterlockedDecrement( &m_pendingRecvQueryCount );

				//m_lastError = nErr;
				OnDisconnect( -2 );
			}
		}

		return bRtn;
	}

	bool IOCPConnection::ProcWriteFile()
	{
		if( !IsConnected() )
			return false;

		int len = m_pSendQueue->Size() - m_sendingBytes;
		int sendSize = (IOCP::BUFFER_SIZE > len ? len : IOCP::BUFFER_SIZE);

		OverlappedIO* pSendOverlapped = m_pSendOverlapped;
		
		InterlockedIncrement( &m_pendingSendQueryCount );

		m_dwSendFlag = 0;
		m_sendWSABUF.buf = const_cast<char*>( m_pSendQueue->GetBuf() + m_sendingBytes);
		m_sendWSABUF.len = sendSize;

		int nRtn = ::WSASend( GetSocketHandle(), &m_sendWSABUF, 1, &pSendOverlapped->dwSize, m_dwSendFlag, pSendOverlapped, NULL );

		if( nRtn == SOCKET_ERROR )
		{
			DWORD nErr = ::WSAGetLastError();
			if( nErr != WSA_IO_PENDING )
			{
			}
			else
			{
				InterlockedDecrement( &m_pendingSendQueryCount );

				//this->SetLastError( nErr );
				OnDisconnect( -3 );
				return false;
			}
		}

		m_bSendPending = true;

		m_sendingBytes += sendSize;

		return true;
	}

	int	 IOCPConnection::Read( void* pBuf, size_t len )
	{
		THREAD_SYNC( m_recvCS );

		uint32 readBytes = std::min<size_t>( len, m_pRecvQueue->Size() );
		m_pRecvQueue->Read( pBuf, readBytes );

		return readBytes;
	}

	int	 IOCPConnection::Write( const void* pBuf, size_t len )
	{
		if( !IsConnected() )
			return -1;

		if( len == 0 )
			return 0;

		THREAD_SYNC( m_sendCS );

		if( m_bAsyncCloseSignal ) return -1;

		uint32 prevSize = m_pSendQueue->Size();
		uint32 sendLen = len;
		if( m_pSendQueue->FreeSize() < len )
		{
			// TOOD+ 큐가 넘치면 무시한다(WaitQueue 처리 해야함)
		}
		else
		{
			if( m_pSendQueue->Write( pBuf, len ) != len )
			{
				OnDisconnect( -1 );
				return -1;
			}
		}

		if( !m_bSendPending )
		{
			ProcWriteFile();
		}

		return len;
	}

	void IOCPConnection::PeekSendBuf(void* pBuf, uint32 size)
	{
		m_pSendQueue->Peek(pBuf, size);
	}

	long IOCPConnection::GetPendingQueryCount()
	{
		return m_pendingRecvQueryCount + m_pendingSendQueryCount;
	}
}