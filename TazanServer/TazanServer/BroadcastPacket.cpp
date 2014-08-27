#include "stdafx.h"

#include "BroadcastPacket.h"

#include "TazanServer.h"
#include "Client.h"

MakePacketHandler(BroadcastPacket, 2)
{
	BroadcastPacket* packet = static_cast<BroadcastPacket*>(inPacket);
	if (packet->checksum == 0x55)
	{
		PacketHeader packetHeader;
		packetHeader.dataSize = inPacket->dataSize;
		packetHeader.flag = 0x2;
		packetHeader.checksum = 0x55;

		PacketSerializer* ps = new PacketSerializer(server->GetLockSource(), sizeof(PacketHeader) + inPacket->dataSize);
		ps->AddData(&packetHeader, sizeof(PacketHeader));
		ps->AddData((char*)&packet->data, packetHeader.dataSize);

		Lock lock(server->GetLockSource());
		std::set<std::shared_ptr<Client>>& Entities = server->GetEntities();
		ps->SetRefCount(Entities.size());

		for (auto& it : Entities)
		{
			it->DoWrite(ps);
		}
	}
}