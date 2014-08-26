#include "stdafx.h"

#include "LoginMessage.h"

#include "TazanServer.h"
#include "Client.h"

int maxUserID = 1;

MakePacketHandler(LoginRequestMessage, 10)
{
	LoginRequestMessage* packet = (LoginRequestMessage*)inPacket;
	if (packet->checksum == 0x55)
	{
		std::set<std::shared_ptr<Client>> Entities = server->GetEntities();
		Lock lock(server->GetLockSource());

		int LoginedUserCount = 0;

		bool UserPosition[30][30] = { false, };
		for (auto& it : Entities)
		{
			if (it->IsLogin == true)
			{
				UserPosition[it->ClientUserInfo.X][it->ClientUserInfo.Y] = true;
				LoginedUserCount++;
			}
		}

		client->ClientUserInfo.UserID = maxUserID++;
		for (client->ClientUserInfo.X = 0; client->ClientUserInfo.X < 30; ++client->ClientUserInfo.X)
		{
			bool IsFind = false;
			for (client->ClientUserInfo.Y = 0; client->ClientUserInfo.Y < 30; ++client->ClientUserInfo.Y)
			{
				if (UserPosition[client->ClientUserInfo.X][client->ClientUserInfo.Y] == false)
				{
					IsFind = true;
					break;
				}
			}

			if (IsFind == true)
			{
				break;
			}
		}
		client->ClientUserInfo.UserDirection = Direction::Down;

		printf("[%d] Login : (%d,%d)\n", client->ClientUserInfo.UserID, client->ClientUserInfo.X, client->ClientUserInfo.Y);

		PacketHeader packetHeaderJoinNewUser = { sizeof(UserInfo), 12, 0x55 };
		PacketSerializer* psJoinNewUser = new PacketSerializer(server->GetLockSource(), 4 + sizeof(UserInfo));
		psJoinNewUser->AddData(&packetHeaderJoinNewUser, 4);
		psJoinNewUser->AddData(&client->ClientUserInfo, sizeof(UserInfo));
		psJoinNewUser->SetRefCount(LoginedUserCount);

		PacketHeader packetHeaderLoginResult = { sizeof(UserInfo)+4 + sizeof(UserInfo)* LoginedUserCount, 11, 0x55 };
		PacketSerializer* psLoginResult = new PacketSerializer(server->GetLockSource(), 4 + sizeof(UserInfo) + 4 + sizeof(UserInfo) * LoginedUserCount);
		psLoginResult->AddData(&packetHeaderLoginResult, 4);
		psLoginResult->AddData(&client->ClientUserInfo, sizeof(UserInfo));
		psLoginResult->AddData(&LoginedUserCount, 4);
		for (auto& it : Entities)
		{
			if (it->IsLogin == true)
			{
				it->DoWrite(psJoinNewUser);
				psLoginResult->AddData(&it->ClientUserInfo, sizeof(UserInfo));
			}
		}
		psLoginResult->SetRefCount(1);

		client->IsLogin = true;
		client->DoWrite(psLoginResult);
	}
}