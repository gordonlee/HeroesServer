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
		volatile long m_currentThreadCount;
		volatile long m_workingThreadCount;
	
	public:
		IOCP( INetworkEventReceiver* pReceiver );
		~IOCP();

		bool Create();
		bool Shutdown();

		bool AddObject( ITypeInfo* pObj );
		bool DeleteObject( ITypeInfo* pObj );

		bool StartThreadPool( uint32 threadNum );
		bool EndThreadPool();

		long GetCurrentThreadCount() const { return m_currentThreadCount; }
		long GetWorkingThreadCount() const { return m_workingThreadCount; }

	private:
		static unsigned int __stdcall WorkerThread( void* pArg );
		static bool OnAcceptEvent( IOCPTag* pTag, int threadNum, OverlappedIO* pOverlapped, bool bSuccess);
		static bool OnConnectionEvent( IOCPTag* pTag, int threadNum, OverlappedIO* pOverlapped, int size );
	};
}