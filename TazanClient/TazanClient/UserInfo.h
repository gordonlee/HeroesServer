#pragma once

#include <Windows.h>
#include <list>
using namespace std;

typedef enum Direction
{
	Down = 0,
	Left = 1,
	Up = 2,
	Right = 3
};

const TCHAR* GetDirectionToString(Direction direction);

struct UserInfo
{
	int UserID;
	int X;
	int Y;
	Direction UserDirection;

	UserInfo()
		: UserID(-1), X(-1), Y(-1), UserDirection(Direction::Down)
	{}
	UserInfo(int userID, int x, int y, Direction userDirection)
		: UserID(userID), X(x), Y(y), UserDirection(userDirection)
	{}
};

extern UserInfo g_MyUserInfo;
extern list<UserInfo*> g_UserInfoList;