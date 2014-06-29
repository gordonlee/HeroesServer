#pragma once
#include <string>
#include <vector>

namespace zedu {

	inline int Replace( std::string & str, const char * pszFrom, size_t unFromLen, const char * pszTo, size_t unToLen )
	{
		int cnt  = 0;
		size_t nPos = 0;

		while ( true )
		{
			nPos = str.find( pszFrom, nPos );
			if ( nPos == std::string::npos ) break;

			str.replace( nPos, unFromLen, pszTo );
			nPos += unToLen;

			cnt++;
		}

		return cnt;
	}

	inline int Replace( std::string & str, const std::string & strFrom, const std::string & strTo )
	{
		return Replace( str, strFrom.c_str(), strFrom.size(), strTo.c_str(), strTo.size() );
	}

	void Split( const char * pszCommand, std::vector< std::string >& vToken, const char *pSeparatorList, bool bProcSpecialCharacter );
}