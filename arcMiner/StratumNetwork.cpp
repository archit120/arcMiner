#include "NetworkHelpers.h" //makes life easier
#include "StratumHelpers.h"
#include "StratumNetwork.h"



bool StratumNetwork::Connect()
{
	return Connect(Client);
}


bool StratumNetwork::Connect(MinerClient& client)
{
	if(!NetworkHelpers::Connect(client))
		return false;

	string s = "{\"id\": 1, \"method\": \"mining.subscribe\", \"params\": []}";

	if(!StratumNetwork::Send(client, s))
		return false;
	
	if(!StratumNetwork::BlockReceive(client, s))
		return false;

	Document doc;
	doc.Parse(s.c_str());

	assert(doc.IsObject());

	Value& result = doc["result"];

	assert(result.IsArray());

	Value& ENonce1 = result[1];
	Value& ENonce2 = result[2];

	assert(ENonce1.IsString());
	assert(ENonce2.IsNumber());

	client.Stratum.ENonce2Size = ENonce2.GetInt64();
	assert((double)strlen(ENonce1.GetString())/2.0 == 0.0);
	if(strlen(ENonce1.GetString()))
		client.Stratum.Nonce1 = stoull(ENonce1.GetString(), nullptr, 16);

	StratumHelpers::GenerateLoginString(client, s);

	if(!StratumNetwork::Send(client, s))
		return false;

	if(!StratumNetwork::BlockReceive(client, s))
		return false;

	doc.Parse(s.c_str());

	assert(doc["error"].IsNull());
	assert(doc["id"].IsNumber());
	assert(doc["result"].IsBool());

	if(!doc["result"].GetBool())
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

bool StratumNetwork::Receive(string& s)
{
	return StratumNetwork::Receive(Client, s);
}

bool StratumNetwork::Receive(MinerClient& client, string& s)
{
	s="";
	std::string s2;
	while(true)
	{ 
		if(!NetworkHelpers::Receive(client, 1, s2))
			return false;

		if(!s2.length())
			return true;

		s.append(s2);
		if(!s2.compare("\n"))
			break;

		if(s.length() > MaxPacketSizeStratum)
			return false;
	}
	return true;
}

bool StratumNetwork::BlockReceive(string& s)
{
	return StratumNetwork::BlockReceive(Client, s);
}

bool StratumNetwork::BlockReceive(MinerClient& client, string& s)
{
	
	while(true)
	{
		if(!StratumNetwork::Receive(client, s))
			return false;

		if(!s.length())
			Sleep(50);
		else
			break;
	}
	return true;
}

