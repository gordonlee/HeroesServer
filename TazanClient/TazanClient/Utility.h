#pragma once

bool CheckPointInRect(int x, int y, int rx, int ry, int rw, int rh)
{
	if (rx <= x && x < rx + rw &&
		ry <= y && y < ry + rh)
	{
		return true;
	}

	return false;
}