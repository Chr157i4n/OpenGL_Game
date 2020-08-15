#include "Client.h"

#include "Logger.h"
#include "Helper.h"
#include "NetworkManager.h"

Client::Client() : Character()
{
	this->setType(ObjectType::Object_Player);
}

void Client::registerHit()
{
	addToHealth(-20);
	lastHitTimestamp = std::chrono::system_clock::now();

	NetworkManager::broadcastData(NetworkCommand::hit, this->getNetworkIDString() + "|0");
}