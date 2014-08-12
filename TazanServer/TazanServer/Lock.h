#pragma once

#include "stdafx.h"

typedef CRITICAL_SECTION LockSource;

void InitLockSource(LockSource& lockSource);

class Lock
{
public:
	Lock(LockSource& lock_source)
		: lockSource(&lock_source)
	{
		EnterCriticalSection(lockSource);
	}
	~Lock()
	{
		LeaveCriticalSection(lockSource);
	}

private:
	LockSource* lockSource;
};

void* LockedMalloc(LockSource& lockSource, size_t size);
void LockedFree(LockSource& lockSource, void* ptr);