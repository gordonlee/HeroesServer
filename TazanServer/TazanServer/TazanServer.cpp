#include "stdafx.h"

#include "TazanServer.h"
#include "Client.h"

TazanServer::TazanServer(boost::asio::io_service& io_service, short port)
: Acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
AcceptedSocket(io_service)
{
	InitLockSource(lockSource);

	option_nodelay = boost::asio::ip::tcp::no_delay(true);

	Acceptor.set_option(option_nodelay);

	DoAccept();
}

TazanServer::~TazanServer()
{
}

void TazanServer::DoAccept()
{
	Acceptor.async_accept(AcceptedSocket,
		[this](boost::system::error_code ec)
	{
		if (!ec)
		{
			AcceptedSocket.set_option(option_nodelay);
			
			std::make_shared<Client>(std::move(AcceptedSocket))->OnConnected((TazanServer*)this);
			printf("Connected, Count : %d\n", GetClientCount());
		}

		DoAccept();
	});
}