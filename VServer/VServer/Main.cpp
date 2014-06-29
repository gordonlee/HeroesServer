#include <iostream>
#include "../VCore/VNetwork.h"

#pragma comment(lib, "VCore.lib")

int main()
{
	VCore::VNetwork net(9011);
	net.Initialize();
	net.Run();
}
