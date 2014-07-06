#include <Windows.h>
#include <mmsystem.h>
#include "timer.h"

#pragma comment(lib, "winmm.lib")


namespace zedu {

	ulong g_startedMSec = 0;
	
	void StartMiliSeconds()
	{
		g_startedMSec = timeGetTime();
	}
	ulong GetMiliSeconds()
	{
		return timeGetTime() - g_startedMSec;
	}
}