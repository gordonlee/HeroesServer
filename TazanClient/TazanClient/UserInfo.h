#pragma once

typedef enum Direction
{
	Down = 0,
	Left = 1,
	Up = 2,
	Right = 3
};

struct UserInfo
{
	int UserID;
	int X;
	int Y;
	Direction UserDirection;
};