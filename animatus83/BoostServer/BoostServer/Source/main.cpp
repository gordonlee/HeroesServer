
#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "session.h"
#include "server.h"

#define DEF_MAX_THREAD_COUNT 8

using boost::asio::ip::tcp;

boost::asio::io_service io_service;



void run()
{
	io_service.run();
}


int main(int argc, char* argv[])
{
  try
  {	  
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }
    

    using namespace std; 	
    server* s = GetServerInstance(io_service, atoi(argv[1]));

	boost::thread_group group;
	boost::thread* t[DEF_MAX_THREAD_COUNT - 1];

	for (int i = 0; i < DEF_MAX_THREAD_COUNT; i++)
	{
		t[i] = new boost::thread(run);
		group.add_thread(t[i]);
	}
	group.join_all();

  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}