#ifndef _H_STRATUM_PROTOCOL
#define _H_STRATUM_PROTOCOL

#include "global.h"

class StratumProtocol
{
public:
	//Login blocks
	static bool Login();
	static bool Login(MinerClient& client);

	static bool HandleMethod(string s);
	static bool HandleMethod(MinerClient& client, string s);

	static void StratumThreadStarter();
	static void StratumThreadStarter(MinerClient& client);
	static void StratumThread(MinerClient &lclient);

	static bool AddShares(StratumShare &share);
	static bool AddShares(StratumShare &share, MinerClient &client);
	static bool HandleShares();
	static bool HandleShares(MinerClient& client);

	static bool GetWork(WorkBlob& work);
	static bool GetWork(MinerClient& client, WorkBlob& work);
};

#endif