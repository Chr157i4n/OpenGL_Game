#include "NetworkManager.h"


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
	address.port = 1234;

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
			Logger::log("A new client connected from" + std::to_string(event.peer->address.host) + ":" + std::to_string(event.peer->address.port));

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
			for (std::shared_ptr<Client> client : clients)
			{
				if (client->peerclient->address.host == event.peer->address.host)
				{
					auto it = std::find(clients.begin(), clients.end(), client);
					if (it != clients.end()) { clients.erase(it); }
					break;
				}
			}
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

void NetworkManager::broadcastData(std::string data)
{
	for (std::shared_ptr<Client> client : clients)
	{
		sendData(client, data);
	}
}

void NetworkManager::broadcastClientPosition()
{
	for (std::shared_ptr<Client> client : clients)
	{
		std::string data;

		data = "p|";
		if (client->clientID < 10) data += "0";
		data += std::to_string(client->clientID) + "|";
		data += std::to_string(client->position.x) + ";";
		data += std::to_string(client->position.y) + ";";
		data += std::to_string(client->position.z);

		Logger::log(data);

		broadcastData(data);

		data = "r|";
		if (client->clientID < 10) data += "0";
		data += std::to_string(client->clientID) + "|";
		data += std::to_string(client->rotation.x) + ";";
		data += std::to_string(client->rotation.y) + ";";
		data += std::to_string(client->rotation.z);

		Logger::log(data);

		broadcastData(data);
	}
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

	std::string message = "i|";
	if (newClient->clientID < 10) message += "0";
	message += std::to_string(newClient->clientID) + "|0";
	sendData(newClient, message);
	sendData(newClient, "m|00|level_farm");

	for (std::shared_ptr<Client> client : clients)
	{
		message = "n|";
		if (client->clientID < 10) message += "0";
		message += std::to_string(client->clientID) + "|0";
		broadcastData(message);
	}

}

void NetworkManager::parseData(ENetPeer* peerclient, std::string data)
{
	if (data.length() <= 0) return;

	char command = data[0];
	int clientId_m = std::stoi(data.substr(2, 2));
	std::string param_string = data.substr(5);

	std::shared_ptr<Client> currentClient;
	for (std::shared_ptr<Client> client : clients)
	{
		if (client->clientID == clientId_m)
		{
			currentClient = client;
			break;
		}
	}

	switch (command)
	{
		case 'p':
		{
			std::vector<std::string> params;
			split(param_string, params, ';');
			currentClient->position = glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2]));
			break;
		}
		case 'r':
		{
			std::vector<std::string> params;
			split(param_string, params, ';');
			currentClient->rotation = glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2]));
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