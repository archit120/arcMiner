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
	InitializeCriticalSection(&client.cs_Job);
	InitializeCriticalSection(&client.Stratum.cs_share);
	return true;
}

bool StratumProtocol::AddShares(StratumShare &share)
{
	return StratumProtocol::AddShares(share, GlobalClient);
}
bool StratumProtocol::AddShares(StratumShare &share,MinerClient& client)
{
	EnterCriticalSection(&client.Stratum.cs_share);
	client.Stratum.Shares.push_back(share);
	LeaveCriticalSection(&client.Stratum.cs_share);

	return true;
}

bool StratumProtocol::HandleShares()
{
	return StratumProtocol::HandleShares(GlobalClient);
}
bool StratumProtocol::HandleShares(MinerClient& client)
{
	if (client.Stratum.Shares.size() > 0)
	{
		vector<StratumShare> shares;
		EnterCriticalSection(&client.Stratum.cs_share);
		int size = client.Stratum.Shares.size();
		for (int i = size - 1; i >= 0; i--)
		{
			StratumShare share = client.Stratum.Shares[i];
			shares.push_back(share);
			client.Stratum.Shares.pop_back();
		}
		LeaveCriticalSection(&client.Stratum.cs_share);
		for (int i = 0; i < shares.size(); i++)
		{
			StratumShare s = shares[i];
			char cShare[256];
			char buffer1[9]; Helpers::BinaryToHex(buffer1, s.ENonce2, 4);
			char buffer2[9]; Helpers::BinaryToHex(buffer2, s.Nonce, 4);
			char buffer3[9]; Helpers::BinaryToHex(buffer3, s.Ntime, 4);
			sprintf(cShare, "{ \"params\":[\"%s\", \"%s\", \"%s\", \"%s\", \"%s\"], \"id\" : %u, \"method\" : \"mining.submit\" }", client.Username,s.Id.c_str(), buffer1, buffer3, buffer2, client.Stratum.Id);

			client.Stratum.IdMap.insert(pair<int, string>(client.Stratum.Id, "mining.submit"));
			client.Stratum.Id++;

			bool a = StratumNetwork::Send(client, string(cShare));
			if (!a)
				return false;
		}
	}
	return true;
}

bool StratumProtocol::GetWork(WorkBlob& work)
{
	return GetWork(GlobalClient, work);
}

bool StratumProtocol::GetWork(MinerClient& client, WorkBlob& work)
{
	//we will focus on the locks later though
	EnterCriticalSection(&client.cs_Job);
	//assert(client.CurrentJob.Id.length());
	memset(work.Blob, 0, 128);

	//Add an if statement for scrypt over here
	work.Length = 80;
	work.Id = client.CurrentJob.Id;
	memcpy(work.Blob, client.CurrentJob.Version, 4);
	memcpy(work.Blob + 4, client.CurrentJob.PrevHash.data, 32);
	Helpers::GenerateExtraNonce((char *)client.CurrentJob.ENonce2, client.Stratum.ENonce2Size);
	memcpy(work.ENonce2, client.CurrentJob.ENonce2, client.Stratum.ENonce2Size);
	Hash merkleRoot;
	StratumHelpers::GenerateMerkleRoot(client.CurrentJob.TransactionHashes, client.CurrentJob.Coinbase, client.CurrentJob.CoinbaseSize, merkleRoot);
	

	memcpy(work.Blob + 36, client.CurrentJob.MerkleRoot.data, 32);

	memcpy(work.Blob + 68, client.CurrentJob.Ntime, 4);
	memcpy(work.Blob + 72, client.CurrentJob.Nbits, 4);

	work.NoncePointer =(int*)(work.Blob + 76);
	work.NonceSize = 4;
	work.NtimePointer = (int*)(work.Blob + 68);
	work.NtimeSize = 4;

	memcpy(work.ShareTarget.data, client.CurrentJob.ShareTarget.data, 32);
	work.ShareTarget = StratumHelpers::TargetFromDifficulty(32, Scrypt);
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
			}if (client.Stratum.IdMap[returnId] == "mining.submit")
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
	 thread t(StratumProtocol::StratumThread, ref(GlobalClient));
	 t.detach();

}
void StratumProtocol::StratumThreadStarter(MinerClient& client)
{
	thread t(StratumProtocol::StratumThread, ref(client));
	t.detach();
	
}

void StratumProtocol::StratumThread(MinerClient& client)
{

	//We only want to connect one of the clients. Create a lock over here
	if (!client.Connected)//Connect one
	{
		StratumNetwork::Connect(client);
	}
	uint32_t timerPrintDetails = GetTickCount() + 8000;

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
				printf("diconnected.. and reconnecting in 10 seconds.\n");
				Sleep(10*1000);
				StratumNetwork::Connect(client);
				continue; // The client disconnected
			}
			if(s!= "" && !StratumProtocol::HandleMethod(client, s))
			{
				printf("StratumProtocol::StratumThread ignores unknown method.\n");

			}

			 uint32_t passedSeconds = (uint32_t)time(NULL) - client.MiningStartTime;
			 uint32_t currentTick = GetTickCount();

			 if (passedSeconds > 5 && currentTick >= timerPrintDetails)
			 {
				 double hs = (double) client.TotalHashCount / passedSeconds;
				 printf("Total Hash/s is %f\n", hs);
				 timerPrintDetails = currentTick + 8000;
				 
			 }
			 if (!HandleShares())
			 {
				 printf("Error occured while submitting share.\n");
			 }

		}

		Sleep(50); //50ms sleep
	}
}

