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

};

#endif