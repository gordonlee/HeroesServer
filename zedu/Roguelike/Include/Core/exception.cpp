#include "exception.h"

namespace zedu {

	// TLS
	__declspec( thread ) char * g_lastError = "Success";

	void Error::SetLastError( const char* error )
	{
		g_lastError = const_cast<char*>( error );
	}

	const char* Error::GetLastError()
	{
		return g_lastError;
	}
}