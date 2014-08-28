#include "stdafx.h"

#include "TazanServer.h"

#include "Packet.h"

int main(int argc, char* argv[])
{
	InitPacket();

	try
	{
		boost::asio::io_service io_service;

		TazanServer g_server(io_service);

#pragma omp parallel
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}