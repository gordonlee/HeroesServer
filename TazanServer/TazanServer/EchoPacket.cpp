#include "stdafx.h"

#include "EchoPacket.h"

#include "TazanServer.h"
#include "Entity.h"

MakePacketHandler(EchoPacket, 1)
{
	if (inPacket->checksum)
	{
		char* buf = NULL;
		{
			Lock lock(server->GetLockSource());
			buf = (char*)tc_malloc(inPacket->dataSize + 6);
		}

		short ref_count = 1;
		memcpy(buf, (void*)&ref_count, 2);
		memcpy(buf + 2, inPacket, inPacket->dataSize + 4);

		entity->DoWrite(buf, inPacket->dataSize + 4);
	}
}