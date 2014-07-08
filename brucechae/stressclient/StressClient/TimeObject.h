#pragma once

#include <chrono>

using namespace std;

class Time
{
public:
	static UInt64 Now()
	{
		return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	}

	static chrono::milliseconds To(UInt64 msec)
	{
		return chrono::milliseconds(msec);
	}
};