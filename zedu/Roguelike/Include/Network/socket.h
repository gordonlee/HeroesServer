#pragma once

#include <winsock2.h>
#include "Core/exception.h"


namespace zedu {

	class Socket
	{
		SOCKET m_socket;
	
	public:
		Socket( SOCKET socket=INVALID_SOCKET) : m_socket(socket)
		{
		}

		bool IsValidSocket() const
		{
			return (m_socket != INVALID_SOCKET && m_socket != SOCKET_ERROR);
		}

		bool CreateStreamSocket( bool bOverlapped=false )
		{
			m_socket = ::WSASocket( PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, bOverlapped ? WSA_FLAG_OVERLAPPED : 0 );

			if( IsValidSocket() )
				return true;

			Error::SetLastError( "WSASocket() error" );
			return false;
		}

		bool CreateDatagramSocket( bool bOverlapped=false )
		{
			m_socket = ::WSASocket( PF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, bOverlapped ? WSA_FLAG_OVERLAPPED : 0 );

			if( IsValidSocket() )
				return true;

			Error::SetLastError( "WSASocket() error" );
			return false;
		}

		bool Destroy()
		{
			if( closesocket( m_socket ) == SOCKET_ERROR )
				return false;

			m_socket = INVALID_SOCKET;
			return true;
		}

		SOCKET GetSocketHandle() const				{ return m_socket; }
		operator SOCKET() const						{ return GetSocketHandle(); }
		void operator =( SOCKET sock )				{ m_socket = sock; }
		bool operator ==( const SOCKET &rh ) const	{ return m_socket == rh; }
	};
}