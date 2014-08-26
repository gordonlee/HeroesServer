#pragma once

#include "stdafx.h"

#include "PacketSerializer.h"

#define MakePacket(packetType) \
struct packetType; \
void Handler##packetType(PacketHeader* inPacket, Client* client, TazanServer* server); \
struct packetType : public PacketHeader

#define MakePacketHandler(packetType, flag) \
void Handler##packetType(PacketHeader* inPacket, Client* client, TazanServer* server)

#define RegisterPacketHandler(flag, packetType) \
PacketHandlerMap.insert(std::make_pair(flag, Handler##packetType));

#define HandlePacket(packet, client, server) \
PacketHandlerMap[packet->flag](packet, client, server);

class TazanServer;
class Client;

struct PacketHeader
{
	unsigned short dataSize;
	unsigned char flag;
	unsigned char checksum;
};

typedef void(*PacketHandler)(PacketHeader* inPacket, Client* Client, TazanServer* server);

extern std::map<unsigned char, PacketHandler> PacketHandlerMap;
void InitPacket();