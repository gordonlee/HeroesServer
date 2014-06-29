  // Copyright 2014 GordonLee

#include "network/network.h"
int main() {
    Network network;
    network.Initialize();

    while ( network.RunAcceptThread() ) {
    }

    network.Cleanup();
    return 0;
}
