#include "stdafx.h"

#include "Entity.h"

#include "TazanServer.h"
#include "Packet.h"
#include "PacketSerializer.h"

#include "Lock.h"

Entity::Entity(tcp::socket& socket)
: EntitySocket(std::move(socket)),
CurrentReadLength(0)
{
}

void Entity::OnConnected(TazanServer* cheetah)
{
	MyTazan = cheetah;

	MyTazan->InsertEntity(shared_from_this());

	DoRead();
}

void Entity::OnDisconnect()
{
	MyTazan->EraseEntity(shared_from_this());
}

void Entity::DoRead()
{
	auto self(shared_from_this());
	EntitySocket.async_read_some(
		boost::asio::buffer(EntityReadBuffer + CurrentReadLength,
		MAX_READBUFFER_SIZE - CurrentReadLength),
		[this, self](boost::system::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			/* Check Kernel Buffer is either Overflow
			boost::asio::socket_base::bytes_readable c(true);
			EntitySocket.io_control(c);
			if (c.get() >= 8192)
			{
				OnDisconnect();

				printf("Session is disconnected Count : %d\n", MyTazan->GetEntityCount());
				printf("Receive Buffer is Overflow!!\n");

				return;
			}
			*/

			// Success Read
			CurrentReadLength += length;

			PacketHeader* packetHeader = (PacketHeader*)EntityReadBuffer;
			if (packetHeader->dataSize + 4 <= CurrentReadLength)
			{
				HandlePacket(packetHeader, ((Entity*)this), MyTazan);

				CurrentReadLength -= packetHeader->dataSize + 4;
				memmove(EntityReadBuffer, EntityReadBuffer + packetHeader->dataSize + 4, CurrentReadLength);
			}
			else
			{
			}

			DoRead();
		}
		else if (ec == boost::asio::error::eof)
		{
			OnDisconnect();

			printf("Session is disconnected Count : %d\n", MyTazan->GetEntityCount());
			printf("End Of File\n", ec.message().c_str());
		}
		else if (ec == boost::asio::error::operation_aborted ||
			ec == boost::asio::error::connection_reset)
		{
			OnDisconnect();

			printf("Session is disconnected Count : %d\n", MyTazan->GetEntityCount());
			printf("Client disconnect to server.\n");
		}
		else
		{
			OnDisconnect();

			printf("Session is disconnected Count : %d\n", MyTazan->GetEntityCount());
			printf("Unknown Error : %s\n", ec.message().c_str());
		}
	});
}

void Entity::DoWrite(PacketSerializer* ps)
{
	auto self(shared_from_this());
	boost::asio::async_write(EntitySocket, boost::asio::buffer(ps->GetBuffer(), ps->GetBufferSize()),
		[this, self, ps](boost::system::error_code ec, std::size_t send_length)
	{
		if (!ec)
		{
		}
		else
		{
			/*if (send_length != 0 && send_length != ps->GetBufferSize())
			{
				printf("Send Length is different [ %d : %d ]\n", send_length, ps->GetBufferSize());
			}
			printf("Send Error Code : %s\n", ec.message().c_str());*/
		}

		if (ps->DecreaseRefCount())
		{
			delete ps;
		}
	});
}