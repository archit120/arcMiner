#ifndef _H_STRATUM_PROTOCOL
#define _H_STRATUM_PROTOCOL

#include "global.h"

class StratumProtocol
{
public:
	//Login blocks
	static bool Login();
	static bool Login(MinerClient& client);

	static void StratumThread(WorkBlob& work);
	static bool GetWork(MinerClient& client, WorkBlob& work);
};

#endif