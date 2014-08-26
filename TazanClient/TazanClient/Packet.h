#pragma once

#include "UserInfo.h"

struct PacketHeader
{
	short DataSize;
	unsigned char Flag;
	unsigned char Checksum;
};

const unsigned char LoginRequestFlag = 10;
struct LoginRequestMessage : public PacketHeader
{
	LoginRequestMessage()
	{
		DataSize = sizeof(LoginRequestMessage) - 4;
		Flag = LoginRequestFlag;
		Checksum = 0x55;
	}
};

const unsigned char LoginResultFlag = 11;
struct LoginResultMessage : public PacketHeader
{
	LoginResultMessage()
	{
		DataSize = sizeof(LoginResultMessage)-4;
		Flag = LoginResultFlag;
		Checksum = 0x55;
	}

	UserInfo MyUserInfo;
	int UserCount;
	UserInfo OtherUsersInfo;
};

const unsigned char JoinNewUserFlag = 12;
struct JoinNewUserMessage : public PacketHeader
{
	JoinNewUserMessage()
	{
		DataSize = sizeof(JoinNewUserMessage)-4;
		Flag = JoinNewUserFlag;
		Checksum = 0x55;
	}

	UserInfo NewUserInfo;
};

const unsigned char MoveRequestFlag = 20;
struct MoveRequestMessage : public PacketHeader
{
	MoveRequestMessage()
	{
		DataSize = sizeof(MoveRequestMessage)-4;
		Flag = MoveRequestFlag;
		Checksum = 0x55;
	}

	Direction MyDirection;
};

const unsigned char MoveResultFlag = 21;
struct MoveResultMessage : public PacketHeader
{
	MoveResultMessage()
	{
		DataSize = sizeof(MoveResultMessage)-4;
		Flag = MoveRequestFlag;
		Checksum = 0x55;
	}

	UserInfo MovedUserInfo;
};