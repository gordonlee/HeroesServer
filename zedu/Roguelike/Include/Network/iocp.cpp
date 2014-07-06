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

		// ������ AcceptEx()�� ��ȿȭ �Ǿ����Ƿ� ���� ���� ���ش�.
		pAcceptor->PendAcceptRequest();

		if( bSuccess )
		{
			// Connection�� �����ؼ� receiver���� �־�� �Ѵ�.
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

			// EventReceiver ȣ��
			if( pTag->pReceiver )
				pTag->pReceiver->OnRead( threadNum, pConnection, size );

			// �������� Recv ���ٸ�..
			if( size > 0 && pConnection->IsConnected() )
			{
				// ������ WSARecv()�� ��ȿȭ �Ǿ����Ƿ� ���� ���� ���ش�
				if( !pConnection->PendRecvRequest() )
				{
					break;
				}
			}

			// �ٸ� �����忡�� disconnect �̺�Ʈ�� �߻��ߴٸ� ���� ������ ó��
			break;

		case OverlappedIO::IO_SEND:
			// �̺�Ʈ ���ù� ȣ��
			if( pTag->pReceiver )
				pTag->pReceiver->OnWrite( threadNum, pConnection, size );

			// ������ �����͸� SendQueue���� �����Ѵ�
			pConnection->OnSendCompletionEvent( size );

			// size�� 1���� ������ ���� ����������
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

		InterlockedIncrement( &pTag->pIOCP->m_currentThreadCount );
		InterlockedIncrement( &pTag->pIOCP->m_workingThreadCount );

		uint32 threadNum = 0;
		for( threadNum=0; 
			threadNum < pTag->threadIDs.size() && GetCurrentThreadId() != pTag->threadIDs[threadNum];
			threadNum++ ); // threadNum�� ��� ���� ����
		
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
				// WorkingThreadCount ����(������̹Ƿ� 1����)
				InterlockedDecrement( &pTag->pIOCP->m_workingThreadCount );

				rtn = GetQueuedCompletionStatus( pTag->hIOCP, 
												&dwNumberOfBytes,
												&dwCompletionKey,
												(OVERLAPPED**)&pOverlapped,
												INFINITE );

				// WorkingThreadCount ����(ó������̹Ƿ� 1����)
				InterlockedIncrement( &pTag->pIOCP->m_workingThreadCount );

				// ������ ���� �ñ׳��� �����ߴٸ� ����
				if( dwCompletionKey == IOCP::EVENT_STOP || pTag->bFinished )
				{
					break;
				}

				// pOverlapped->pObj�� 0�̶�� Ŀ�ؼ��� �̹� �޸𸮿��� �����Ǿ���.
				if( pOverlapped->pObj == 0 )
				{
					delete pOverlapped;
					continue;
				}

				// Ŀ�ؼ� ������ �̺�Ʈ���..
				if( dwCompletionKey == IOCP::EVENT_CONNECTION_CLOSED )
				{
					// �̺�Ʈ ���ù��� disconnect �̺�Ʈ �˷�����
					pTag->pReceiver->OnDisconnect( threadNum, static_cast<IOCPConnection*>(pOverlapped->pObj) );
					continue;
				}

				// GetQueuedCompletionStatus() �������..
				if( !rtn )
				{
					int errorCode = ::WSAGetLastError();
					//pOverlapped->pObj->SetLastError( errorCode );

					if( errorCode == WSAECONNREFUSED )
					{
						continue;
					}

					// ��Ŀ��Ʈ
					if(	errorCode == WSAENETDOWN ||
						errorCode == WSAENETUNREACH ||
						errorCode == WSAENETRESET ||
						errorCode == WSAECONNABORTED ||
						errorCode == WSAECONNRESET ||
						errorCode == WSAETIMEDOUT ||
						errorCode == WSAEHOSTDOWN ||
						errorCode == WSAEHOSTUNREACH ||
						errorCode == WSAEDISCON ||
						errorCode == WSA_OPERATION_ABORTED ||

						errorCode == ERROR_SEM_TIMEOUT ||
						errorCode == ERROR_NETNAME_DELETED ||
						errorCode == ERROR_CONNECTION_ABORTED ||
						errorCode == ERROR_OPERATION_ABORTED ||

						errorCode == ERROR_HOST_UNREACHABLE ) // ���� �߻��Ѵٳ�?
					{
						if( pOverlapped->typeIO == OverlappedIO::IO_ACCEPT )
						{
							OnAcceptEvent( pTag, threadNum, pOverlapped, false );
						}
						else if( pOverlapped->typeIO == OverlappedIO::IO_RECV || pOverlapped->typeIO == OverlappedIO::IO_SEND )
						{
							OnConnectionEvent( pTag, threadNum, pOverlapped, -1 );
						}

						// ��Ÿ������
						continue;
					}
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

		// ThreadCount ����
		InterlockedDecrement( &pTag->pIOCP->m_currentThreadCount );
		InterlockedDecrement( &pTag->pIOCP->m_workingThreadCount );

		return 0;
	}
	
	IOCP::IOCP( INetworkEventReceiver* pReceiver )
	{
		m_pTag = new IOCPTag( pReceiver );
		m_pTag->pIOCP = this;

		m_threadCount = 0;
		m_currentThreadCount = 0;
		m_workingThreadCount = 0;
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
			m_currentThreadCount = 0;
			m_workingThreadCount = 0;

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

		// ��ü ����
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

		// �⺻ IOCP �ڵ鿡 ���
		HANDLE hIOCP = CreateIoCompletionPort( hFileHandle, m_pTag->hIOCP, (ULONG_PTR)hFileHandle, 0 );

		// IO �̸� ��û
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

		m_threadCount = threadNum;
		return true;
	}
	
	bool IOCP::EndThreadPool()
	{
		if( !m_pTag->hIOCP )
			return false;
		
		int errorCount = 0;
		std::vector<uintptr_t>::iterator it;
		std::vector<uintptr_t>& threadHandles = m_pTag->threadHandles;

		// ��� �����忡�� ����ñ׳�(IOCP::EVENT_STOP)�� �����Ѵ�
		for(it = threadHandles.begin(); it != threadHandles.end(); it++)
		{
			if( !PostQueuedCompletionStatus(m_pTag->hIOCP, 0, IOCP::EVENT_STOP, NULL ) )
			{
				++errorCount ;
			}
		}

		if( errorCount > 0 )
			return false;

		// ��� �����尡 ��ĥ������ ����Ѵ�
		while( m_currentThreadCount > 0 )
		{
			Sleep( 100 );
		}

		
		for(it = threadHandles.begin(); it != threadHandles.end(); it++)
		{
			CloseHandle( (HANDLE)*it );
		}

		threadHandles.clear();
		return true;
	}

	OverlappedAllocator::OverlappedAllocator()
	{
	}

	OverlappedAllocator::~OverlappedAllocator()
	{
	}
}