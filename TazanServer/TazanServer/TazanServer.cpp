#include "stdafx.h"
#include <iostream>
#include <set>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include <windows/gperftools/tcmalloc.h>
#ifdef _DEBUG
#pragma comment(lib, "libtcmalloc_minimal-debug.lib")
#else
#pragma comment(lib, "libtcmalloc_minimal.lib")
#endif

using boost::asio::ip::tcp;

CRITICAL_SECTION cs;

struct packet_header
{
	unsigned short data_size;
	unsigned char flag;
	unsigned char checksum;
	char* datas;
};
class entity
{
public:
	virtual ~entity() {}
	virtual void do_read() = 0;
	virtual void do_write(char* send_buffer, std::size_t length) = 0;
};
std::set<std::shared_ptr<entity>> sessions;
class session
	: public entity,
	public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket)
		: socket_(std::move(socket)),
		current_length(0)
	{
	}

	void start()
	{
		sessions.insert(shared_from_this());
		do_read();
	}

	enum { max_length = 65536 };
	char data_[max_length];
	std::size_t current_length;

public:

	void do_read()
	{
		auto self(shared_from_this());
		socket_.async_read_some(boost::asio::buffer(data_ + current_length, max_length - current_length),
			[this, self](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				// Success Read
				current_length += length;

				packet_header* ph = (packet_header*)data_;
				if (ph->data_size + 4 <= current_length)
				{
					if (ph->checksum == 0x55)
					{
						short ref_count = 0;
						char* buf = (char*)tc_malloc(ph->data_size + 6);
						memcpy(buf + 2, data_, ph->data_size + 4);

						if (ph->flag == 1)
						{
							ref_count = 1;
							memcpy(buf, (void*)&ref_count, 2);

							do_write(buf, ph->data_size + 4);
						}
						else if (ph->flag == 2)
						{
							ref_count = sessions.size();
							memcpy(buf, (void*)&ref_count, 2);

							EnterCriticalSection(&cs);
							for (auto it : sessions)
							{
								it->do_write(buf, ph->data_size + 4);
							}
							LeaveCriticalSection(&cs);
						}
						else
						{
						}

						current_length -= ph->data_size + 4;
						memmove(data_, data_ + ph->data_size + 4, current_length);
					}
					else
					{
					}
				}
				else
				{
				}

				do_read();
			}
			else if (ec == boost::asio::error::eof)
			{
				EnterCriticalSection(&cs);
				sessions.erase(shared_from_this());
				LeaveCriticalSection(&cs);

				printf("Session Count : %d Disconnect\n", sessions.size());
				printf("End Of File\n", ec.message().c_str());
			}
			else if (ec == boost::asio::error::operation_aborted ||
				ec == boost::asio::error::connection_reset)
			{
				EnterCriticalSection(&cs);
				sessions.erase(shared_from_this());
				LeaveCriticalSection(&cs);

				printf("Session Count : %d Disconnect\n", sessions.size());
				printf("Client disconnect to server.\n");
			}
			else
			{
				EnterCriticalSection(&cs);
				sessions.erase(shared_from_this());
				LeaveCriticalSection(&cs);

				printf("Session Count : %d Disconnect\n", sessions.size());
				printf("Error Code : %s\n", ec.message().c_str());
			}
		});
	}

	void do_write(char* send_buffer, std::size_t length)
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(send_buffer + 2, length),
			[this, self, send_buffer, length](boost::system::error_code ec, std::size_t send_length)
		{
			short* ref_count = (short*)send_buffer;
			--(*ref_count);

			if (*ref_count <= 0)
			{
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

	tcp::socket socket_;
};

class server
{
public:
	server(boost::asio::io_service& io_service, short port = 9000)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service)
	{
		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				boost::asio::ip::tcp::no_delay option_nodelay(true);
				socket_.set_option(option_nodelay);

				/*boost::asio::socket_base::send_buffer_size option_sendbuffersize(8192 * 10);
				boost::asio::socket_base::receive_buffer_size option_recvbuffersize(8192 * 10);
				socket_.set_option(option_sendbuffersize);
				socket_.set_option(option_recvbuffersize);*/

				EnterCriticalSection(&cs);
				std::make_shared<session>(std::move(socket_))->start();
				/*boost::shared_ptr<session> entity = boost::shared_ptr<session>(new session(socket_));
				entity->start();
				sessions.insert(entity);*/
				LeaveCriticalSection(&cs);

				printf("Session Count : %d Accept\n", sessions.size());
			}

			do_accept();
		});
	}

	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

int main(int argc, char* argv[])
{
	InitializeCriticalSection(&cs);
		
	try
	{
		boost::asio::io_service io_service;

		server s(io_service);

		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	DeleteCriticalSection(&cs);

	return 0;
}