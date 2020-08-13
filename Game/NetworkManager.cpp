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
		while (enet_host_service(client, &event, 1000) > 0)
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

void NetworkManager::sendData(NetworkCommand command, std::string data)
{
	if (!isConnected) return;

	std::string message = "";
	int commandI = (int)command;
	if (commandI < 10) message += "0";
	message += std::to_string(commandI) + "|";
	message += NetworkManager::getClientID() + "|" + data;

	ENetPacket* packet = enet_packet_create(message.c_str(), strlen(message.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(peer, 0, packet);

	enet_host_flush(client);
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

void NetworkManager::sendPlayerPosition()
{
	NetworkManager::sendData(NetworkCommand::change_position, NetworkManager::glmVec3_to_string(Game::players[0]->getPosition()));
}

void NetworkManager::sendPlayerRotation()
{
	NetworkManager::sendData(NetworkCommand::change_rotation, NetworkManager::glmVec3_to_string(Game::players[0]->getRotation()));
}

void NetworkManager::parseData(std::string data)
{
	if (data.length() <= 0) return;

	NetworkCommand command = (NetworkCommand)std::stoi(data.substr(0, 2));
	int clientId_m = std::stoi(data.substr(3,2));
	std::string param_string = data.substr(6);
	

	switch (command)
	{
	case NetworkCommand::change_id:
		{
			Logger::log("this client has id: "+std::to_string(clientId_m));
			NetworkManager::clientID = clientId_m;
			break;
		}
	case NetworkCommand::change_map:
		{
			Logger::log("Server changes map: "+param_string);
			Map::load(param_string);
			Game::startGame();
			sendPlayerPosition();
			sendPlayerRotation();
			break;
		}
	case NetworkCommand::player_connected:
		{
			if (clientId_m == NetworkManager::clientID) break;
			Logger::log("new client connected to server, id: " + std::to_string(clientId_m));
			std::shared_ptr<Character> newCharacter = std::make_shared<Character>(Renderer::getShader(ShaderType::basic));
			newCharacter->setClientID(clientId_m);
			Game::clients.push_back(newCharacter);
			Game::objects.push_back(newCharacter);
			//Renderer::initFrameBuffer();
			break;
		}
	case NetworkCommand::player_disconnected:
		{
			if (clientId_m == NetworkManager::clientID) break;
			Logger::log("client diconnected from server: " + std::to_string(clientId_m));
			for (std::shared_ptr<Character> client : Game::clients)
			{
				if (client->getClientID() == clientId_m)
				{
					auto it = std::find(Game::clients.begin(), Game::clients.end(), client);
					if (it != Game::clients.end()) { Game::clients.erase(it); }

					auto it1 = std::find(Game::objects.begin(), Game::objects.end(), client);
					if (it1 != Game::objects.end()) { Game::objects.erase(it1); }
					break;
				}
			}
			break;
		}
	case NetworkCommand::change_position:
		{
			if (clientId_m == NetworkManager::clientID) break;
			//Logger::log("update Client position, id: " + std::to_string(clientId_m));
			for (std::shared_ptr<Character> client : Game::clients)
			{
				if (client->getClientID() == clientId_m)
				{
					std::vector<std::string> params;
					Helper::split(param_string, params, ';');
					client->setPosition(glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2])));
					break;
				}
			}
			break;
		}
	case NetworkCommand::change_rotation:
		{
			if (clientId_m == NetworkManager::clientID) break;
			//Logger::log("update Client rotation, id: " + std::to_string(clientId_m));
			for (std::shared_ptr<Character> client : Game::clients)
			{
				if (client->getClientID() == clientId_m)
				{
					std::vector<std::string> params;
					Helper::split(param_string, params, ';');
					client->setRotation(glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2])));
					break;
				}
			}
			break;
		}
	}
}


std::string NetworkManager::glmVec3_to_string(glm::vec3 vector)
{
	std::string data = "";
	data += std::to_string(vector.x) + ";";
	data += std::to_string(vector.y) + ";";
	data += std::to_string(vector.z);

	return data;
}

glm::vec3 NetworkManager::string_to_glmVec3(std::string string)
{
	std::vector<std::string> params;
	Helper::split(string, params, ';');
	return glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2]));
}
