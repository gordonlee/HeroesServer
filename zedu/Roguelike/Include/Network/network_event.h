#pragma once
#include "Core/types.h"
#include "connection.h"
#include "socket.h"

namespace zedu {

	class IAcceptor;
	class IConnection;

	struct INetworkEventReceiver
	{
		virtual IConnection* CreateConnection(const Socket& sock ) = 0;
		virtual bool OnAccept( int id, IAcceptor* pAcceptor, IConnection* pConnection ) = 0;
		virtual void OnAccepted( int threadId, IConnection* pConnection ) {}
		virtual void OnRead( int threadId, IConnection* pConnection, uint32 size ) {}
		virtual void OnWrite( int threadID, IConnection* pConnection, uint32 size ) {}
		virtual void OnDisconnect( int threadID, IConnection* pConnection ) {}
	};
}