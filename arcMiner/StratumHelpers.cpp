#include "StratumHelpers.h"
#include "Helpers.h"
#include "StratumNetwork.h"

void StratumHelpers::DecodeNetworkInteger(char* str, char* destination)
{
	char a[4];
	Helpers::HexToBinary(str, a, 4);
	uint32_t b = Helpers::BigEndian32Decode(a);
	memcpy(destination, &b, 4);
}

Target StratumHelpers::TargetFromDifficulty(double difficulty)
{
	return TargetFromDifficulty(difficulty, GlobalClient.Algorithm);
}

Target StratumHelpers::TargetFromDifficulty(double difficulty, Algorithms algorithm)
{
	Target target;
	uint32_t d = difficulty;
	uint64_t a = 0xFFFF00000000;
	uint64_t b = a / difficulty;
	switch (algorithm)
	{
	case Scrypt:
		memset(target.data, 0, 32);
		((uint64_t*)target.data)[3] = b;
		return target;
		//difficulty =  difficulty/655360.0;
		break;
	case SHA256:
	case Keccak:
	case Momentum:
	case X11:
	case X13:
		break;
	default:
		throw invalid_argument("Algorithm type is unknown.");
		break;
	}
		
	uint64_t m;
	int k;

	for (k = 6; k > 0 && difficulty > 1.0; k--)
	{
		difficulty /= 4294967296.0;
	}

	m = 4294901760.0 / difficulty;
	if (m == 0 && k == 6)
		memset(target.data, 0xff, 32);
	else 
	{
		memset(target.data, 0, 32);
		target.data[k] = (uint32_t)m;
		target.data[k + 1] = (uint32_t)(m >> 32);
	}

	return target;
}

bool StratumHelpers::GenerateLoginString(string& s)
{
	return StratumHelpers::GenerateLoginString(GlobalClient, s);
}

bool StratumHelpers::GenerateLoginString(MinerClient& client,string &s)
{
	char cLogin[128];
	sprintf(cLogin, "{\"params\": [\"%s\", \"%s\"], \"id\": %u, \"method\": \"mining.authorize\"}", client.Username, client.Password, client.Stratum.Id);
	s=cLogin;
	client.Stratum.IdMap.insert(pair<int, string>(client.Stratum.Id, "mining.authorize"));
	client.Stratum.Id++;
	return true;
}
 
bool StratumHelpers::GenerateMerkleRoot(vector<Hash> TransactionHashes, uint8_t* Coinbase, unsigned int CoinbaseLength , Hash& MerkleRoot)
{
	uint8_t merkleRoot[64];

	sha256d(merkleRoot, Coinbase, CoinbaseLength);

	for(int i = 0; i < TransactionHashes.size(); i++)
	{
		memcpy(merkleRoot+32, TransactionHashes[i].data, 32);
		sha256d(merkleRoot, merkleRoot, 64);
	}

	//Reverse endiannness
	/*for(int i = 0; i < 8; i++)
	{
		((uint32_t*)merkleRoot)[i] = Helpers::BigEndian32Decode((uint32_t*)merkleRoot + i);
	}*/

	memcpy(MerkleRoot.data, merkleRoot, 32);

	return true;
}

bool StratumHelpers::StratumNotify(MinerClient& client, Document& s)
{
	Value& params = s["params"];
	if(params.Size() != 9)
	{
		printf("Error in StratumHelpers::StratumNotify, invalid number of parameters supplied by server.");
	}
	EnterCriticalSection(&client.cs_Job);
	client.CurrentJob.CoinbaseSize = 2;
	client.CurrentJob.Id = params[0u].GetString();

	client.Stratum.Difficulty = client.Stratum.NextDifficulty;
	client.CurrentJob.ShareTarget = StratumHelpers::TargetFromDifficulty(client.Stratum.Difficulty, client.Algorithm);

	Helpers::HexToBinary((char *)params[1].GetString(), (char *)client.CurrentJob.PrevHash.data, params[1].GetStringLength()/2);

	int cb1length = params[2].GetStringLength()/2;
	int cb2length = params[3].GetStringLength()/2;

	char* cb1 = (char *)malloc(cb1length);
	char* cb2 = (char *)malloc(cb2length);


	Helpers::HexToBinary((char *)params[2].GetString(), cb1, cb1length);
	Helpers::HexToBinary((char *)params[3].GetString(), cb2, cb2length);
	if (client.CurrentJob.Coinbase != 0x0)
		free(client.CurrentJob.Coinbase);
	char* coinbase = (char *)malloc(cb1length + cb2length + client.Stratum.Nonce1Size + client.Stratum.ENonce2Size + cb2length);

	memcpy(coinbase, cb1, cb1length);
	memcpy(coinbase + cb1length, client.Stratum.Nonce1, client.Stratum.Nonce1Size);
	memcpy(coinbase + cb1length + client.Stratum.Nonce1Size + client.Stratum.ENonce2Size, cb2, cb2length); //Pregenerated coinbase but leave bytes in between for enonce2

	free(cb1);
	free(cb2);

	client.CurrentJob.ENonce2 = (unsigned char*) coinbase + cb1length + client.Stratum.Nonce1Size;
	client.CurrentJob.Coinbase = (unsigned char*) coinbase;
	client.CurrentJob.CoinbaseSize = cb1length + client.Stratum.Nonce1Size + client.Stratum.ENonce2Size + cb2length;
	StratumHelpers::DecodeNetworkInteger((char*)params[6].GetString(), (char *)client.CurrentJob.Nbits);
	StratumHelpers::DecodeNetworkInteger((char*)params[7].GetString(), (char *)client.CurrentJob.Ntime);
	Helpers::HexToBinary((char *)params[1].GetString(), (char *)client.CurrentJob.PrevHash.data, params[1].GetStringLength()/2);

	for (int i = 0; i < 8; i++)
	{
		uint32_t a = ((uint32_t*)client.CurrentJob.PrevHash.data)[i];
		uint32_t b = Helpers::BigEndian32Decode(&a);
		((uint32_t*)client.CurrentJob.PrevHash.data)[i] = b;
	}


	StratumHelpers::DecodeNetworkInteger((char*)params[5].GetString(), (char *)client.CurrentJob.Version);
	client.CurrentJob.TransactionHashes.clear();
	for (int i = 0u; i < params[4].Size(); i++)
	{
		char c[32];
		Helpers::HexToBinary((char *)params[4][i].GetString(), c, params[4][i].GetStringLength());
		Hash h;
		memcpy(h.data, c, 32);
		client.CurrentJob.TransactionHashes.push_back(h);
	}

	LeaveCriticalSection(&client.cs_Job);
	return true;
}
 
bool StratumHelpers::StratumSetDifficulty(MinerClient& client, Document& s)
{
	Value& params = s["params"];
	client.Stratum.NextDifficulty = params[0u].GetDouble();
	return true;
}

bool StratumHelpers::StratumGetVersion(MinerClient& client, Document& s)
{
	char buffer[128];
	sprintf(buffer, "{\"id\":\"%d\", \"error\":null, \"result\":\"%s\"}", s["id"].GetInt(), MinerVersion);
	return StratumNetwork::Send(client, string(buffer));
}

bool StratumHelpers::StratumShowMessage(MinerClient& client, Document& s)
{
	printf("Message from server: %s\n", s["params"][0u].GetString());
	char buffer[128];
	sprintf(buffer, "{\"id\":\"%d\", \"error\":null, \"result\":true}", s["id"].GetInt());
	return StratumNetwork::Send(string(buffer));

}

bool StratumHelpers::StratumHandleLogin(MinerClient& client, Document& s)
{
	printf("Recevied response for login request\n");
	if (s["result"].GetBool() == true && s["error"].IsNull() == true)
	{
		printf("Successfully logged in!\n");
		//Go through the commands received before we were logged in?
		client.Connected = true;
		client.LoggedIn = true;

		return true;
	}
	return false;
}