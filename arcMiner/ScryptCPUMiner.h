#ifndef _H_SCRYPT_CPUMINER
#define _H_SCRYPT_CPUMINER

#include "global.h"
class ScryptCPUMiner
{
public:
	//static bool BlockMine(MinerClient &client, WorkBlob &work);
	static bool BlockMine(MinerClient &client);
};

#endif