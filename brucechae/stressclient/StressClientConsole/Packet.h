#pragma once

#pragma pack(1)
struct PacketHeader
{
	UInt16 dataSize_;
	Int8 flag_;
	Int8 checkSum_;

public:
	PacketHeader() : dataSize_(0), flag_(0), checkSum_(0)
	{
	}
};

struct Packet : public PacketHeader
{
	char data_[65532];

public:
	Packet()
	{
		ZeroMemory(data_, 65532);
	}
};

struct PacketData
{
	int packetId_;
	int clientId_;
	int sentTime_;
	char data_[65528];
};
#pragma pack()