#include "stdafx.h"

#include "PacketSerializer.h"

#include "Lock.h"

PacketSerializer::PacketSerializer(LockSource& lockSource, int bufferSize)
	: Buffer(NULL), BufferCurrentLength(0), BufferSize(bufferSize), lockSource(lockSource)
{
	Buffer = (char*)LockedMalloc(lockSource, bufferSize);
}

PacketSerializer::PacketSerializer(LockSource& lockSource, char* buffer, int bufferCurrentLength, int bufferSize)
	: Buffer(buffer), BufferCurrentLength(bufferCurrentLength), BufferSize(bufferSize), lockSource(lockSource)
{
}

PacketSerializer::~PacketSerializer()
{
}

bool PacketSerializer::AddData(void* data, int length)
{
	if (BufferCurrentLength + length > BufferSize)
	{
		return false;
	}

	memcpy(Buffer + BufferCurrentLength, data, length);
	BufferCurrentLength += length;

	return true;
}

void PacketSerializer::SetRefCount(short refCount)
{
	RefCount = refCount;
}

bool PacketSerializer::DecreaseRefCount()
{
	Lock lock(lockSource);

	--RefCount;

	if (RefCount > 0)
	{
		return false;
	}

	if (Buffer != NULL)
	{
		tc_free(Buffer);
		Buffer = NULL;
	}

	return true;
}

char* PacketSerializer::GetBuffer()
{
	return Buffer;
}

int PacketSerializer::GetBufferSize()
{
	return BufferSize;
}