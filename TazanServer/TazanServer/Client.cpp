#include "stdafx.h"

#include "Client.h"

#include "TazanServer.h"
#include "Packet.h"
#include "PacketSerializer.h"

#include "Lock.h"

Client::Client(tcp::socket& socket)
: ClientSocket(std::move(socket)),
CurrentReadLength(0)
{
}

void Client::OnConnected(TazanServer* cheetah)
{
	MyTazan = cheetah;

	MyTazan->InsertClient(shared_from_this());

	DoRead();
}

void Client::OnDisconnect()
{
	MyTazan->EraseClient(shared_from_this());
}

void Client::DoRead()
{
	auto self(shared_from_this());
	ClientSocket.async_read_some(
		boost::asio::buffer(ClientReadBuffer + CurrentReadLength,
		MAX_READBUFFER_SIZE - CurrentReadLength),
		[this, self](boost::system::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			/* Check Kernel Buffer is either Overflow
			boost::asio::socket_base::bytes_readable c(true);
			ClientSocket.io_control(c);
			if (c.get() >= 8192)
			{
				OnDisconnect();

				printf("Session is disconnected Count : %d\n", MyTazan->GetClientCount());
				printf("Receive Buffer is Overflow!!\n");

				return;
			}
			*/

			// Success Read
			CurrentReadLength += length;

			PacketHeader* packetHeader = (PacketHeader*)ClientReadBuffer;
			if (packetHeader->dataSize + 4 <= CurrentReadLength)
			{
				HandlePacket(packetHeader, ((Client*)this), MyTazan);

				CurrentReadLength -= packetHeader->dataSize + 4;
				memmove(ClientReadBuffer, ClientReadBuffer + packetHeader->dataSize + 4, CurrentReadLength);
			}
			else
			{
			}

			DoRead();
		}
		else
		{
			OnDisconnect();

			printf("Session is disconnected Count : %d\n", MyTazan->GetClientCount());
			if (ec == boost::asio::error::eof)
			{
				printf("End Of File\n", ec.message().c_str());
			}
			else if (ec == boost::asio::error::operation_aborted ||
				ec == boost::asio::error::connection_reset)
			{
				printf("Client disconnect to server.\n");
			}
			else
			{
				printf("Unknown Error : %s\n", ec.message().c_str());
			}
		}
	});
}

void Client::DoWrite(PacketSerializer* ps)
{
	auto self(shared_from_this());
	boost::asio::async_write(ClientSocket, boost::asio::buffer(ps->GetBuffer(), ps->GetBufferSize()),
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