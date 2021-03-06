#pragma once

#include "stdafx.h"

class TazanServer;
class PacketSerializer;
class Entity
	: public std::enable_shared_from_this<Entity>
{
public:
	Entity(tcp::socket& socket);

public:
	virtual void OnConnected(TazanServer* tazan);
	virtual void OnDisconnect();

public:
	virtual void DoRead();
	virtual void DoWrite(PacketSerializer* ps);

private:
	TazanServer* MyTazan;
private:
	enum { MAX_READBUFFER_SIZE = 65536 };
	char EntityReadBuffer[MAX_READBUFFER_SIZE];
	std::size_t CurrentReadLength;
private:
	tcp::socket EntitySocket;
};