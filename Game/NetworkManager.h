#pragma once

#include <enet/enet.h>
#include <string>
#include <vector>

class NetworkManager
{
public:
	static void init();

	static void deinit();

	static void connect();

	static void disconnect();

	static void sendData(std::string data);

	static void readData(int timeout_ms = 0);

	static bool getIsConnected()
	{
		return isConnected;
	}

	static void parseData(std::string data);

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

	static std::string getClientID()
	{
		std::string answer="";

		if (clientID < 10) answer += "0";

		answer += std::to_string(clientID);
		return answer;
	}

private:
	static ENetAddress address;
	static ENetHost* client;
	static ENetPeer* peer;

	static bool isConnected;
	static int clientID;
};

