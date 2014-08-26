#pragma once

#include "stdafx.h"

class TazanServer;
class PacketSerializer;
class Client
	: public std::enable_shared_from_this<Client>
{
public:
	Client(tcp::socket& socket);

public:
	void OnConnected(TazanServer* tazan);
	void OnDisconnect();

public:
	void DoRead();
	void DoWrite(PacketSerializer* ps);

protected:
	TazanServer* MyTazan;
protected:
	enum { MAX_READBUFFER_SIZE = 65536 };
	char ClientReadBuffer[MAX_READBUFFER_SIZE];
	std::size_t CurrentReadLength;
protected:
	tcp::socket ClientSocket;
};