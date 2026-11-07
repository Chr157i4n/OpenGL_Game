#pragma once

#include <glm/glm.hpp>
#include <enet/enet.h>

#include <string>
#include <vector>

#include "Helper.h"
#include "Logger.h"

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

	static void connect();

	static void disconnect();

	static void sendData(NetworkCommand command, std::string data = "00");

	static void sendData(std::string data);

	static void readData(int timeout_ms = 0);

	static void sendPlayerPosition();

	static void sendPlayerRotation();

	static void sendShoot();


	static bool getIsConnected()
	{
		return isConnected;
	}

	static void parseData(std::string data);

	static std::string getClientID()
	{
		std::string answer="";

		if (clientID < 100) answer += "0";
		if (clientID < 10) answer += "0";

		answer += std::to_string(clientID);
		return answer;
	}



private:
	static std::string glmVec3_to_string(glm::vec3 vector);

	static glm::vec3 string_to_glmVec3(std::string string);


	static ENetAddress address;
	static ENetHost* client;
	static ENetPeer* peer;

	static bool isConnected;
	static int clientID;
};

