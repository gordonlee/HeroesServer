#pragma once

#include "stdafx.h"

#include "Lock.h"
#include "Client.h"

class TazanServer
{
public:
	TazanServer(boost::asio::io_service& io_service, short port = 9000);
	~TazanServer();

private:
	void DoAccept();

private:
	std::set<std::shared_ptr<Client>> Entities;
public:
	std::set<std::shared_ptr<Client>> GetEntities() { Lock lock(lockSource); return Entities; }
public:
	void InsertClient(std::shared_ptr<Client> Client) { Lock lock(lockSource); Entities.insert(Client); }
	void EraseClient(std::shared_ptr<Client> Client) { Lock lock(lockSource); Entities.erase(Client); }
public:
	size_t GetClientCount() { Lock lock(lockSource); return Entities.size(); }

private:
	LockSource lockSource;
public:
	LockSource& GetLockSource() { return lockSource; }

private:
	boost::asio::ip::tcp::no_delay option_nodelay;
private:
	tcp::acceptor Acceptor;
	tcp::socket AcceptedSocket;
};