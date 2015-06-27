#include "ScryptCPUMiner.h"
#include "StratumProtocol.h"
#include "Helpers.h"

bool BlockMine2(MinerClient &client, WorkBlob &work);

bool ScryptCPUMiner::BlockMine(MinerClient &client)
{
	WorkBlob work;
	StratumProtocol::GetWork(client, work);

	return BlockMine2(client, work);
}

bool BlockMine2(MinerClient &client, WorkBlob &work)
{
	uint32_t data[SCRYPT_MAX_WAYS * 20]; 
	uint32_t hash[SCRYPT_MAX_WAYS * 8];
	uint32_t midstate[8];


	unsigned char*  ScratchPad = (unsigned char *)malloc(1024 * SCRYPT_MAX_WAYS * 128 + 63);
	uint32_t shareCompare = work.ShareTarget.data[7];
	//shareCompare = Helpers::BigEndian32Decode(&shareCompare);
	uint32_t nonce = 0;
	for (int i = 0; i < 20; i++) //Weird endian adjustment needed for pooler's code
	{
		((int*)work.Blob)[i] = Helpers::BigEndian32Decode(&((int*)work.Blob)[i]);
	}

	int throughput = scrypt_best_throughput();

	if (sha256_use_4way())
		throughput *= 4;
	//throughput = 1;
	for (int i = 0; i < throughput; i++)
	{
		memcpy(data + i * 20, work.Blob, 80);
	}

	sha256_init(midstate);
	sha256_transform(midstate, data, 0);

	for (uint32_t t = 0; t<32768; t++)
	{
		if (client.CurrentProtocol == Stratum)
		{
			if (client.CurrentJob.Id != work.Id)
			{
				free(ScratchPad);
				return true;
			}
		}
		if (!client.Connected || !client.LoggedIn)
		{
			free(ScratchPad);
			return false;
		}
		for (uint32_t x = 0; x<4096; x++)
		{
			for (int i = 0; i < throughput; i++)
				data[i * 20 + 19] = nonce++;

			if (throughput == 4)
				scrypt_1024_1_1_256_4way(data, hash, midstate, ScratchPad, 1024);
			else
			if (throughput == 12)
				scrypt_1024_1_1_256_12way(data, hash, midstate, ScratchPad, 1024);
			
			if (throughput == 24)
				scrypt_1024_1_1_256_24way(data, hash, midstate, ScratchPad, 1024);
			else
			if (throughput == 3)
				scrypt_1024_1_1_256_3way(data, hash, midstate, ScratchPad, 1024);
			else
				scrypt_1024_1_1_256(data, hash, midstate, ScratchPad, 1024);

			for (int i = 0; i < throughput; i++) {
				if (hash[i * 8 + 7] <= shareCompare) {
					printf("We found a share submitting!\n");
					if (client.CurrentProtocol == Stratum)
					{
						StratumShare share;
						//Helpers::BigEndian32Encode(share.Nonce, data[i * 20 + 19]);
						//Helpers::BigEndian32Encode(share.Ntime, *work.NtimePointer);
						//Helpers::BigEndian32Encode(share.ENonce2, *work.ENonce2);
						memcpy(share.Nonce, &(data[i * 20 + 19]), 4);
						memcpy(share.Ntime, work.NtimePointer, 4);
						memcpy(share.ENonce2, work.ENonce2, 4);
						share.Id = work.Id;
						StratumProtocol::AddShares( share, client);
					}
				}
			}
			client.TotalHashCount+=throughput;
		}
	}

	free(ScratchPad);
	return true;
}



/*int scanhash_scrypt(int thr_id, uint32_t *pdata,
	unsigned char *scratchbuf, const uint32_t *ptarget,
	uint32_t max_nonce, unsigned long *hashes_done, int N)
{
	uint32_t n = pdata[19] - 1;
	const uint32_t Htarg = ptarget[7];
	int throughput = scrypt_best_throughput();
	int i;

	if (sha256_use_4way())
		throughput *= 4;
	for (i = 0; i < throughput; i++)
		memcpy(data + i * 20, pdata, 80);

	sha256_init(midstate);
	sha256_transform(midstate, data, 0);

	do {
		for (i = 0; i < throughput; i++)
			data[i * 20 + 19] = ++n;

		if (throughput == 4)
			scrypt_1024_1_1_256_4way(data, hash, midstate, scratchbuf, N);
		else
		if (throughput == 12)
			scrypt_1024_1_1_256_12way(data, hash, midstate, scratchbuf, N);
		else
		if (throughput == 24)
			scrypt_1024_1_1_256_24way(data, hash, midstate, scratchbuf, N);
		else
		if (throughput == 3)
			scrypt_1024_1_1_256_3way(data, hash, midstate, scratchbuf, N);
		else
			scrypt_1024_1_1_256(data, hash, midstate, scratchbuf, N);

		for (i = 0; i < throughput; i++) {
			if (hash[i * 8 + 7] <= Htarg && fulltest(hash + i * 8, ptarget)) {
				*hashes_done = n - pdata[19] + 1;
				pdata[19] = data[i * 20 + 19];
				return 1;
			}
		}
	} while (n < max_nonce && !work_restart[thr_id].restart);

	*hashes_done = n - pdata[19] + 1;
	pdata[19] = n;
	return 0;
}
*/