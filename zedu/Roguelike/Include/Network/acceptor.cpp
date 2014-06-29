#include "acceptor.h"
#include <assert.h>
#include "iocp_struct.h"
#include <mswsock.h>

namespace zedu {
	int IOCPAcceptor::TypeID = ITypeInfo::GenerateTypeID();

	bool IAcceptor::StartAccept( const Addr& addr )
	{
		if( m_bStarted )
		{
			Error::SetLastError( "Acceptor already started" );
			return false;
		}

		struct sockaddr_in addr_in;
		if( !Addr::Convert( addr, addr_in ) )
		{
			m_socket.Destroy();
			Error::SetLastError( "Can't translate address" );
			return false;
		}

		char i = 1;
		if( setsockopt( m_socket, SOL_SOCKET| IPPROTO_TCP, SO_REUSEADDR, &i, sizeof(i) ) )
		{
			m_socket.Destroy();
			Error::SetLastError( "winsock setsockopt() error" );
			return false;
		}

		if( bind( m_socket, reinterpret_cast<struct sockaddr*>(&addr_in), sizeof(addr_in) ) )
		{
			int lastError = ::WSAGetLastError();
			Error::SetLastError( "winsock bind() error" );
			m_socket.Destroy();
			return false;
		}

		if( listen( m_socket, m_backlog ) )
		{
			Error::SetLastError( "winsock listen() error" );
			m_socket.Destroy();
			return false;
		}

		m_bStarted = true;
		return true;
	}

	bool IAcceptor::EndAccept()
	{
		if( !m_bStarted )
		{
			Error::SetLastError( "it's not started acceptor" );
			return false;
		}

		m_socket.Destroy();
		m_bStarted = false;
		return m_socket.CreateStreamSocket( true );
	}

	IOCPAcceptor::IOCPAcceptor( int backlog ) : IAcceptor(backlog)
	{
		int cnt = sizeof(sockaddr_in)*2+32;
		m_pAcceptOverlapped = new OverlappedIO( NULL, this, new char[cnt] );
	}

	IOCPAcceptor::~IOCPAcceptor()
	{
		for( std::list<Socket*>::iterator it = m_pendingSockets.begin(); 
			it != m_pendingSockets.end(); ++it )
		{
			(*it)->Destroy();
		}
		m_pendingSockets.clear();

		delete [] m_pAcceptOverlapped->pBuf;
		delete m_pAcceptOverlapped;
	}

	bool IOCPAcceptor::PendAcceptRequest()
	{
		if( !m_bStarted ) return false;

		Socket* pNewSocket = new Socket();

		if( !pNewSocket->CreateStreamSocket( true ) )
		{
			assert( 0 );
			return false;
		}
		
		{
			THREAD_SYNC( m_lock );
			m_pendingSockets.push_back( pNewSocket );
		}

		ZeroMemory( m_pAcceptOverlapped->pBuf, sizeof(sockaddr_in)*2+32 );

		// OVERLAPPED 구조체에 새로운 연결을 받을 소켓 세팅
		m_pAcceptOverlapped->hFileHandle = reinterpret_cast<HANDLE>( pNewSocket->GetSocketHandle() );
		m_pAcceptOverlapped->typeIO = OverlappedIO::IO_ACCEPT;

		DWORD dwByteReceived = 0;
		int nRtn = AcceptEx( m_socket, // 리스닝 소켓
							pNewSocket->GetSocketHandle(), // 클라이언트 소켓
							m_pAcceptOverlapped->pBuf, // 접속과 함께 데이터를 받기위한 버퍼
							0, // 수신할 데이터 크기
							sizeof(sockaddr_in)+16, // 클라이언트가 사용한 소켓주소 구조체크기에 16을 더한 값
							sizeof(sockaddr_in)+16, // 
							&dwByteReceived, // 연결되자마자 받은 데이터 크기
							m_pAcceptOverlapped // OVERLAPPED 구조체
							); 

		if( !nRtn )
		{
			int errorCode = ::WSAGetLastError();
			if( errorCode != WSA_IO_PENDING )
			{
				pNewSocket->Destroy();
				DeleteFromPendingList( pNewSocket->GetSocketHandle() );
			}
		}

		return true;
	}
	
	void IOCPAcceptor::DeleteFromPendingList( SOCKET hSock )
	{
		THREAD_SYNC( m_lock );

		for( std::list<Socket*>::iterator it = m_pendingSockets.begin() ; 
			it != m_pendingSockets.end(); ++it )
		{
			if( (*it)->GetSocketHandle() == hSock )
			{
				delete (*it);
				m_pendingSockets.erase( it );
				break;
			}
		}
	}
}