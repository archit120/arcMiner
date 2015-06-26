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
uint32_t GlobalMiningStartTime;
extern "C"{
void scrypt_core_6way(uint32_t *X, uint32_t *V, int N);
}

int main()
{
	NetworkHelpers::Start();

	Client.Username = "yrestest.a";
	Client.Password = "b";
	Client.Stratum.Id = 1;
	Client.Algorithm = Scrypt;
	Client.MiningStartTime = -1;

	NetworkHelpers::GetIpFromUrl("global.wemineltc.com", Client.Target.Ip);
	Client.Target.port = 3333;

	StratumProtocol::Login(Client);

	StratumProtocol::StratumThreadStarter(Client); 

	while (!Client.LoggedIn){ Sleep(50); }

	Sleep(2000);

	WorkBlob work;

	StratumProtocol::GetWork(Client, work);
	Client.MiningStartTime = (uint32_t)time(NULL);
	while (true)
	{
		if (Client.LoggedIn && Client.Connected && Client.CurrentJob.Id != "")
		{
			vector<thread*> threads;
			for (int i = 0; i < 4; i++)
			{
				thread* t = new thread(ScryptCPUMiner::BlockMine, ref(Client));
				threads.push_back(t);
			}
			while (work.Id == Client.CurrentJob.Id)
			{
				Sleep(50); 
			}

			for (int i = 0; i < 4; i++)
			{
				(*(threads[i])).join();
				delete (threads[i]);
			}
			printf("New job!\n");
		}
	}

}