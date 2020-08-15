#include "NetworkManager.h"

#include "ConfigManager.h"
#include "Helper.h"
#include "Game.h"

ENetAddress								NetworkManager::address;
ENetHost*								NetworkManager::server;

void NetworkManager::init()
{
	if (enet_initialize() != 0)
	{
		Logger::log("An error occurred while initializing ENet");
	}
	Logger::log("ENet initialized");

	address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = ConfigManager::server_port;

	server = enet_host_create(&address,32,2,0,0);
	if (server == NULL)
	{
		Logger::log("An error occurred while trying to create an ENet server host");
	}
	Logger::log("Server created");

	Logger::log("listening on Port: "+std::to_string(address.port));

}

void NetworkManager::deinit()
{
	disconnect();
	enet_host_destroy(server);
	enet_deinitialize();
}

void NetworkManager::disconnect()
{
	ENetEvent event;

	for (std::shared_ptr<Client> client : Game::clients)
	{
		enet_peer_disconnect(client->getPeer(), 0);
		while (enet_host_service(server, &event, 3000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				Logger::log("Disconnection succeeded.");
				return;
			}
		}
	}
}

void NetworkManager::waitForEvent()
{
    ENetEvent event;

    while (enet_host_service(server, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
		{
			Logger::log("A new client connected from " + std::to_string(event.peer->address.host) + ":" + std::to_string(event.peer->address.port));
			initClient(event);
			break;
		}
        case ENET_EVENT_TYPE_RECEIVE:
		{
			std::string message = std::string((char*)event.packet->data);
			//Logger::log("Data Received: " + message);
			parseData(event.peer, message);
			enet_packet_destroy(event.packet);

			break;
		}
        case ENET_EVENT_TYPE_DISCONNECT:
            Logger::log("disconnect");
			deinitClient(event);
        }
    }
}

void NetworkManager::sendData(std::shared_ptr<Client> client, std::string data)
{
	if (client->getPeer() == nullptr) return;
	
	ENetPacket* packet = enet_packet_create(data.c_str(), strlen(data.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(client->getPeer(), 0, packet);

	enet_host_flush(server);
}

void NetworkManager::sendData(std::shared_ptr<Client> client, NetworkCommand command, std::string data)
{
	if (client->getPeer() == nullptr) return;

	std::string message = "";
	int commandI = (int)command;
	if (commandI < 10) message += "0";
	message += std::to_string(commandI) + "|";
	message += data;

	if (command != NetworkCommand::change_position_player && command != NetworkCommand::change_rotation_player &&
		command != NetworkCommand::change_position_object && command != NetworkCommand::change_rotation_object &&
		command != NetworkCommand::change_position_bullet && command != NetworkCommand::change_rotation_bullet )
	{
		Logger::log(message);
	}

	sendData(client, message);
}

void NetworkManager::broadcastData(std::string data)
{
	for (std::shared_ptr<Client> client : Game::clients)
	{
		sendData(client, data);
	}
}

void NetworkManager::broadcastData(NetworkCommand command, std::string data)
{
	std::string message = "";
	int commandI = (int)command;
	if (commandI < 10) message += "0";
	message += std::to_string(commandI) + "|";
	message += data;

	if (command != NetworkCommand::change_position_player && command != NetworkCommand::change_rotation_player &&
		command != NetworkCommand::change_position_object && command != NetworkCommand::change_rotation_object &&
		command != NetworkCommand::change_position_bullet && command != NetworkCommand::change_rotation_bullet)
	{
		Logger::log(message);
	}

	broadcastData(message);
}

void NetworkManager::broadcastObjectPosition(std::shared_ptr<Object> object)
{
	//Logger::log("Broadcasting position of client: " + networkIDtoString(client));
	broadcastData(NetworkCommand::change_position_object, networkIDtoString(object) + "|" + Helper::glmVec3_to_string(object->getPosition()));
}

void NetworkManager::broadcastObjectRotation(std::shared_ptr<Object> object)
{
	//Logger::log("Broadcasting rotation of client: " + networkIDtoString(client));
	broadcastData(NetworkCommand::change_rotation_object, networkIDtoString(object) + "|" + Helper::glmVec3_to_string(object->getRotation()));
}

void NetworkManager::initClient(ENetEvent event)
{
	std::shared_ptr<Client> newClient = std::make_shared<Client>();
	newClient->setPeer(event.peer);
	newClient->setNetworkID(Game::clients.size());
	for (std::shared_ptr<Client> client : Game::clients)
	{
		if (client->getNetworkID() == newClient->getNetworkID())
		{
			newClient->setNetworkID(newClient->getNetworkID()+1);
		}
	}

	Game::clients.push_back(newClient);
	Logger::log("new Client got id assigned: "+std::to_string(newClient->getNetworkID()));

	std::string data = networkIDtoString(newClient) + "|0";
	sendData(newClient, NetworkCommand::change_id, data);
	
	data = "000|"+ConfigManager::level;
	sendData(newClient, NetworkCommand::change_map, data);

	for (std::shared_ptr<Client> client : Game::clients)
	{
		data = networkIDtoString(client) + "|0";
		broadcastData(NetworkCommand::player_connected, data);
	}

	for (std::shared_ptr<Client> client : Game::clients)
	{
		broadcastObjectPosition(client);
		broadcastObjectRotation(client);
	}

	for (std::shared_ptr<Object> object : Game::objects)
	{
		broadcastObjectPosition(object);
		broadcastObjectRotation(object);
	}

	if (Game::clients.size() == 1)
	{
		Game::startGame();
	}
}

void NetworkManager::deinitClient(ENetEvent event)
{
	std::shared_ptr<Client> client_ptr;

	for (std::shared_ptr<Client> client : Game::clients)
	{
		if (client->getPeer()->address.host == event.peer->address.host)
		{
			broadcastData(NetworkCommand::player_disconnected,networkIDtoString(client) + "|0");

			auto it = std::find(Game::clients.begin(), Game::clients.end(), client);
			if (it != Game::clients.end()) { Game::clients.erase(it); }
			break;
		}
	}
}

void NetworkManager::parseData(ENetPeer* peerclient, std::string data)
{
	if (data.length() <= 0) return;

	NetworkCommand command = (NetworkCommand)std::stoi(data.substr(0, 2));
	int clientId_m = std::stoi(data.substr(3, 3));
	std::string param_string = data.substr(7);

	std::shared_ptr<Client> currentClient;
	for (std::shared_ptr<Client> client : Game::clients)
	{
		if (client->getNetworkID() == clientId_m)
		{
			currentClient = client;
			break;
		}
	}
	if (currentClient == nullptr) return;

	switch (command)
	{
		case NetworkCommand::change_position_player:
		{
			currentClient->setPosition(Helper::string_to_glmVec3(param_string));
			broadcastObjectPosition(currentClient);
			break;
		}
		case NetworkCommand::change_rotation_player:
		{
			currentClient->setRotation(Helper::string_to_glmVec3(param_string));
			broadcastObjectRotation(currentClient);
			break;
		}
		case NetworkCommand::change_lookdirection_player:
		{
			currentClient->setLookDirection(Helper::string_to_glmVec3(param_string));
			break;
		}
		case NetworkCommand::change_map:
		{
			Helper::eraseSubStr(param_string, "^");
			broadcastData(NetworkCommand::change_map, "000|"+param_string);
			break;
		}
		case NetworkCommand::shoot:
		{
			
			if (currentClient->shoot() != nullptr)
			{
				Logger::log("client: " + networkIDtoString(currentClient) + "|0");
				broadcastData(NetworkCommand::shoot, networkIDtoString(Game::bullets[Game::bullets.size() - 1]) + "|0");
			}
			break;
		}

	}
}