#pragma once

#include <array>
#include <stdarg.h>

using namespace std;

class Log
{
	static const int LOG_BUFFER_SIZE = 4096;

public:
	static Log& GetInstance()
	{
		static Log instance;
		return instance;
	}

	static void Std(const char* format, ...)
	{
		va_list ap;
		va_start(ap, format);
		GetInstance().Write(format, ap);
		va_end(ap);
	}

private:
	void Write(const char* format, char* ap)
	{
		array<char, LOG_BUFFER_SIZE> buffer;
		_vsnprintf_s(buffer.data(), LOG_BUFFER_SIZE, LOG_BUFFER_SIZE - 1, format, ap);
		fprintf(stderr, "%s", buffer.data());
		fflush(stdout);
	}
};

#define LOG Log::Std