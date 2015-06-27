#ifndef _H_STRATUM_HELPERS
#define _H_STRATUM_HELPERS

#include "global.h"

class StratumHelpers
{
public:
	static Target TargetFromDifficulty(double difficulty);
	static Target TargetFromDifficulty(double difficulty, Algorithms algorithm);
	
	static bool GenerateLoginString(string& s);
	static bool GenerateLoginString(MinerClient& client, string& s);
		
	static bool GenerateMerkleRoot(vector<Hash> TransactionHashes, uint8_t* Coinbase, unsigned int CoinbaseLength, Hash& MerkleRoot);

	static bool StratumNotify(MinerClient& client, Document& s);
	static bool StratumSetDifficulty(MinerClient& client, Document& s);
	static bool StratumGetVersion(MinerClient& client, Document& s);
	static bool StratumShowMessage(MinerClient& client, Document& s);

	static bool StratumHandleLogin(MinerClient& client, Document& s);
	static bool StratumHandleShareResponse(MinerClient& client, Document& s);
	
	static void DecodeNetworkInteger(char* str, char* destination);

};

#endif