#pragma once

#include <stdarg.h>
#include <stdio.h>

namespace zedu {

	struct Logger
	{
		typedef void (*Output)(const char*);
		static Output m_output;

		Logger()
		{
			m_output = NULL;
		}

		static Logger& GetLogger()
		{
			static Logger inst;
			return inst;
		}

		static void SetOutputHandler( Output fn )
		{
			m_output = fn;
		}

		static void Printf(const char* str, ...)
		{
			char buf[2048];
			va_list va;
			va_start( va, str );
			_vsnprintf( buf, sizeof(buf), str, va );
			va_end( va );

			if( m_output )
				m_output( buf );
		}
	};
}

#define log_print		Logger::Printf
#define cprint			Logger::Printf
//#define cprint	Console::DefaultConsole().Printf