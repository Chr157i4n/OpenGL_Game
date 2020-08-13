#include "NetworkManager.h"

#include "Game.h"

ENetAddress NetworkManager::address;
ENetHost*	NetworkManager::client;
ENetPeer*	NetworkManager::peer;

bool		NetworkManager::isConnected = false;
int			NetworkManager::clientID;

void NetworkManager::init()
{
	if (enet_initialize() != 0)
	{
		Logger::log("[ENet] An error occurred while initializing ENet");
	}


	client = enet_host_create(NULL, 1, 1, 0, 0);
	if (client == NULL)
	{
		Logger::log("[ENet] An error occurred while trying to create an ENet server host");
	}


}

void NetworkManager::deinit()
{
	NetworkManager::disconnect();
	enet_host_destroy(client);
	enet_deinitialize();
}

void NetworkManager::connect()
{
	ENetEvent event;
	/* Connect to some.server.net:1234. */
	enet_address_set_host(&address, ConfigManager::server_address.c_str());
	address.port = ConfigManager::server_port;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	peer = enet_host_connect(client, &address, 2, 0);
	if (peer == NULL)
	{
		Logger::log("[ENet] No available peers for initiating an ENet connection");
		return;
	}
	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		Logger::log("[ENet] Connection to Server succeeded.");
		isConnected = true;
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(peer);
		Logger::log("[ENet] Connection to Server failed.");
	}
}

void NetworkManager::disconnect()
{
	if (isConnected)
	{
		ENetEvent event;

		enet_peer_disconnect(peer, 0);
		while (enet_host_service(client, &event, 3000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				Logger::log("Disconnection succeeded.");
				isConnected = false;
				return;
			}
		}
	}
}

void NetworkManager::sendData(std::string data)
{
	if (!isConnected) return;

	ENetPacket* packet = enet_packet_create(data.c_str(), strlen(data.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(peer, 0, packet);

	enet_host_flush(client);
}

void NetworkManager::readData(int timeout_ms)
{
	if (isConnected)
	{
		ENetEvent event;

		while (enet_host_service(client, &event, timeout_ms) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				Logger::log("A new client connected from" + std::to_string(event.peer->address.host) + ":" + std::to_string(event.peer->address.port));
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				std::string data = std::string((char*)event.packet->data);
				//Logger::log("Data Received: " + data);
				parseData(data);
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				Logger::log("disconnect");
				break;
			}
			}
		}
	}
}

void NetworkManager::parseData(std::string data)
{
	if (data.length() <= 0) return;

	char command = data[0];
	int clientId_m = std::stoi(data.substr(2,2));
	std::string param_string = data.substr(5);
	

	switch (command)
	{
		case 'i':
		{
			Logger::log("this client has id: "+std::to_string(clientId_m));
			NetworkManager::clientID = clientId_m;
			break;
		}
		case 'm':
		{
			Logger::log("Server send current Map: "+param_string);
			Map::load(param_string);
			Game::startGame();
			break;
		}
		case 'n':
		{
			if (clientId_m == NetworkManager::clientID) break;
			Logger::log("new client connected to server, id: " + std::to_string(clientId_m));
			std::shared_ptr<Character> newCharacter = std::make_shared<Character>(Renderer::getShader(ShaderType::basic));
			newCharacter->setClientID(clientId_m);
			Game::clients.push_back(newCharacter);
			Game::objects.push_back(newCharacter);
			Renderer::initFrameBuffer();
			break;
		}
		case 'p':
		{
			if (clientId_m == NetworkManager::clientID) break;
			//Logger::log("update Client position, id: " + std::to_string(clientId_m));
			for (std::shared_ptr<Character> client : Game::clients)
			{
				if (client->getClientID() == clientId_m)
				{
					std::vector<std::string> params;
					split(param_string, params, ';');
					client->setPosition(glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2])));
					break;
				}
			}
			break;
		}
		case 'r':
		{
			if (clientId_m == NetworkManager::clientID) break;
			//Logger::log("update Client rotation, id: " + std::to_string(clientId_m));
			for (std::shared_ptr<Character> client : Game::clients)
			{
				if (client->getClientID() == clientId_m)
				{
					std::vector<std::string> params;
					split(param_string, params, ';');
					client->setRotation(glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2])));
					break;
				}
			}
			break;
		}
	}
}

size_t NetworkManager::split(const std::string& txt, std::vector<std::string>& strs, char ch)
{
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, (std::min)(pos, txt.size()) - initialPos + 1));

	return strs.size();
}
