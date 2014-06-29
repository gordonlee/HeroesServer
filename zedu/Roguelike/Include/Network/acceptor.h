#pragma once

#include <list>
#include "socket_object.h"
#include "Network/addr.h"
#include "Network/socket.h"
#include "Core/types.h"
#include "Thread/lock.h"

namespace zedu {

	class IAcceptor : public ISocketObject
	{
	protected:
		int m_backlog;
		bool m_bStarted;

	public:
		IAcceptor( int backlog=SOMAXCONN )
		{
			m_socket.CreateStreamSocket( true );
			m_backlog = backlog;
			m_bStarted = false;
		}

		virtual ~IAcceptor()
		{
			if( m_bStarted ) EndAccept();
		}

		virtual bool StartAccept( const Addr& addr );
		virtual bool StartAccept( uint32 port )
		{
			return StartAccept( Addr( "0.0.0.0", port) );
		}
		virtual bool EndAccept();
	};

	class IOCPAcceptor : public ITypeInfo, public IAcceptor
	{
	public:
		static int TypeID;
		int GetTypeID() const { return TypeID; }

	public:
		IOCPAcceptor( int backlog=SOMAXCONN );
		virtual ~IOCPAcceptor();

	protected:
		friend class IOCP;
		bool PendAcceptRequest();
		void DeleteFromPendingList( SOCKET hSock );

		struct OverlappedIO* m_pAcceptOverlapped;

		CriticalSection m_lock;
		std::list<Socket*> m_pendingSockets;
	};
}