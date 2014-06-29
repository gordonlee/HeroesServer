#pragma once
#include "Core/types.h"
#include <stdlib.h>

namespace zedu {

	class IQueueAllocator
	{
	public:
		virtual void* Alloc( uint32 size ) { return malloc( size ); }
		virtual void* Realloc( void* ptr, uint32 size ) { return realloc( ptr, size ); }
		virtual void Free( void* ptr ) { return free( ptr ); }
	};

	class IQueue
	{
	protected:
		IQueue() {}

	public:
		enum { PLAIN = 0x00, };

		virtual ~IQueue() {}

		virtual uint32 Write( const void* pData, uint32 size ) = 0;
		virtual uint32 Peek( void* pBuf, uint32 size ) = 0;
		virtual uint32 Read( void* pBuf, uint32 size ) = 0;
		virtual uint32 Size() = 0;
		virtual uint32 FreeSize() = 0;
		virtual uint32 Count() = 0;
		virtual bool Clear() = 0;

		virtual bool Reserve( uint32 size ) = 0;
		virtual uint32 GetReservedSize() = 0;
		virtual bool Resize( uint32 size ) = 0;

		virtual byte* GetBuf() = 0;

		static IQueue* CreateQueue( uint32 defaultSize );
	};

}