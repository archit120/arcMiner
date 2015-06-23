#include "StratumProtocol.h"
#include "StratumNetwork.h"
#include "StratumHelpers.h"
#include "Helpers.h"

bool StratumProtocol::Login()
{
	return StratumProtocol::Login(GlobalClient);
}
bool StratumProtocol::Login(MinerClient& client)
{
	StratumNetwork::Connect(client);

	
	string s;

	while(true)
	{
		if(!StratumNetwork::Receive(client, s))
			return false;

		if(!s.length())
			Sleep(50);
		else
			break;
	}

	InitializeCriticalSection(&client.cs_Job);
	InitializeCriticalSection(&client.Stratum.cs_share);
	return true;
}

bool StratumProtocol::HandleShares()
{
	return StratumProtocol::HandleShares(GlobalClient);
}
bool StratumProtocol::HandleShares(MinerClient& client)
{
	vector<StratumShare> shares;
	EnterCriticalSection(&client.Stratum.cs_share);
	int size = client.Stratum.Shares.size();
	for(int i = size-1; i >= 0; i--)
	{
		StratumShare share = client.Stratum.Shares[i];
		shares.push_back(share);
		client.Stratum.Shares.pop_back();
	}
	LeaveCriticalSection(&client.Stratum.cs_share);
	for(int i = 0;i<shares.size();i++)
	{
		//Submit share over here
	}
	return true;
}

bool StratumProtocol::GetWork(WorkBlob& work)
{
	return GetWork(GlobalClient, work);
}

bool StratumProtocol::GetWork(MinerClient& client, WorkBlob& work)
{
	//assert(client.CurrentJob.Id.length());
	memset(work.Blob, 0, 128);
	work.Length = 80;

	memcpy(work.Blob, client.CurrentJob.Version, 4);
	memcpy(work.Blob + 4, client.CurrentJob.PrevHash.data, 32);
	//We need a lock here, may be avoidable
	EnterCriticalSection(&client.cs_Job);
	Helpers::GenerateExtraNonce((char *)client.CurrentJob.ENonce2, client.Stratum.ENonce2Size);
	Hash merkleRoot;
	StratumHelpers::GenerateMerkleRoot(client.CurrentJob.TransactionHashes, client.CurrentJob.Coinbase, client.CurrentJob.CoinbaseSize, merkleRoot);


	memcpy(work.Blob + 36, merkleRoot.data, 32);

	memcpy(work.Blob + 68, client.CurrentJob.Ntime, 4);
	memcpy(work.Blob + 72, client.CurrentJob.Nbits, 4);
	LeaveCriticalSection(&client.cs_Job);
	return true;
}

bool StratumProtocol::HandleMethod(string s)
{
	return HandleMethod(GlobalClient, s);
}

bool StratumProtocol::HandleMethod(MinerClient& client, string s)
{
	Document doc;

	doc.Parse(s.c_str());


	if (doc.HasMember("method"))
	{
		const char* method = doc["method"].GetString();

		if (!stricmp(method, "mining.notify"))
		{
			return StratumHelpers::StratumNotify(client, doc);
		}
		else if (!stricmp(method, "mining.set_difficulty"))
		{
			return StratumHelpers::StratumSetDifficulty(client, doc);
		}
		else if (!stricmp(method, "client.reconnect"))
		{
			printf("Ignoring request to reconnect!\n");
			return true;
		}
		else if (!stricmp(method, "client.get_version"))
		{
			return StratumHelpers::StratumGetVersion(client, doc);
		}
		else if (!stricmp(method, "client.show_message"))
		{
			return StratumHelpers::StratumShowMessage(client, doc);
		}
		printf("Unknown method sent by stratum server : %s\n", method);
	}
	else if (doc.HasMember("result"))
	{
		const int returnId = doc["id"].GetInt();

		if (client.Stratum.IdMap.count(returnId) > 0)
		{
			if (client.Stratum.IdMap[returnId] == "mining.authorize")
			{
				return StratumHelpers::StratumHandleLogin(client, doc);
			}

			printf("Found in return map but no matching clause: %s, ignoring", client.Stratum.IdMap[returnId].c_str());
		}

		printf("Unknown return sent by stratum server : %u, ignoring\n", returnId);
	}
	
	return false;
}

void StratumProtocol::StratumThreadStarter()
{
	new thread(StratumProtocol::StratumThread, ref(GlobalClient));
}
void StratumProtocol::StratumThreadStarter(MinerClient& client)
{
	new thread(StratumProtocol::StratumThread, ref(client));
	
}

void StratumProtocol::StratumThread(MinerClient& client)
{

	//We only want to connect one of the clients. Create a lock over here
	if (!client.Connected)//Connect one
	{
		StratumNetwork::Connect(client);
	}
	uint32 timerPrintDetails = GetTickCount() + 8000;

	while(true)
	{
		if(!client.Connected)
		{
			printf("Error in StratumProtocol::StratumThread, client is not conencted. Retrying in 10 seconds.\n");
			Sleep(10*1000);
			StratumNetwork::Connect(client);
		}
		else
		{
			string s;
			if(!StratumNetwork::Receive(client, s))
			{
				printf("diconnecting.. and reconnecting in 5 seconds.\n");
				Sleep(10*1000);
				StratumNetwork::Connect(client);
				continue; // The client disconnected
			}
			if(s!= "" && !StratumProtocol::HandleMethod(client, s))
			{
				printf("StratumProtocol::StratumThread ignores unknown method.\n");

			}

			 uint32 passedSeconds = (uint32)time(NULL) - MiningStartTime;
			 uint32 currentTick = GetTickCount();

			 if (passedSeconds > 5 && currentTick >= timerPrintDetails)
			 {
				 double hs = (double) TotalHashCount / passedSeconds;
				 printf("Total Hash/s is %d\n", hs);
				 timerPrintDetails = currentTick + 8000;
			 }

		}

		Sleep(50); //50ms sleep
	}
}

