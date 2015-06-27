#include "global.h"
#include "StratumProtocol.h"
#include "StratumHelpers.h"
#include "NetworkHelpers.h"
#include "ScryptCPUMiner.h"
#include "Helpers.h"

MinerClient GlobalClient;
MinerClient Client;
volatile uint32_t GlobalUniqueGenerator;
volatile uint64_t TotalHashCount;
uint64_t GlobalMiningStartTime;

int main()
{
	AppLog::Log(General, "%u", 2);
	printf("\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\n");
	printf("\xBA  arcMiner (alpha)                                    \xBA\n");
	printf("\xBA  author: archit                                      \xBA\n");
	printf("\xBA  credit: jh00,pooler,authors of rapidjson,cpuminer   \xBA\n");
	printf("\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n");


	NetworkHelpers::Start();

	Client.Username = "yrestest.a";
	Client.Password = "byy";
	Client.Stratum.Id = 1;
	Client.Algorithm = Scrypt;
	Client.MiningStartTime = -1;

	NetworkHelpers::GetIpFromUrl("global.wemineltc.com", Client.Target.Ip);
	Client.Target.port = 3333;

	printf("arcMiner::main Logging in!\n");
	if (!StratumProtocol::Login(Client))
	{
		printf("arcMiner::main Failed at initial log in, exiting!\n");
		return 0;
	}
	StratumProtocol::StratumThreadStarter(Client);

	while (!Client.LoggedIn && !Client.LogInFailed){ Sleep(50); }

	while (!Client.LogInFailed)
	{
		if (Client.LoggedIn && Client.Connected && Client.CurrentJob.Id != "")
		{
			WorkBlob work;
			StratumProtocol::GetWork(Client, work);
			if (Client.MiningStartTime == -1)
				Client.MiningStartTime = (uint32_t)time(NULL);
			vector<thread*> threads;
			for (int i = 0; i < 4; i++)
			{
				thread* t = new thread(ScryptCPUMiner::BlockMine, ref(Client));
				threads.push_back(t);
			}
			/*while (work.Id == Client.CurrentJob.Id && Client.LoggedIn)
			{
				Sleep(500);
			}*/

			for (int i = 0; i < 4; i++)
			{
				(*(threads[i])).join(); 
				delete (threads[i]);
			}
		}
		
		Sleep(50);
	}

}