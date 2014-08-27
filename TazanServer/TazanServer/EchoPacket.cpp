#include "stdafx.h"

#include "EchoPacket.h"

#include "TazanServer.h"
#include "Client.h"

MakePacketHandler(EchoPacket, 1)
{
	EchoPacket* packet = (EchoPacket*)inPacket;
	if (packet->checksum == 0x55)
	{
		PacketHeader packetHeader;
		packetHeader.dataSize = inPacket->dataSize;
		packetHeader.flag = 0x1;
		packetHeader.checksum = 0x55;

		PacketSerializer* ps = new PacketSerializer(server->GetLockSource(), sizeof(PacketHeader) + inPacket->dataSize);
		ps->AddData(&packetHeader, sizeof(PacketHeader));
		ps->AddData((char*)&packet->data, packetHeader.dataSize);

		ps->SetRefCount(1);

		client->DoWrite(ps);
	}
}