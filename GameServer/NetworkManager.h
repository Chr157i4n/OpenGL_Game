#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "libs/glm/glm.hpp"
#include <enet/enet.h>

#include "Logger.h"
#include "Client.h"

enum NetworkCommand {
	change_position_player,
	change_rotation_player,
	change_lookdirection_player,
	change_position_object,
	change_rotation_object,
	change_position_bullet,
	change_rotation_bullet,
	change_id,
	change_map,
	player_connected,
	player_disconnected,
	shoot,
	disable_object,
	hit,
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

	static void broadcastObjectPosition(std::shared_ptr<Object> object);

	static void broadcastObjectRotation(std::shared_ptr<Object> object);

	static void initClient(ENetEvent event);

	static void deinitClient(ENetEvent event);

	static void parseData(ENetPeer* peerclient, std::string data);

	static std::string networkIDtoString(std::shared_ptr<Object> object)
	{
		std::string answer = "";

		if (object->getNetworkID() < 100) answer += "0";
		if (object->getNetworkID()< 10) answer += "0";

		answer += std::to_string(object->getNetworkID());
		return answer;
	}

private:

	static ENetAddress address;
	static ENetHost* server;
};

