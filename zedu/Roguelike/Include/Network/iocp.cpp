#include <assert.h>
#include "acceptor.h"
#include "iocp.h"
#include "iocp_struct.h"
#include "network_event.h"
#include "Thread/thread.h"
#include <process.h>
#include <vector>
#include <Windows.h>


namespace zedu {

	OverlappedAllocator* OverlappedAllocator::DefaultAllocator()
	{
		struct UsingHeapOverlappedAllocator : OverlappedAllocator
		{
			virtual OverlappedIO* allocOverlapped()
			{
				OverlappedIO* p = new OverlappedIO();
				p->pAllocator = this;
				return p;
			}

			virtual void freeOverlapped( OverlappedIO* ptr )
			{
				delete ptr;
			}

			virtual void Lock( const char* fileName, int lineNum )
			{
			}

			virtual void Unlock()
			{
			}

			virtual bool IsLocked() 
			{
				return false;
			}
		};
		static UsingHeapOverlappedAllocator s_defaultOverlappedAllocator;
		return &s_defaultOverlappedAllocator;
	}

	struct IOCPTag
	{
		HANDLE hIOCP;
		std::vector<uint32> threadIDs;
		std::vector<uintptr_t> threadHandles;

		OverlappedAllocator* pOverlappedAllocator;
		INetworkEventReceiver* pReceiver;

		IOCP* pIOCP;
		bool bFinished;

		IOCPTag( INetworkEventReceiver* _pReceiver )
		{
			bFinished = false;
			hIOCP = NULL;
			pOverlappedAllocator = new OverlappedAllocator();
			pReceiver = _pReceiver;
		}

		~IOCPTag()
		{
			delete pOverlappedAllocator;
		}
	};

	bool IOCP::OnAcceptEvent( IOCPTag* pTag, int threadNum, OverlappedIO* pOverlapped, bool bSuccess=true )
	{
		IOCPAcceptor* pAcceptor = static_cast<IOCPAcceptor*>( pOverlapped->pObj );
		SOCKET hFileHandle = reinterpret_cast<SOCKET>( pOverlapped->hFileHandle );

		char buf[sizeof(sockaddr_in)*2 + 32];
		memcpy( buf, pOverlapped->pBuf, sizeof(sockaddr_in)*2 + 32 );

		pAcceptor->DeleteFromPendingList( hFileHandle );

		// 이전의 AcceptEx()가 무효화 되었으므로 재차 콜을 해준다.
		pAcceptor->PendAcceptRequest();

		if( bSuccess )
		{
			// Connection을 조립해서 receiver에게 주어야 한다.
			IOCPConnection* pConnection = static_cast<IOCPConnection*>( pTag->pReceiver->CreateConnection(Socket(hFileHandle)) );
			if( pConnection )
			{
				pConnection->OnConnect( buf );

				bool bAddObject = true;
				if( pTag->pReceiver )
				{
					bAddObject = pTag->pReceiver->OnAccept( threadNum, pAcceptor, pConnection );
				}

				if( bAddObject )
				{
					pTag->pIOCP->AddObject( pConnection );
					if( pTag->pReceiver )
					{
						pTag->pReceiver->OnAccepted( threadNum, pConnection );
					}
				}
			}	
		}
		else
		{
			closesocket( hFileHandle );
		}

		return true;
	}

	bool IOCP::OnConnectionEvent( IOCPTag* pTag, int threadNum, OverlappedIO* pOverlapped, int size )
	{
		IOCPConnection* pConnection = static_cast<IOCPConnection*>( pOverlapped->pObj );

		switch( pOverlapped->typeIO )
		{
		case OverlappedIO::IO_RECV:
			pConnection->OnRecvCompletionEvent( size );
			if( size < 1 )
			{
				pConnection->Close();
				break;
			}

			// EventReceiver 호출
			if( pTag->pReceiver )
				pTag->pReceiver->OnRead( threadNum, pConnection );

			// 정상적인 Recv 였다면..
			if( size > 0 && pConnection->IsConnected() )
			{
				// 이전의 WSARecv()가 무효화 되었으므로 재차 콜을 해준다
				if( !pConnection->PendRecvRequest() )
				{
					break;
				}
			}

			// 다른 스레드에서 disconnect 이벤트가 발생했다면 연결 끊어짐 처리
			break;

		case OverlappedIO::IO_SEND:
			// 이벤트 리시버 호출
			if( pTag->pReceiver )
				pTag->pReceiver->OnWrite( threadNum, pConnection, size );

			// 보내진 데이터를 SendQueue에서 제거한다
			pConnection->OnSendCompletionEvent( size );

			// size가 1보다 작으면 연결 끊어진것임
			if( size < 1 )
			{
				pConnection->OnDisconnect( 3 );
				break;
			}

			break;
		}
		return true;
	}

	uint32 __stdcall IOCP::WorkerThread( void* pArg )
	{
		IOCPTag* pTag = static_cast<IOCPTag*>( pArg );

		uint32 threadNum = 0;
		for( threadNum=0; 
			threadNum < pTag->threadIDs.size() && GetCurrentThreadId() != pTag->threadIDs[threadNum];
			threadNum++ ); // threadNum을 얻기 위한 루프
		
		char buf[1024];
		sprintf( buf, "IOCP %02d", threadNum );
		SetThreadName( -1, buf );

		int rtn;
		DWORD dwNumberOfBytes;
		ULONG_PTR dwCompletionKey;
		OverlappedIO* pOverlapped;

		while( true )
		{
			try
			{
				rtn = GetQueuedCompletionStatus( pTag->hIOCP, 
												&dwNumberOfBytes,
												&dwCompletionKey,
												(OVERLAPPED**)&pOverlapped,
												INFINITE );

				// 스레드 종료 시그널이 도착했다면 중지
				if( dwCompletionKey == IOCP::EVENT_STOP || pTag->bFinished )
				{
					break;
				}

				// pOverlapped->pObj가 0이라면 커넥션이 이미 메모리에서 해지되었다.
				if( pOverlapped->pObj == 0 )
				{
					delete pOverlapped;
					continue;
				}

				// 커넥션 끊어짐 이벤트라면..
				if( dwCompletionKey == IOCP::EVENT_CONNECTION_CLOSED )
				{
					// 이벤트 리시버에 disconnect 이벤트 알려주자
					pTag->pReceiver->OnDisconnect( threadNum, static_cast<IOCPConnection*>(pOverlapped->pObj) );
					continue;
				}

				// GetQueuedCompletionStatus() 에러라면..
				if( !rtn )
				{
					int errorCode = ::WSAGetLastError();
					//pOverlapped->pObj->SetLastError( errorCode );

					// TODO+ 에러처리..
					// continue;
					
					if( pOverlapped->typeIO == OverlappedIO::IO_ACCEPT )
					{
						// TODO+ onAcceptEvent()
					}
					else if( pOverlapped->typeIO == OverlappedIO::IO_RECV || pOverlapped->typeIO == OverlappedIO::IO_SEND )
					{
						// TODO+ onConnectionEvent
					}

					// 기타에러임
					continue;
				}

				if(pOverlapped->pObj->GetTypeID() == IOCPAcceptor::TypeID)
				{
					IOCP::OnAcceptEvent( pTag, threadNum, pOverlapped, true );
				}
				else if(pOverlapped->pObj->GetTypeID() == IOCPConnection::TypeID)
				{
					IOCP::OnConnectionEvent( pTag, threadNum, pOverlapped, dwNumberOfBytes );
				}
			}
			catch( std::exception& e )
			{
				assert( 0 );
			}
			catch( const char* str )
			{
				assert( 0 );
			}
		}

		return 0;
	}
	
	IOCP::IOCP( INetworkEventReceiver* pReceiver )
	{
		m_threadCount = 0;
		m_pTag = new IOCPTag( pReceiver );
		m_pTag->pIOCP = this;
	}
	
	IOCP::~IOCP()
	{
		delete m_pTag;
	}
	
	bool IOCP::Create()
	{
		m_pTag->hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
		if( m_pTag->hIOCP )
		{
			m_threadCount = 0;
			return true;
		}
		return false;
	}

	bool IOCP::Shutdown()
	{
		if( !m_pTag->hIOCP )
		{
			return false;
		}

		if( !CloseHandle( m_pTag->hIOCP ) )
		{
			return false;
		}

		m_pTag->hIOCP = NULL;
		
		return true;
	}

	bool IOCP::AddObject( ITypeInfo* pObj )
	{
		if( !m_pTag->hIOCP ) 
			return false;

		HANDLE hFileHandle = INVALID_HANDLE_VALUE;
		IOCPAcceptor* pAcceptor = NULL;
		IOCPConnection* pConnection = NULL;

		

		// 객체 얻어옴
		if(pObj->GetTypeID() == IOCPAcceptor::TypeID )
		{
			pAcceptor = static_cast<IOCPAcceptor*>( pObj );
			hFileHandle = reinterpret_cast<HANDLE>( pAcceptor->GetSocketHandle() );
		}
		else if(pObj->GetTypeID() == IOCPConnection::TypeID )
		{
			pConnection = static_cast<IOCPConnection*>( pObj );
			hFileHandle = reinterpret_cast<HANDLE>( pConnection->GetSocketHandle() );
			pConnection->m_hIOCP = m_pTag->hIOCP;
		}

		// 기본 IOCP 핸들에 등록
		HANDLE hIOCP = CreateIoCompletionPort( hFileHandle, m_pTag->hIOCP, (ULONG_PTR)hFileHandle, 0 );

		// IO 미리 요청
		if(pObj->GetTypeID() == IOCPAcceptor::TypeID )
		{
			pAcceptor->PendAcceptRequest();
		}
		else if(pObj->GetTypeID() == IOCPConnection::TypeID )
		{
			if( pConnection->IsConnected() )
				pConnection->PendRecvRequest();
		}
		return true;
	}
	
	bool IOCP::DeleteObject( ITypeInfo* pObj )
	{
		return true;
	}

	bool IOCP::StartThreadPool( uint32 threadNum )
	{
		if( !m_pTag->hIOCP )
		{
			Error::SetLastError( "Invalid IOCP handle" );
			return false;
		}

		m_pTag->threadHandles.reserve( threadNum * 2 );

		uint32 i;
		uint32 threadID;
		uint32 hThread;
		for( i = 0; i < threadNum; i++ )
		{
			hThread = (uint32)_beginthreadex( NULL, 0, WorkerThread, m_pTag, CREATE_SUSPENDED, &threadID );
			if( hThread )
			{
				m_pTag->threadHandles.push_back( hThread );
				m_pTag->threadIDs.push_back( threadID );
			}

		}

		for( i = 0; i < threadNum; i++ )
		{
			::ResumeThread( (HANDLE)m_pTag->threadHandles[i] );
		}
		return true;
	}
	
	bool IOCP::EndThreadPool()
	{
		return true;
	}

	OverlappedAllocator::OverlappedAllocator()
	{
	}

	OverlappedAllocator::~OverlappedAllocator()
	{
	}
}