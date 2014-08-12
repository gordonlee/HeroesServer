#include "stdafx.h"

#include "Lock.h"

void InitLockSource(LockSource& lockSource)
{
	InitializeCriticalSection(&lockSource);
}

void* LockedMalloc(LockSource& lockSource, size_t size)
{
	Lock lock(lockSource);
	return tc_malloc(size);
}

void LockedFree(LockSource& lockSource, void* ptr)
{
	Lock lock(lockSource);
	tc_free(ptr);
}