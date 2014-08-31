#include "stdafx.h"

#include "MoveMessage.h"

#include "TazanServer.h"
#include "Client.h"

MakePacketHandler(MoveRequestMessage, 20)
{
	MoveRequestMessage* packet = (MoveRequestMessage*)inPacket;
	if (packet->checksum == 0x55)
	{
		std::set<std::shared_ptr<Client>> Entities = server->GetEntities();

		printf("[%d] Move : (%d,%d), %d -> ", client->ClientUserInfo.UserID, client->ClientUserInfo.X, client->ClientUserInfo.Y, client->ClientUserInfo.UserDirection);
		
		int dx = 0, dy = 0;
		switch (packet->MyDirection)
		{
		case Direction::Down:
			dy = 1;
			break;
		case Direction::Left:
			dx = -1;
			break;
		case Direction::Up:
			dy = -1;
			break;
		case Direction::Right:
			dx = 1;
			break;
		}

		if (client->ClientUserInfo.X + dx < 0 || client->ClientUserInfo.X + dx >= 30 ||
			client->ClientUserInfo.Y + dy < 0 || client->ClientUserInfo.Y + dy >= 30)
		{
		}
		else
		{
			bool CanMove = true;
			for (auto& it : Entities)
			{
				if (it->IsLogin == true)
				{
					if (it->ClientUserInfo.X == client->ClientUserInfo.X + dx &&
						it->ClientUserInfo.Y == client->ClientUserInfo.Y + dy)
					{
						CanMove = false;
						break;
					}
				}
			}

			if (CanMove == true)
			{
				client->ClientUserInfo.X += dx;
				client->ClientUserInfo.Y += dy;
			}
		}
		client->ClientUserInfo.UserDirection = packet->MyDirection;
		
		printf("(%d,%d), %d\n", client->ClientUserInfo.X, client->ClientUserInfo.Y, client->ClientUserInfo.UserDirection);

		PacketHeader packetHeader = { sizeof(UserInfo), 21, 0x55 };
		PacketSerializer* psMoveResult = new PacketSerializer(server->GetLockSource(), sizeof(PacketHeader)+packetHeader.dataSize);
		psMoveResult->AddData(&packetHeader, sizeof(PacketHeader));
		psMoveResult->AddData(&client->ClientUserInfo, sizeof(UserInfo));
		psMoveResult->SetRefCount(Entities.size());
		for (auto& it : Entities)
		{
			if (it->IsLogin == true)
			{
				it->DoWrite(psMoveResult);
			}
		}
	}
}