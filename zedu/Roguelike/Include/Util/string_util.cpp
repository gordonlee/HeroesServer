#include "string_util.h"

namespace zedu {

	static inline bool isSep( char _c, const char *cSep )
	{
		char *c = const_cast< char* >( cSep );
		while ( *c )
		{
			if ( *c == _c ) return true;
			++c;
		}

		return false;
	}

	void Split( const char * pszCommand, std::vector< std::string >& vToken, const char *pSeparatorList, bool bProcSpecialCharacter )
	{
		if ( !pszCommand ) return;

		std::string strTmp;
	
		bool bBackSlash = false;
		bool bQuotationMark = false;

		char newCmd[512];
		char *pszCmd = const_cast< char* >( pszCommand );
		unsigned len = (unsigned)strlen(pszCmd)+1;

		// { 백스페이스 처리
		if ( bProcSpecialCharacter && strchr( pszCmd, '\b' ) ) {
			unsigned cp=0;
			for ( unsigned x = 0; x < len ; x ++ ) {
				if ( pszCmd[x] == '\b' ) {
					if ( cp ) cp--;
				} else newCmd[cp++] = pszCmd[x];
			}
			pszCmd = newCmd;
		}
		// }

	
		for ( unsigned i = 0 ; i < len ; i++ )
		{
			// { 역슬래쉬 처리
			if ( !bBackSlash && pszCmd[i] == '\\' && bProcSpecialCharacter ) {
				bBackSlash = true;
				continue;
			}
			// }

			if ( !bBackSlash ) {
				if ( ( pszCmd[i] == '\"' && bProcSpecialCharacter			)   ||	// 따옴표가 나왔거나
					 ( !bQuotationMark && (isSep(pszCmd[i], pSeparatorList) ) ) || 	// 스페이스로 파싱중일때 스페이스가 나왔거나
					 ( pszCmd[i] == '\0'					) )			// 문장의 끝일경우
				 
				 
				{
					if ( pszCmd[i] == '\"' )
					{
						bQuotationMark ^= true;				// 따옴표가 나오면 토글
						if (bQuotationMark) continue;
					}

					// 구문 저장
					if ( strTmp.size() ) vToken.push_back( strTmp );
					strTmp.erase( strTmp.begin(), strTmp.end() );
					continue;
				}
			}

			strTmp += pszCmd[i];
			bBackSlash = false;
		}
	}
}