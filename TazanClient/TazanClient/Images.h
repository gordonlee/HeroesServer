#pragma once

namespace Images
{
	// 여기에 이미지 인스턴스를 추가합니다.
	extern Image *pImgTile;
	extern Image *pImgOveredTile;

	extern Image *pImgCharacter;

	// 이미지들을 읽고 반환하는 함수입니다.
	bool Loading(void);
	void Release(void);
}