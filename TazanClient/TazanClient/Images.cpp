#include "App.h"
#include "Images.h"

namespace Images
{
	// 여기에서 인스턴스를 NULL로 초기화해야합니다.
	Image *pImgTile = NULL;
	Image *pImgOveredTile = NULL;

	Image *pImgCharacter = NULL;

	bool Loading(void)
	{
		// 여기에서 이미지를 읽습니다.
		// 예외 처리를 해주면 좋습니다.
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
		// 여기에서 이미지를 반환합니다.
		// if(Image != NULL) delete Image; 할 필요 없이 delete만 해도 됩니다.
		delete pImgTile;
		delete pImgOveredTile;
		delete pImgCharacter;
	}
}