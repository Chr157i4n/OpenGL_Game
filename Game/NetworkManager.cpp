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
	Logger::log("Connecting to: "+ ConfigManager::server_address +":"+std::to_string(ConfigManager::server_port));
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
	NetworkManager::sendData(NetworkCommand::change_position_player, NetworkManager::glmVec3_to_string(Game::players[0]->getPosition()));
}

void NetworkManager::sendPlayerRotation()
{
	NetworkManager::sendData(NetworkCommand::change_rotation_player, NetworkManager::glmVec3_to_string(Game::players[0]->getRotation()));
	NetworkManager::sendData(NetworkCommand::change_lookdirection_player, NetworkManager::glmVec3_to_string(Game::players[0]->getLookDirection()));
}

void NetworkManager::sendShoot()
{
	NetworkManager::sendData(NetworkCommand::shoot);
}

void NetworkManager::parseData(std::string data)
{
	if (data.length() <= 0) return;

	NetworkCommand command;
	int networkID_m;
	std::string param_string;

	//Logger::log("NetworkData: " + data);

	try {
		command = (NetworkCommand)std::stoi(data.substr(0, 2));
		networkID_m = std::stoi(data.substr(3, 3));
		param_string = data.substr(7);
	}
	catch (std::exception)
	{
		Logger::log("cant parse NetworkData: "+ data);
		
		std::stringstream errMsg;
		errMsg << "cant parse NetworkData: " << data;
		throw std::exception(errMsg.str().c_str());
	}
	

	switch (command)
	{
	case NetworkCommand::change_id:
		{
			Logger::log("this client has id: "+std::to_string(networkID_m));
			NetworkManager::clientID = networkID_m;
			break;
		}
	case NetworkCommand::change_map:
		{
			Logger::log("Server changes map: "+param_string);
			Map::load(param_string);
			Game::startGame(Game_Mode::GameMode_MultiPlayer);
			sendPlayerPosition();
			sendPlayerRotation();
			break;
		}
	case NetworkCommand::player_connected:
		{
			if (networkID_m == NetworkManager::clientID) break;
			Logger::log("new client connected to server, id: " + std::to_string(networkID_m));
			std::shared_ptr<Character> newCharacter = std::make_shared<Character>(Renderer::getShader(ShaderType::basic));
			newCharacter->setNetworkID(networkID_m);
			Game::clients.push_back(newCharacter);
			Game::objects.push_back(newCharacter);
			//Renderer::initFrameBuffer();
			break;
		}
	case NetworkCommand::player_disconnected:
		{
			if (networkID_m == NetworkManager::clientID) break;
			Logger::log("client diconnected from server: " + std::to_string(networkID_m));
			for (std::shared_ptr<Character> client : Game::clients)
			{
				if (client->getNetworkID() == networkID_m)
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
	case NetworkCommand::change_position_player:
		{
			if (networkID_m == NetworkManager::clientID) break;
			//Logger::log("update Client position, id: " + std::to_string(clientId_m));
			for (std::shared_ptr<Character> client : Game::clients)
			{
				if (client->getNetworkID() == networkID_m)
				{
					std::vector<std::string> params;
					Helper::split(param_string, params, ';');
					client->setPosition(glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2])));
					break;
				}
			}
			break;
		}
	case NetworkCommand::change_rotation_player:
		{
			if (networkID_m == NetworkManager::clientID) break;
			//Logger::log("update Client rotation, id: " + std::to_string(clientId_m));
			for (std::shared_ptr<Character> client : Game::clients)
			{
				if (client->getNetworkID() == networkID_m)
				{
					std::vector<std::string> params;
					Helper::split(param_string, params, ';');
					client->setRotation(glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2])));
					break;
				}
			}
			break;
		}
	case NetworkCommand::change_position_object:
	{
		if (networkID_m == NetworkManager::clientID) break;
		//Logger::log("update Client position, id: " + std::to_string(clientId_m));
		for (std::shared_ptr<Object> object : Game::objects)
		{
			if (object->getNetworkID() == networkID_m)
			{
				std::vector<std::string> params;
				Helper::split(param_string, params, ';');
				object->setPosition(glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2])));

				//Logger::log("new position for: " + object->printObject() + ": " + glmVec3_to_string(object->getPosition()));
				break;
			}
		}
		break;
	}
	case NetworkCommand::change_rotation_object:
	{
		if (networkID_m == NetworkManager::clientID) break;
		//Logger::log("update Client rotation, id: " + std::to_string(clientId_m));
		for (std::shared_ptr<Object> object : Game::objects)
		{
			if (object->getNetworkID() == networkID_m)
			{
				std::vector<std::string> params;
				Helper::split(param_string, params, ';');
				object->setRotation(glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2])));
				break;
			}
		}
		break;
	}
	case NetworkCommand::shoot:
		{
			Logger::log("someone shot");
			std::shared_ptr<Bullet> newBullet = std::make_shared<Bullet>(Renderer::getShader(ShaderType::basic));
			newBullet->setNetworkID(networkID_m);
			Game::bullets.push_back(newBullet);
			Game::objects.push_back(newBullet);
			break;
		}
	case NetworkCommand::disable_object:
		{
		for (std::shared_ptr<Object> object : Game::objects)
		{
			if (object->getNetworkID() == networkID_m)
			{
				object->disable();
			}
		}
		break;
	}
	case NetworkCommand::hit:
	{
		if (networkID_m != NetworkManager::clientID) break;
		Logger::log("you got hit");
		Game::players[0]->registerHit();
		AudioManager::play3D("audio/hit.wav", Game::players[0]->getPosition());
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
