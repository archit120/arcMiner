#include "StratumHelpers.h"

Target StratumHelpers::TargetFromDifficulty(double difficulty)
{
	return TargetFromDifficulty(difficulty, Client.Algorithm);
}

Target StratumHelpers::TargetFromDifficulty(double difficulty, Algorithms algorithm)
{
	switch (algorithm)
	{
	case Scrypt:
		difficulty =  difficulty/655360.0;
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
	Target target;
		
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
	return StratumHelpers::GenerateLoginString(Client, s);
}

bool StratumHelpers::GenerateLoginString(MinerClient client,string &s)
{
	char cLogin[128];
	sprintf(cLogin, "{\"params\": [\"%s\", \"%s\"], \"id\": 2, \"method\": \"mining.authorize\"}", client.Username, client.Password);
	s=cLogin;
	return true;
}
 