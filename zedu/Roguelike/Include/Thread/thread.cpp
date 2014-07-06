#include "thread.h"
#include <windows.h>

namespace zedu {

	__declspec( thread ) ThreadInfo g_threadInfo;

	// VC 디버거 thread list에 등록
	const DWORD MS_VC_EXCEPTION=0x406D1388;
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)

	void XSetThreadName( __int32 threadID, const char* threadName )
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = threadID;
		info.dwFlags = 0;

		strncpy( g_threadInfo.threadName, threadName, sizeof(g_threadInfo.threadName) );

		__try
		{
			RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	const char* GetThreadName()
	{
		return g_threadInfo.threadName;
	}
}