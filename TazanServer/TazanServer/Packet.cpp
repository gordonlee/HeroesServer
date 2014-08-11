#include "stdafx.h"

#include "Packet.h"

#include "EchoPacket.h"
#include "BroadcastPacket.h"

std::map<unsigned char, PacketHandler> PacketHandlerMap;
void InitPacket()
{
	RegisterPacketHandler(1, EchoPacket);
	RegisterPacketHandler(2, BroadcastPacket);
}