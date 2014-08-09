#pragma once

#include "stdafx.h"

#include "Lock.h"
#include "Entity.h"

class TazanServer
{
public:
	TazanServer(boost::asio::io_service& io_service, short port = 9000);
	~TazanServer();

private:
	void DoAccept();

private:
	std::set<std::shared_ptr<Entity>> Entities;
public:
	std::set<std::shared_ptr<Entity>> GetEntities() { Lock lock(lockSource); return Entities; }
public:
	void InsertEntity(std::shared_ptr<Entity> entity) { Lock lock(lockSource); Entities.insert(entity); }
	void EraseEntity(std::shared_ptr<Entity> entity) { Lock lock(lockSource); Entities.erase(entity); }
public:
	size_t GetEntityCount() { Lock lock(lockSource); return Entities.size(); }

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