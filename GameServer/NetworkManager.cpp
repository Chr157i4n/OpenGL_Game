#include "NetworkManager.h"

#include "ConfigManager.h"
#include "Helper.h"

ENetAddress								NetworkManager::address;
ENetHost*								NetworkManager::server;
std::vector<std::shared_ptr<Client>>	NetworkManager::clients;

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

	for (std::shared_ptr<Client> client : clients)
	{
		enet_peer_disconnect(client->peerclient, 0);
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
	if (client->peerclient == nullptr) return;
	
	ENetPacket* packet = enet_packet_create(data.c_str(), strlen(data.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(client->peerclient, 0, packet);

	enet_host_flush(server);
}

void NetworkManager::sendData(std::shared_ptr<Client> client, NetworkCommand command, std::string data)
{
	if (client->peerclient == nullptr) return;

	std::string message = "";
	int commandI = (int)command;
	if (commandI < 10) message += "0";
	message += std::to_string(commandI) + "|";
	message += data;

	if (command != NetworkCommand::change_position && command != NetworkCommand::change_rotation)
	{
		Logger::log(message);
	}

	sendData(client, message);
}

void NetworkManager::broadcastData(std::string data)
{
	for (std::shared_ptr<Client> client : clients)
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

	if (command != NetworkCommand::change_position && command != NetworkCommand::change_rotation)
	{
		Logger::log(message);
	}

	broadcastData(message);
}

void NetworkManager::broadcastClientPosition(std::shared_ptr<Client> client)
{
	//Logger::log("Broadcasting position of client: " + getClientID(client));
	broadcastData(NetworkCommand::change_position, getClientID(client) + "|" + glmVec3_to_string(client->position));
}

void NetworkManager::broadcastClientRotation(std::shared_ptr<Client> client)
{
	//Logger::log("Broadcasting rotation of client: " + getClientID(client));
	broadcastData(NetworkCommand::change_rotation, getClientID(client) + "|" + glmVec3_to_string(client->rotation));
}

void NetworkManager::initClient(ENetEvent event)
{
	std::shared_ptr<Client> newClient = std::make_shared<Client>();
	newClient->peerclient = event.peer;
	newClient->clientID = clients.size();
	for (std::shared_ptr<Client> client : clients)
	{
		if (client->clientID == newClient->clientID)
		{
			newClient->clientID++;
		}
	}

	clients.push_back(newClient);
	Logger::log("new Client got id assigned: "+std::to_string(newClient->clientID));

	std::string data = getClientID(newClient) + "|0";
	sendData(newClient, NetworkCommand::change_id, data);
	
	data = "00|"+ConfigManager::level;
	sendData(newClient, NetworkCommand::change_map, data);

	for (std::shared_ptr<Client> client : clients)
	{
		data = getClientID(client) + "|0";
		broadcastData(NetworkCommand::player_connected, data);
	}

	for (std::shared_ptr<Client> client : clients)
	{
		broadcastClientPosition(client);
		broadcastClientRotation(client);
	}
}

void NetworkManager::deinitClient(ENetEvent event)
{
	std::shared_ptr<Client> client_ptr;

	for (std::shared_ptr<Client> client : clients)
	{
		if (client->peerclient->address.host == event.peer->address.host)
		{
			broadcastData(NetworkCommand::player_disconnected,getClientID(client) + "|0");

			auto it = std::find(clients.begin(), clients.end(), client);
			if (it != clients.end()) { clients.erase(it); }
			break;
		}
	}
}

void NetworkManager::parseData(ENetPeer* peerclient, std::string data)
{
	if (data.length() <= 0) return;

	NetworkCommand command = (NetworkCommand)std::stoi(data.substr(0, 2));
	int clientId_m = std::stoi(data.substr(3, 2));
	std::string param_string = data.substr(6);

	std::shared_ptr<Client> currentClient;
	for (std::shared_ptr<Client> client : clients)
	{
		if (client->clientID == clientId_m)
		{
			currentClient = client;
			break;
		}
	}
	if (currentClient == nullptr) return;

	switch (command)
	{
		case NetworkCommand::change_position:
		{
			currentClient->position = NetworkManager::string_to_glmVec3(param_string);
			broadcastClientPosition(currentClient);
			break;
		}
		case NetworkCommand::change_rotation:
		{
			currentClient->rotation = NetworkManager::string_to_glmVec3(param_string);
			broadcastClientRotation(currentClient);
			break;
		}
		case NetworkCommand::change_map:
		{
			Helper::eraseSubStr(param_string, "^");
			broadcastData(NetworkCommand::change_map, "00|"+param_string);
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
	split(string, params, ';');
	return glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2]));
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