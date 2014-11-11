#include "StratumProtocol.h"
#include "StratumNetwork.h"
#include "StratumHelpers.h"
#include "Helpers.h"

bool StratumProtocol::Login()
{
	return StratumProtocol::Login(Client);
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
	return StratumProtocol::HandleShares(Client);
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
	return GetWork(Client, work);
}

bool StratumProtocol::GetWork(MinerClient& client, WorkBlob& work)
{
	assert(client.CurrentJob.Id.length());
	memset(work.Blob, 0, 128);
	work.Length = 80;

	memcpy(work.Blob, client.CurrentJob.Version, 4);
	memcpy(work.Blob + 4, client.CurrentJob.PrevHash.data, 32);
	char b[65];

	Helpers::BinaryToHex(b, (char *)client.CurrentJob.PrevHash.data, 32);

	printf("%s\n", b);
	//We need a lock here, probably avoidable
	//EnterCriticalSection(&client.cs_Job);
	Helpers::GenerateExtraNonce((char *)client.CurrentJob.ENonce2, client.Stratum.ENonce2Size);
	Helpers::HexToBinary("00000002", (char*)client.CurrentJob.ENonce2, 4);
	Hash merkleRoot;
	StratumHelpers::GenerateMerkleRoot(client.CurrentJob.TransactionHashes, client.CurrentJob.Coinbase, client.CurrentJob.CoinbaseSize, merkleRoot);


	memcpy(work.Blob + 36, merkleRoot.data, 32);

	memcpy(work.Blob + 68, client.CurrentJob.Ntime, 4);
	memcpy(work.Blob + 72, client.CurrentJob.Nbits, 4);

	return true;
}

bool StratumProtocol::HandleMethod(string s)
{
	return HandleMethod(Client, s);
}

bool StratumProtocol::HandleMethod(MinerClient& client, string s)
{
	Document doc;
	doc.Parse(s.c_str());

	assert(doc["method"].IsString());

	const char* method = doc["method"].GetString();

	if (!stricmp(method, "mining.notify")) 
	{
		return StratumHelpers::StratumNotify(client, doc);
	}
    /*else if (!stricmp(method, "mining.set_difficulty")) 
	{
		return stratum_set_difficulty(sctx, doc);
    }
    else if (!stricmp(method, "client.reconnect")) 
	{
		return stratum_reconnect(sctx, doc);
	}
	else if (!stricmp(method, "client.get_version")) 
	{
		return stratum_get_version(sctx, doc);
	}
	else if (!stricmp(method, "client.show_message")) 
	{
		return stratum_show_message(sctx, id, doc);
	}*/

	printf("Unknown method sent by stratum server : %s\n", method);
	return false;
}

void StratumProtocol::StratumThreadStarter()
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StratumProtocol::StratumThread, (LPVOID)&Client, 0, NULL);
}

void StratumProtocol::StratumThread(LPVOID lClient)
{
	MinerClient client = *static_cast<MinerClient*>(lClient);
	//We only want to connect one of the clients. Create a lock over here
	if (!client.Connected)//Connect one
	{
		StratumNetwork::Connect(client);
	}
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
			 if(passedSeconds > 5)
			 {
				 double hs = (double) TotalHashCount / passedSeconds;
				 printf("Total Hash/s is %d\n", hs);
			 }

		}

		Sleep(50); //50ms sleep
	}
}

