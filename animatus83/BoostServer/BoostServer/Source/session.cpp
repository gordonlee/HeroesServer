#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "session.h"
#include "Common/Define.h"
#include "server.h"

using boost::asio::ip::tcp;

session::session(boost::asio::io_service& io_service, server* parent)
: socket_(io_service),
strand_(io_service),
cir_buffer_(DEF_MAX_PACKETSIZE * 10)
{
	boost::system::error_code ec;
	socket_.set_option(boost::asio::socket_base::reuse_address(true), ec);
	socket_.set_option(boost::asio::socket_base::receive_buffer_size(DEF_MAX_PACKETSIZE), ec);
	socket_.set_option(boost::asio::socket_base::linger(true, 20), ec);

	parentserver_ = parent;
		
}
session::~session()
{
	parentserver_->DelSession(this);
}

tcp::socket& session::socket()
{
	return socket_;
}

void session::start()
{
	parentserver_->AddSession(this);


	socket_.async_read_some(boost::asio::buffer(data_, max_length),
		strand_.wrap( boost::bind(&session::handle_read, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred) ) );
}


void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (!error)
	{
		cir_buffer_.insert(cir_buffer_.end(), data_, data_ + bytes_transferred);

		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			strand_.wrap(boost::bind(&session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)));

		ParsePacket();		
	}
	else
	{
		delete this;
	}
}

void session::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		/*
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			boost::bind(&session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
		*/
	}
	else
	{
		delete this;
	}
}


void session::ParsePacket()
{
	
	while (1)
	{
		if ( cir_buffer_.empty())
			return;

		// 패킷 구조 
		// Header ( int ) / BODY (BYTE[65532] )
			// Header 구조 : 길이 (Short) / FLAG (BYTE) / Check Sum (BYTE)
		
		// 최소 헤더 사이즈
		if (sizeof(int) > cir_buffer_.size())
			return;

		char* begin = (char*)cir_buffer_.linearize();

		
		int index = 0;
		unsigned short bodysize = 0;
		BYTE flag = 0;
		

		if ( CheckHeader(begin, bodysize, flag , DEF_HEADER_CHECKSUM )  == false)
		{
			//만약 패킷이 깨져 들어 왔을 경우에는 해당 패킷의 마지막 또는 다음 패킷 헤더의 앞까지 버려야 한다.
			// Ver 0.1 에서는 일단 break; 처리한다.			
			return;
		}
		
		
		char packet[DEF_MAX_PACKETSIZE];//65532
		ZeroMemory(packet, DEF_MAX_PACKETSIZE);

		if (bodysize + DEF_HEADER_SIZE <= cir_buffer_.size())
		{			
			GetData(packet, begin, index ,bodysize + DEF_HEADER_SIZE);

			cir_buffer_.erase_begin(index);
		}
		else
		{
			// 버퍼의 크기보다 패킷의 크기가 작으면 해당 패킷은 처리 하지 않는다.
			return;
		}

		
		ProcessPacket(packet, bodysize, flag);
	}
}

bool session::CheckHeader(char* begin, unsigned short& bodysize, BYTE& flag, BYTE checksum)
{
	int idx = 0;
	BYTE comp_checksum;
	GetData((char*)&bodysize, begin, idx, sizeof(unsigned short));
	GetData((char*)&flag, begin, idx, sizeof(BYTE));
	GetData((char*)&comp_checksum, begin, idx, sizeof(BYTE));

	if (comp_checksum != checksum)
	{
		return false;
	}

	return true;
}


//추후 헤더 파싱이 있어야 한다.
void session::ProcessPacket(char* packet, unsigned short bodysize, BYTE flag)
{
	SendTestPacket(packet, bodysize, flag);
}

void session::SendTestPacket(char* packet, unsigned short bodysize, BYTE flag)
{
	char sendbuff[DEF_MAX_PACKETSIZE] = { 0, };
	ZeroMemory(sendbuff, DEF_MAX_PACKETSIZE);

	int idx = 0;

	MakeHeader(sendbuff, idx,  bodysize, flag, DEF_HEADER_CHECKSUM);

	SetData(sendbuff , packet + idx , idx, bodysize);


	if (flag == DEF_FLAG_ECHO)
	{
		SendPacket(sendbuff, idx);	
	}
	else
	{
		parentserver_->BroadCast(sendbuff, idx);
	}
}

void session::SendPacket(char* sendbuff, int sendsize)
{
	boost::asio::async_write(socket_,
		boost::asio::buffer(sendbuff, sendsize),
		boost::bind(&session::handle_write, this,
		boost::asio::placeholders::error));
}

void session::MakeHeader(char* sendbuff, int& idx, unsigned short bodysize, BYTE flag, BYTE checksum)
{	
	SetData(sendbuff, (char*)&bodysize, idx, sizeof(unsigned short));
	SetData(sendbuff, (char*)&flag, idx, sizeof(BYTE));
	SetData(sendbuff, (char*)&checksum, idx, sizeof(BYTE));
}

void session::GetData(char* dest, char* source, int& idx, int size)
{
	memcpy_s(dest, size, source + idx, size);
	idx += size;
}

void session::SetData(char* dest, char* source, int& idx, int size)
{
	memcpy_s(dest + idx, size ,  source, size);
	idx += size;
}