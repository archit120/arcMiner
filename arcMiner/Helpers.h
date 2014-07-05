#ifndef _H_HELPERS
#define _H_HELPERS

#include "global.h"

class Helpers
{
public:
	static bool GenerateMerkleRoot(vector<Hash> TransactionHashes, uint8_t* Coinbase, unsigned int CoinbaseLength, Hash& MerkleRoot);
};

#endif