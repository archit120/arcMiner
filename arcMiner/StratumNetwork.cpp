#include "NetworkHelpers.h" //makes life easier
#include "StratumHelpers.h"
#include "StratumNetwork.h"



bool StratumNetwork::Connect(void)
{
	return Connect(Client);
}


bool StratumNetwork::Connect(MinerClient& client)
{
	if(!NetworkHelpers::Connect(client))
		return false;
	string s = StratumHelpers::GenerateLoginString(client);

	if(!StratumNetwork::Send(client, s))
		return false;

	return true;
	
}

bool StratumNetwork::Send(string s)
{
	return StratumNetwork::Send(Client, s);
}

bool StratumNetwork::Send(MinerClient client, string s)
{
	s.append("\n");

	return NetworkHelpers::Send(client, s);
}