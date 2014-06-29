#pragma once

#include "Core/types.h"
#include "network_event.h"
#include "socket_object.h"

namespace zedu {

	struct IOCPTag;
	struct OverlappedIO;

	class IOCP
	{
	public:
		enum { BUFFER_SIZE = 16384 };
		enum { EVENT_STOP=0xffffffff, EVENT_CONNECTION_CLOSED= 0xfffffffe };

	private:
		IOCPTag* m_pTag;
		uint32 m_threadCount;
	
	public:
		IOCP( INetworkEventReceiver* pReceiver );
		~IOCP();

		bool Create();
		bool Shutdown();

		bool AddObject( ITypeInfo* pObj );
		bool DeleteObject( ITypeInfo* pObj );

		bool StartThreadPool( uint32 threadNum );
		bool EndThreadPool();

	private:
		static unsigned int __stdcall WorkerThread( void* pArg );
		static bool OnAcceptEvent( IOCPTag* pTag, int threadNum, OverlappedIO* pOverlapped, bool bSuccess);
		static bool OnConnectionEvent( IOCPTag* pTag, int threadNum, OverlappedIO* pOverlapped, int size );
	};
}