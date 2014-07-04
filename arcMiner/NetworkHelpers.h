#ifndef _H_NETWORK_HELPERS
#define _H_NETWORK_HELPERS

#include "global.h"

class NetworkHelpers
{
public:
	static bool OpenConnection();
	static bool OpenConnection(SOCKET& s, RequestTarget target);

	static bool Connect();
	static bool Connect(MinerClient& client);

	static bool Send(string s);
	static bool Send(MinerClient client, string s);

};

#endif