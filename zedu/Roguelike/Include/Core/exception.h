#pragma once

namespace zedu {

	struct Error
	{
		// SetLastError�� �����͸� �����ϹǷ� error�� ������ ��ȿ�ؾ���~!
		// DATA ������ ���� ����Ұ�. �����̳� free�� ���� ���� ��� ����!!
		static void SetLastError( const char* error );
		static const char* GetLastError();
	};
}