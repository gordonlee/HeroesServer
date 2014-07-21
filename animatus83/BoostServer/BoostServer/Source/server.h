#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"

class server
{
public:
	server(boost::asio::io_service& io_service, short port);

	void AddSession(session* addsession);
	void DelSession(session* delsession);
	void BroadCast(char* sendbuff, int sendsize);

private:
	void start_accept();	

	void handle_accept(session* new_session, const boost::system::error_code& error);	
	
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;

	
	std::map< int, session*  > sessions_;
};

static server* g_server = NULL;
static server* GetServerInstance(boost::asio::io_service& io_service, short port)
{
	if (g_server == NULL)
	{
		g_server = new server(io_service, port);
	}
	return g_server;
}