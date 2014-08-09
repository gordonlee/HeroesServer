#include "stdafx.h"

#include "Entity.h"

#include "TazanServer.h"

struct packet_header
{
	unsigned short data_size;
	unsigned char flag;
	unsigned char checksum;
	char* datas;
};

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

			packet_header* ph = (packet_header*)EntityReadBuffer;
			if (ph->data_size + 4 <= CurrentReadLength)
			{
				if (ph->checksum == 0x55)
				{
					short ref_count = 0;
					char* buf = NULL;
					{
						Lock lock(MyTazan->GetLockSource());
						buf = (char*)tc_malloc(ph->data_size + 6);
					}
					memcpy(buf + 2, EntityReadBuffer, ph->data_size + 4);

					if (ph->flag == 1)
					{
						ref_count = 1;
						memcpy(buf, (void*)&ref_count, 2);

						DoWrite(buf, ph->data_size + 4);
					}
					else if (ph->flag == 2)
					{
						std::set<std::shared_ptr<Entity>> Entities = MyTazan->GetEntities();
						ref_count = Entities.size();
						memcpy(buf, (void*)&ref_count, 2);
						
						for (auto& it : Entities)
						{
							it->DoWrite(buf, ph->data_size + 4);
						}
						/*parallel_for_each(Entities.begin(), Entities.end(), [buf, ph](const std::shared_ptr<Entity>& it)
						{
							it->DoWrite(buf, ph->data_size + 4);
						});*/
					}
					else
					{
					}

					CurrentReadLength -= ph->data_size + 4;
					memmove(EntityReadBuffer, EntityReadBuffer + ph->data_size + 4, CurrentReadLength);
				}
				else
				{
				}
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

void Entity::DoWrite(char* send_buffer, std::size_t length)
{
	auto self(shared_from_this());
	boost::asio::async_write(EntitySocket, boost::asio::buffer(send_buffer + 2, length),
		[this, self, send_buffer, length](boost::system::error_code ec, std::size_t send_length)
	{
		short* ref_count = (short*)send_buffer;
		--(*ref_count);

		if (*ref_count <= 0)
		{
			Lock lock(MyTazan->GetLockSource());
			tc_free(send_buffer);
		}

		if (!ec)
		{
		}
		else
		{
			if (send_length != 0 && send_length != length)
			{
				printf("Send Length is different [ %d : %d ]\n", send_length, length);
			}
			printf("Send Error Code : %s\n", ec.message().c_str());
		}
	});
}