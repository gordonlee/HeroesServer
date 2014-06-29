#pragma once
#include <windows.h>

namespace zedu {

	inline int interlocked_increment( volatile int * target, int count = 1 ) 
	{ 
	   __asm
	   { 
		  mov         ebx, target 
		  mov         eax, count 
		  lock xadd   dword ptr [ebx],eax 
	   } 
	}

	inline int interlocked_decrement( volatile int * target, int count = 1 ) 
	{ 
	   __asm
	   {
		  mov         ebx, target 
		  mov         eax, count 
		  neg         eax 
		  lock xadd   dword ptr [ebx],eax 
	   } 
	} 

	inline int interlocked_exchange( volatile int * target, int value )
	{
		volatile int i = *target; // save previous value;

		__asm 
		{ 
			mov         ebx, target 
			mov         eax, value 
			lock xchg   dword ptr [ebx],eax 
		} 
		return i;
	}

	inline void spinlock_init( volatile int* lock )
	{
		*lock = 0;
	}

	inline void spinlock_enter( volatile int* lock )
	{
enter:
		if( interlocked_exchange( lock, 1 ) == 0 ) return;

		goto enter;
	}

	inline void spinlock_leave( volatile int* lock )
	{
		interlocked_exchange( lock, 0 );
	}

	inline bool spinlock_tryenter( volatile int* lock )
	{
		if( interlocked_exchange( lock, 1 ) == 0 ) return true;

		return false;
	}
}