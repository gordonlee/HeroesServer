#include "stdafx.h"

#include "EchoPacket.h"

#include "TazanServer.h"
#include "Client.h"

MakePacketHandler(EchoPacket, 1)
{
	EchoPacket* packet = (EchoPacket*)inPacket;
	if (packet->checksum == 0x55)
	{
		EchoResultPacket outPacket;
		outPacket.dataSize = inPacket->dataSize;
		outPacket.flag = 0x1;
		outPacket.checksum = 0x55;
		outPacket.data = (char*)&packet->data;

		PacketSerializer* ps = new PacketSerializer(server->GetLockSource(), outPacket.dataSize + 4);
		ps->AddData(&outPacket, 4);
		ps->AddData(outPacket.data, outPacket.dataSize);

		ps->SetRefCount(1);

		client->DoWrite(ps);
	}
}