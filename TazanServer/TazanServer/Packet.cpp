#include "stdafx.h"

#include "Packet.h"

#include "EchoPacket.h"
#include "BroadcastPacket.h"
#include "LoginMessage.h"
#include "MoveMessage.h"

std::map<unsigned char, PacketHandler> PacketHandlerMap;
void InitPacket()
{
	RegisterPacketHandler(1, EchoPacket);
	RegisterPacketHandler(2, BroadcastPacket);
	RegisterPacketHandler(10, LoginRequestMessage);
	RegisterPacketHandler(20, MoveRequestMessage);
}