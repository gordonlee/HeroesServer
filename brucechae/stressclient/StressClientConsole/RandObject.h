#pragma once

#include <random>

using namespace std;

class Random
{
	random_device device_;
	mt19937_64 engine_;
	uniform_int_distribution<int> dist_;

public:
	Random() : engine_(device_()), dist_(uniform_int_distribution<int>(0, INT_MAX))
	{
	}

	~Random()
	{
	}

	int NextInt(int max)
	{
		return dist_(engine_) % (max + 1);
	}
};