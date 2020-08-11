#include "NetworkManager.h"

#include "Game.h"

ENetAddress NetworkManager::address;
ENetHost*	NetworkManager::server;

void NetworkManager::init()
{
	if (enet_initialize() != 0)
	{
		Logger::log("An error occurred while initializing ENet");
	}


	address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = 1234;

	server = enet_host_create(&address /* the address to bind the server host to */,
		32      /* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (server == NULL)
	{
		Logger::log("An error occurred while trying to create an ENet server host");
	}


}

void NetworkManager::deinit()
{
	enet_host_destroy(server);
	enet_deinitialize();
}
