#include "App.h"
#include "Images.h"

namespace Images
{
	// ���⿡�� �ν��Ͻ��� NULL�� �ʱ�ȭ�ؾ��մϴ�.
	Image *pImgTile = NULL;
	Image *pImgOveredTile = NULL;

	Image *pImgCharacter = NULL;

	bool Loading(void)
	{
		// ���⿡�� �̹����� �н��ϴ�.
		// ���� ó���� ���ָ� �����ϴ�.
		pImgTile = new Image(TEXT("resource\\tile.png"));
		if (!pImgTile) return false;
		pImgOveredTile = new Image(TEXT("resource\\overed_tile.png"));
		if (!pImgOveredTile) return false;
		pImgCharacter = new Image(TEXT("resource\\character.png"));
		if (!pImgCharacter) return false;

		return true;
	}

	void Release(void)
	{
		// ���⿡�� �̹����� ��ȯ�մϴ�.
		// if(Image != NULL) delete Image; �� �ʿ� ���� delete�� �ص� �˴ϴ�.
		delete pImgTile;
		delete pImgOveredTile;
		delete pImgCharacter;
	}
}