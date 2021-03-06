#ifndef __DEF_SESSION__
#define __DEF_SESSION__

#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>
#include "common/Define.h"

using boost::asio::ip::tcp;

class server;

class session
{
public:
	
	session(boost::asio::io_service& io_service, server* parent);
	~session();

	tcp::socket& socket();

	void start();
	void Shudown();
	int GetSocketID();
	void SendPacket(char* sendbuff, int sendsize);

private:
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error);	

	void ParsePacket();
	void GetData(char* dest, char* source, int& idx, int size);
	void SetData(char* dest, char* source, int& idx, int size);
	
	void ProcessPacket(char* packet, unsigned short bodysize, BYTE flag);
	bool CheckHeader(char* begin, unsigned short& bodysize, BYTE& flag, BYTE checksum);
	void SendTestPacket(char* packet, unsigned short bodysize, BYTE flag);
	void MakeHeader(char* sendbuff, int& idx, unsigned short bodysize, BYTE flag, BYTE checksum);

	
	

	server* parentserver_;

	boost::asio::io_service::strand strand_;
	tcp::socket socket_;
	
	char data_[DEF_MAX_PACKETSIZE];
	boost::circular_buffer<BYTE>* cir_buffer_;

	//std::array<char, DEF_MAX_PACKETSIZE * 2> buffer_;

};

#endif