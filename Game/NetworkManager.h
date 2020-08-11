#pragma once

#include <enet/enet.h>

class NetworkManager
{
public:
	static void init();

	static void deinit();

private:
	static ENetAddress address;
	static ENetHost* server;
};

