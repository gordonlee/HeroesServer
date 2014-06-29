#pragma once

namespace zedu {

	struct Error
	{
		// SetLastError는 포인터만 저장하므로 error는 언제나 유효해야함~!
		// DATA 영역의 것을 사용할것. 스택이나 free된 힙은 절대 사용 금지!!
		static void SetLastError( const char* error );
		static const char* GetLastError();
	};
}