#include "stdafx.h"

#include "BroadcastPacket.h"

#include "TazanServer.h"
#include "Entity.h"

MakePacketHandler(BroadcastPacket, 2)
{
	if (inPacket->checksum)
	{
		char* buf = NULL;
		{
			Lock lock(server->GetLockSource());
			buf = (char*)tc_malloc(inPacket->dataSize + 6);
		}

		std::set<std::shared_ptr<Entity>> Entities = server->GetEntities();
		short ref_count = Entities.size();
		memcpy(buf, (void*)&ref_count, 2);
		memcpy(buf + 2, inPacket, inPacket->dataSize + 4);
			
		for (auto& it : Entities)
		{
			it->DoWrite(buf, inPacket->dataSize + 4);
		}
	}
}