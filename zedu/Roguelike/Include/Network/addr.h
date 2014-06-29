#pragma once
#include "net_const.h"
#include <string.h>
#include <winsock2.h>

namespace zedu {

	class Addr
	{
	public:
		Addr( const char* addr=0, int port=0 )
		{
			SetAddr( addr );
			SetPort( port );
		}

		const char* GetAddr() const { return m_addr; }
		int GetPort() const { return m_port; }
		
		void SetAddr( const char* addr )
		{
			if( addr )
				strncpy( m_addr, addr, ADDRESS_MAX_LENGTH-1 );
			else
				m_addr[0] = 0;
		}

		void SetPort( int port )
		{
			m_port = port;
		}

		const Addr& operator =( const Addr& addr )
		{
			SetAddr( addr.GetAddr() );
			SetPort( addr.GetPort() );
			
			return *this;
		}

		bool operator ==( const Addr& addr ) const
		{
			if( strcmp( GetAddr(), addr.GetAddr()) )
				return false;

			if( GetPort() != addr.GetPort() )
				return false;

			return true;
		}

		template<typename T1, typename T2>
		static bool Convert( const T1& v1, T2& v2 )
		{
		}

		template<>
		static bool Convert<Addr, sockaddr_in>( const Addr& addr, sockaddr_in& addr_in )
		{
			unsigned long unaddr;

			unaddr = inet_addr( addr.GetAddr() );

			if( unaddr == INADDR_NONE )
			{
				struct hostent FAR * hostEnt;
				hostEnt = gethostbyname( addr.GetAddr() );

				if( !hostEnt ) return false;

				memcpy( &unaddr, hostEnt->h_addr_list[0], hostEnt->h_length );
			}
			memset( &addr_in, 0, sizeof(struct sockaddr_in) );
			addr_in.sin_family = AF_INET;
			addr_in.sin_addr.S_un.S_addr = unaddr;
			addr_in.sin_port = htons( addr.GetPort() );

			return true;
		}

	private:
		char m_addr[ADDRESS_MAX_LENGTH];
		int m_port;
	};
}
