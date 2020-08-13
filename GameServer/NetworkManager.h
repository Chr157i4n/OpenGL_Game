#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "libs/glm/glm.hpp"

#include "Logger.h"
#include <enet/enet.h>

struct Client {
	ENetPeer* peerclient;
	int clientID;
	glm::vec3 position;
	glm::vec3 rotation;
};

class NetworkManager
{
public:
	static void init();

	static void deinit();

	static void disconnect();

	static void waitForEvent();

	static void sendData(std::shared_ptr<Client> client, std::string data);

	static void broadcastData(std::string data);

	static void broadcastClientPosition();

	static void initClient(ENetEvent event);

	static void parseData(ENetPeer* peerclient, std::string data);

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

private:
	static ENetAddress address;
	static ENetHost* server;
	static std::vector<std::shared_ptr<Client>> clients;
};

