#include "stdafx.h"

#include "TazanServer.h"

#include "Packet.h"

int main(int argc, char* argv[])
{
	HWND h = FindWindow(NULL, L"Borderlands 2 (32-bit, DX9)");
	while (true)
	{
		SendMessage(h, WM_KEYDOWN, VK_RETURN, 0);
		SendMessage(h, WM_KEYUP, VK_RETURN, 0);
	}

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