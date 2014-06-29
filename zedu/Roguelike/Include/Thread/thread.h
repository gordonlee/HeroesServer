#pragma once

namespace zedu {

	struct ThreadInfo
	{
		char threadName[64];
	};

	void XSetThreadName( __int32 threadID, const char* threadName );

	inline void SetThreadName( __int32 threadID, const char* threadName ) { XSetThreadName(threadID, threadName); }
}