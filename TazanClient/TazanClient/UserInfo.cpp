#include "UserInfo.h"

const TCHAR* GetDirectionToString(Direction direction)
{
	switch (direction)
	{
	case Direction::Down:
	{
							return TEXT("Down");
	}
	case Direction::Left:
	{
							return TEXT("Left");
	}
	case Direction::Up:
	{
						  return TEXT("Up");
	}
	case Direction::Right:
	{
							 return TEXT("Right");
	}
	}

	return TEXT("None");
}