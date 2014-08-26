#include "stdafx.h"

#include "BroadcastPacket.h"

#include "TazanServer.h"
#include "Client.h"

MakePacketHandler(BroadcastPacket, 2)
{
	BroadcastPacket* packet = static_cast<BroadcastPacket*>(inPacket);
	if (packet->checksum == 0x55)
	{
		BroadcastResultPacket outPacket;
		outPacket.dataSize = inPacket->dataSize;
		outPacket.flag = 0x2;
		outPacket.checksum = 0x55;
		outPacket.data = (char*)&packet->data;

		PacketSerializer* ps = new PacketSerializer(server->GetLockSource(), outPacket.dataSize + 4);
		ps->AddData(&outPacket, 4);
		ps->AddData(outPacket.data, outPacket.dataSize);

		std::set<std::shared_ptr<Client>> Entities = server->GetEntities();
		ps->SetRefCount(Entities.size());

		for (auto& it : Entities)
		{
			it->DoWrite(ps);
		}
	}
}