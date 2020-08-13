#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "libs/glm/glm.hpp"

#include "Logger.h"
#include <enet/enet.h>

enum NetworkCommand {
	change_position,
	change_rotation,
	change_id,
	change_map,
	player_connected,
	player_disconnected
};

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

	static void sendData(std::shared_ptr<Client> client, NetworkCommand command, std::string data);

	static void broadcastData(std::string data);

	static void broadcastData(NetworkCommand command, std::string data);

	static void broadcastClientPosition(std::shared_ptr<Client> client);

	static void broadcastClientRotation(std::shared_ptr<Client> client);

	static void initClient(ENetEvent event);

	static void deinitClient(ENetEvent event);

	static void parseData(ENetPeer* peerclient, std::string data);

	static std::string getClientID(std::shared_ptr<Client> client)
	{
		std::string answer = "";

		if (client->clientID < 10) answer += "0";

		answer += std::to_string(client->clientID);
		return answer;
	}

private:
	static std::string glmVec3_to_string(glm::vec3 vector);

	static glm::vec3 string_to_glmVec3(std::string);

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

	static ENetAddress address;
	static ENetHost* server;
	static std::vector<std::shared_ptr<Client>> clients;
};

