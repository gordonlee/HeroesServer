#pragma once

#include "Packet.h"

MakePacket(EchoPacket)
{
	char* data;
};

MakePacket(EchoResultPacket)
{
	char* data;
};