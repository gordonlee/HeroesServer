#pragma once

#include "stdafx.h"

#include "Lock.h"

class PacketSerializer
{
private:
	PacketSerializer();
public:
	PacketSerializer(LockSource& lockSource, int bufferSize);
	PacketSerializer(LockSource& lockSource, char* buffer, int bufferCurrentLength, int bufferSize);
	~PacketSerializer();

public:
	bool AddData(void* data, int length);

public:
	void SetRefCount(short refCount);
	bool DecreaseRefCount();

private:
	char* Buffer;
	int BufferCurrentLength;
	int BufferSize;
public:
	char* GetBuffer();
	int GetBufferSize();

private:
	short RefCount;

private:
	LockSource& lockSource;
};