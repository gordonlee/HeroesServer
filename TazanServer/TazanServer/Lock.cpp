#include "stdafx.h"

#include "Lock.h"

void InitLockSource(LockSource& lockSource)
{
	InitializeCriticalSection(&lockSource);
}