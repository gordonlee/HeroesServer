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

		// { �齺���̽� ó��
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
			// { �������� ó��
			if ( !bBackSlash && pszCmd[i] == '\\' && bProcSpecialCharacter ) {
				bBackSlash = true;
				continue;
			}
			// }

			if ( !bBackSlash ) {
				if ( ( pszCmd[i] == '\"' && bProcSpecialCharacter			)   ||	// ����ǥ�� ���԰ų�
					 ( !bQuotationMark && (isSep(pszCmd[i], pSeparatorList) ) ) || 	// �����̽��� �Ľ����϶� �����̽��� ���԰ų�
					 ( pszCmd[i] == '\0'					) )			// ������ ���ϰ��
				 
				 
				{
					if ( pszCmd[i] == '\"' )
					{
						bQuotationMark ^= true;				// ����ǥ�� ������ ���
						if (bQuotationMark) continue;
					}

					// ���� ����
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