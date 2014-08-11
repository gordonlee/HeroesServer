#pragma once

#include "stdafx.h"

#define MakePacket(packetType) \
struct packetType; \
void Handler##packetType(PacketHeader* inPacket, Entity* entity, TazanServer* server); \
struct packetType : public PacketHeader

#define MakePacketHandler(packetType, flag) \
void Handler##packetType(PacketHeader* inPacket, Entity* entity, TazanServer* server)

#define RegisterPacketHandler(flag, packetType) \
PacketHandlerMap.insert(std::make_pair(flag, Handler##packetType));

#define HandlePacket(packet, entity, server) \
PacketHandlerMap[packet->flag](packet, entity, server);

class TazanServer;
class Entity;

struct PacketHeader
{
	unsigned short dataSize;
	unsigned char flag;
	unsigned char checksum;
};

typedef void(*PacketHandler)(PacketHeader* inPacket, Entity* entity, TazanServer* server);

extern std::map<unsigned char, PacketHandler> PacketHandlerMap;
void InitPacket();