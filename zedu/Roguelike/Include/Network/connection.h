#pragma once

#include "addr.h"
#include "iocp_struct.h"
#include "socket.h"
#include "socket_object.h"
#include "Thread/lock.h"

namespace zedu {

	class IConnection
	{
	public:
		virtual ~IConnection() {}

		virtual bool Close() = 0;
		virtual bool IsConnected() = 0;

		virtual void GetMyAddress( Addr& addr ) = 0;
		virtual void GetPeerAddress( Addr& addr ) = 0;
		virtual const Addr& GetMyAddress() = 0;
		virtual const Addr& GetPeerAddress() = 0;
	};

	class ISocketConnection : public IConnection, public ISocketObject
	{
	protected:
		bool m_bConnected;
		Addr m_myAddr;
		Addr m_peerAddr;

	public:
		ISocketConnection( bool bOverlapped )
		{
			m_bConnected = false;
			m_socket.CreateStreamSocket( bOverlapped );
		}

		ISocketConnection( const Socket& sock )
		{
			m_bConnected = false;
			m_socket = sock;
		}

		bool IsConnected() { return m_bConnected; }

		virtual void GetMyAddress( Addr& addr )			{ addr = m_myAddr; }
		virtual void GetPeerAddress( Addr& addr )		{ addr = m_peerAddr; }
		virtual const Addr& GetMyAddress()				{ return m_myAddr; }
		virtual const Addr& GetPeerAddress()			{ return m_peerAddr; }

		virtual bool Connect( const Addr& addr ) = 0;
		virtual bool Close() = 0;

		virtual int Read( void* pBuf, size_t len ) = 0;
		virtual int Write( const void* pBuf, size_t len ) = 0;
		virtual int Peek( void* pBuf, size_t len ) = 0;
	};

	class IQueue;
	class IOCPConnection : public ITypeInfo, public ISocketConnection
	{
		friend class IOCP;

	private:
		struct OverlappedAllocator* m_pAllocator;
		
		volatile bool m_bSendPending;
		volatile int m_sendingBytes;
		
		volatile long m_pendingRecvQueryCount;
		volatile long m_pendingSendQueryCount;

		CriticalSection m_sendCS;
		CriticalSection m_recvCS;
		CriticalSection m_closeCS;

		IQueue* m_pRecvQueue;
		IQueue* m_pSendQueue;
		IQueue* m_pWaitQueue;

		OverlappedIO* m_pRecvOverlapped;
		OverlappedIO* m_pSendOverlapped;
		WSABUF m_recvWSABUF;
		WSABUF m_sendWSABUF;
		DWORD m_dwRecvFlag;
		DWORD m_dwSendFlag;

		HANDLE m_hIOCP;

		volatile bool m_bAsyncCloseSignal;
		volatile bool m_bDisconnectSignal;
		volatile bool m_bDisconnectEventPosted;

		int m_recvBytes;
		int m_sendBytes;
		int m_disconnectReason;
		long m_connectionNumber;

	public:
		IOCPConnection( OverlappedAllocator* pAllocator, const Socket& sock );
		virtual ~IOCPConnection();

		virtual bool Connect( const Addr& addr );

		virtual int Read( void* pBuf, size_t len );
		virtual int Write( const void* pBuf, size_t len );
		virtual int Peek( void* pBuf, size_t len );
		virtual int Size();
		byte* GetBuf();
		bool Reserve( int size );

		static int TypeID;
		virtual int GetTypeID() const { return TypeID; }

		virtual bool Close(); // 전송중인 버퍼를 무시하고 접속종료
		void AsyncClose(); // 전송중인 버퍼를 모두 전송후 접속죵료

		volatile long GetPendingQueryCount();
		volatile long GetPendingRecvCount();
		volatile long GetPendingSendCount();
		volatile bool CheckDisconnectSignal();

		int GetTotalRecvBytes() const { return m_recvBytes; }
		int GetTotalSendBytes() const { return m_sendBytes; }
		void SetSocket( const Socket& sock )
		{
			m_socket = sock;
		}
		int GetDisconnectReason() const { return m_disconnectReason; }
		void SetConnNumber( long num ) { m_connectionNumber = num; }
		long GetConnNumber() const { return m_connectionNumber; }

	//private:
		void Init( OverlappedAllocator* pAllocator );

	protected:
		void OnRecvCompletionEvent( int size );
		void OnSendCompletionEvent( int size );
		void OnConnect( void* pBuf );
		void OnDisconnect( int flag );
		bool PendRecvRequest();
		bool ProcWriteFile();
	};

}