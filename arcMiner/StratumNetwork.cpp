#include "NetworkHelpers.h" //makes life easier
#include "StratumHelpers.h"
#include "StratumNetwork.h"
#include "Helpers.h"


bool StratumNetwork::Connect()
{
	return Connect(GlobalClient);
}


bool StratumNetwork::Connect(MinerClient& client)
{
	//Reset client's properties

	client.TotalHashCount = 0;
	client.UniqueGenerator = 0;
	client.Work = {};
	client.MiningStartTime = -1;
	client.Stratum = {};
	client.CurrentJob = {};
	client.WorkAvailaible = false;

	InitializeCriticalSection(&client.cs_Job);
	InitializeCriticalSection(&client.Stratum.cs_share);


	if(!NetworkHelpers::Connect(client))
		return false;

	string s = "{\"id\": 1, \"method\": \"mining.subscribe\", \"params\": []}";

	client.Stratum.Id = 2;
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
	//assert((double)strlen(ENonce1.GetString())/2.0 == 0.0);
	if(strlen(ENonce1.GetString()))
	{
		//Store the Nonce1 
 		Helpers::HexToBinary((char *)ENonce1.GetString(), client.Stratum.Nonce1, ENonce1.GetStringLength()/2);
		client.Stratum.Nonce1Size = ENonce1.GetStringLength() / 2;
		
	}
	//Send the login command
	StratumHelpers::GenerateLoginString(client, s);

	if(!StratumNetwork::Send(client, s))
		return false;

	cout << "Login string sent, starting listener thread now!" << endl;

	return true;

	
	/*
	if(!StratumNetwork::BlockReceive(client, s))
		return false;

	doc.Parse(s.c_str());

	assert(doc["error"].IsNull());
	assert(doc["id"].IsNumber());
	assert(doc["result"].IsBool());

	if(!doc["result"].GetBool())
		return false;

	printf("Connected to server using Stratum protocol");

	return true;*/
	
}

bool StratumNetwork::Send(string s)
{
	return StratumNetwork::Send(GlobalClient, s);
}

bool StratumNetwork::Send(MinerClient client, string s)
{
	s.append("\n");

	return NetworkHelpers::Send(client, s);
}

bool StratumNetwork::Receive(string& s)
{
	return StratumNetwork::Receive(GlobalClient, s);
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
		{
			printf("Error in StratumNetwork::Receive, packet size went above maximum limit");
			client.Connected = false;
			client.LoggedIn = false;
			return false;
		}
	}
	return true;
}

bool StratumNetwork::BlockReceive(string& s)
{
	return StratumNetwork::BlockReceive(GlobalClient, s);
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

