#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include "server.h"

boost::mutex sessionmutex_;

server::server(boost::asio::io_service& io_service, short port)
	: io_service_(io_service),
	acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{
	start_accept();
}


void server::start_accept()
{
	session* new_session = new session(io_service_, this);
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&server::handle_accept, this, new_session,
		boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session, const boost::system::error_code& error)
{
	if (!error)
	{
		new_session->start();
	}
	else
	{
		delete new_session;
	}

	start_accept();
}


void server::BroadCast(char* sendbuff,int sendsize)
{	
	//target->SendPacket(sendbuff, sendsize);	
}

void server::AddSession(session* addsession)
{	
	boost::lock_guard<boost::mutex> lock(sessionmutex_);
	sessions_.insert(std::pair<int, session*>(addsession->GetSocketID(), addsession));
	//std::cout << addsession->socket().native().remote_endpoint().address().to_string() << " [ " << addsession->socket().native().remote_endpoint().port() << " ] "<< "\t: Connect" << std::endl;

	
}

void server::DelSession(session* delsession)
{
	boost::lock_guard<boost::mutex> lock(sessionmutex_);
	if (sessions_.find(delsession->GetSocketID()) != sessions_.end())
	{
		sessions_.erase(delsession->GetSocketID());
		//std::cout << delsession->socket().native().remote_endpoint().address().to_string() << " [ " << delsession->socket().native().remote_endpoint().port() << " ] " << "\t: Disconnect" << std::endl;

		delsession->Shudown();
	}	
}

