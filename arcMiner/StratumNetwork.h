#ifndef _H_STRATUM_NETWORK
#define _H_STRATUM_NETWORK

#include "global.h"

class StratumNetwork
{
public:
	static bool Connect();
	static bool Connect(MinerClient& client);

	static bool Send(string s);
	static bool Send(MinerClient client, string s);

	static bool Receive(string& s);
	static bool Receive(MinerClient& client, string& s);

	static bool BlockReceive(string& s);
	static bool BlockReceive(MinerClient& client, string& s);

};

const int MaxPacketSizeStratum = 1024*2*2;

#endif