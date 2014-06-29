#pragma once
#include <windows.h>
#include "socket_object.h"
#include "Thread/lock.h"
#include "Core/Types.h"

namespace zedu {

	struct OverlappedAllocator;
	struct OverlappedIO : public OVERLAPPED
	{
		enum { IO_NULL=0, IO_CONNECT=1, IO_ACCEPT=2, IO_DATAGRAM=3, IO_SEND=4, IO_RECV=5, IO_CLOSE=6 };

		HANDLE hFileHandle;
		DWORD dwSize;
		ITypeInfo* pObj;
		char typeIO;
		char* pBuf;
		OverlappedAllocator* pAllocator;

		void Init( HANDLE _hFileHandle=NULL, ITypeInfo* _pObj=NULL, char* _pBuf=NULL )
		{
			Internal = 0;
			InternalHigh = 0;
			Offset = 0;
			OffsetHigh = 0;
			hEvent = NULL;
			dwSize = 0;
			hFileHandle = _hFileHandle;
			typeIO = OverlappedIO::IO_NULL;
			pObj = _pObj;
			pBuf = _pBuf;
			pAllocator = 0;
		}

		OverlappedIO( HANDLE _hFileHandle=NULL, ITypeInfo* _pObj=NULL, char* _pBuf=NULL )
		{
			Init( _hFileHandle, _pObj, _pBuf );
		}

		~OverlappedIO()
		{
		}
	};

	struct OverlappedAllocator : public ILock
	{
		OverlappedAllocator();
		virtual ~OverlappedAllocator();

		virtual OverlappedIO* allocOverlapped()						{ return new OverlappedIO; }
		virtual void freeOverlapped( OverlappedIO* ptr )			{ delete ptr; }

		virtual void Lock( const char* fileName="", int lineNum=0 ) {}
		virtual void Unlock()										{}
		virtual bool IsLocked()										{ return false; }

		static OverlappedAllocator* DefaultAllocator();
	};
}