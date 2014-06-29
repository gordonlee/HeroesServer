#pragma once
#include "socket.h"

namespace zedu {

	// Å¸ÀÔ µð½ºÆÐÄª °í¹Î
	// http://members.gamedev.net/sicrane/articles/dispatch.html
	
	struct ITypeInfo
	{
		static int GenerateTypeID()
		{
			static int typeID = 0;
			return typeID++;
		}
		virtual int GetTypeID() const = 0;
	};
	
	//struct TypeInfo
	//{
	//protected:
	//	int m_typeID;
	//	int m_lastError;
	//	
	//public:
	//	enum { OBJ_NULL=0, OBJ_ACCEPTOR=1, OBJ_CONNECTION =3 };

	//	TypeInfo()
	//	{
	//		m_typeID = OBJ_NULL;
	//	}

	//	TypeInfo( int id )
	//	{
	//		m_typeID = id;
	//	}
	//	int GetTypeID() const { return m_typeID; }
	//	void SetLastError( int e ) { m_lastError = e; }
	//	int GetLastError() const { return m_lastError; }
	//};

	class ISocketObject
	{
	public:
		virtual ~ISocketObject()
		{
			m_socket.Destroy();
		}

		const Socket& GetSocket() const			{ return m_socket; }
		const SOCKET GetSocketHandle() const	{ return m_socket.GetSocketHandle(); }
		
	protected:
		Socket m_socket;
	};
}