#include "Helpers.h"


bool Helpers::GenerateMerkleRoot(vector<Hash> TransactionHashes, uint8_t* Coinbase, unsigned int CoinbaseLength , Hash& MerkleRoot)
{
	uint8_t merkleRoot[64];
	sha256d(Coinbase, CoinbaseLength, merkleRoot);
	for(int i = 0; i < TransactionHashes.size(); i++)
	{
		memcpy(merkleRoot+32, TransactionHashes[i].data, 32);
		sha256d(merkleRoot, 64, merkleRoot);
	}
	memcpy(MerkleRoot.data, merkleRoot, 32);

	return true;
}
