#pragma once
#include "Character.h"

#include <enet/enet.h>

class Client : public Character
{
public:

	Client();

	void setPeer(ENetPeer* peer)
	{
		peerclient = peer;
	}

	ENetPeer* getPeer()
	{
		return peerclient;
	}

	virtual void registerHit();

private:

	ENetPeer* peerclient = nullptr;

};

