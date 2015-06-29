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

struct CommandLineOptions
{
	char* Host;
	int Port;
	char* WorkerName;
	char* WorkerPassword;
	Protocol ServerProtocol;
	Algorithms MiningAlgorithm;
	int Threads;
};

void arcMiner_printHelp()
{
	puts("Usage: arcMiner.exe [options]");
	puts("General options:");
	puts("   -o, -O                        The miner will connect to this url");
	puts("                                 You can specify a port after the url using -o url:port");
	puts("   -u                            The username (workername) used for login");
	puts("   -p                            The password used for login");
	puts("   -t <num>                      The number of threads for mining (default is set to number of cores)");
	puts("                                 For most efficient mining, set to number of physical CPU cores");
	puts("   -a <algo>                     Name of the mining POW in case of stratum server");
	puts("                                 Possible values - scrypt");
	puts("   -pr <protocol>                Server connection protocol to be used");
	puts("                                 Possible values - stratum");

	//puts("MaxCoin specific:");
	//puts("   -gpu						   Use OpenCL GPU acceleration");
	puts("Example usage:");
	puts("   arcMiner.exe -o http://poolurl.com:10034 -u workername.pts_1 -p workerpass -t 4 -pr stratum -a scrypt");
}
CommandLineOptions arcMiner_parseCommandline(int argc, char **argv)
{
	CommandLineOptions commandlineInput;
	int cIdx = 1;
	while (cIdx < argc)
	{
		char* argument = argv[cIdx];
		cIdx++;
		if (memcmp(argument, "-o", 3) == 0 || memcmp(argument, "-O", 3) == 0)
		{
			// -o
			if (cIdx >= argc)
			{
				printf("Missing URL after -o option\n");
				exit(0);
			}
			if (strstr(argv[cIdx], "http://"))
				commandlineInput.Host = _strdup(strstr(argv[cIdx], "http://") + 7);
			else
				commandlineInput.Host = _strdup(argv[cIdx]);
			char* portStr = strstr(commandlineInput.Host, ":");
			if (portStr)
			{
				*portStr = '\0';
				commandlineInput.Port = atoi(portStr + 1);
			}
			cIdx++;
		}
		else if (memcmp(argument, "-u", 3) == 0)
		{
			// -u
			if (cIdx >= argc)
			{
				printf("Missing username/workername after -u option\n");
				exit(0);
			}
			commandlineInput.WorkerName = _strdup(argv[cIdx]);
			cIdx++;
		}
		else if (memcmp(argument, "-p", 3) == 0)
		{
			// -p
			if (cIdx >= argc)
			{
				printf("Missing password after -p option\n");
				exit(0);
			}
			commandlineInput.WorkerPassword = _strdup(argv[cIdx]);
			cIdx++;
		}
		else if (memcmp(argument, "-t", 3) == 0)
		{
			// -t
			if (cIdx >= argc)
			{
				printf("Missing thread number after -t option\n");
				exit(0);
			}
			commandlineInput.Threads = atoi(argv[cIdx]);
			if (commandlineInput.Threads < 1 || commandlineInput.Threads > 128)
			{
				printf("-t parameter out of range");
				exit(0);
			}
			cIdx++;
		}
		else if (memcmp(argument, "-a", 3) == 0)
		{
			if (cIdx >= argc)
			{
				printf("Missing algorithm name after -a option\n");
				exit(0);
			}
			if (strcmp(argv[cIdx],"scrypt") ==0)
				commandlineInput.MiningAlgorithm = Scrypt;
			else
			{
				printf("-a parameter invalid");
				exit(0);
			}
			cIdx++;
		}
		else if (memcmp(argument, "-pr", 4) == 0)
		{
			if (cIdx >= argc)
			{
				printf("Missing protocol name after -pr option\n");
				exit(0);
			}
			if (strcmp(argv[cIdx] , "stratum") == 0)
				commandlineInput.ServerProtocol = Stratum;
			else
			{
				printf("-pr parameter invalid");// , argv[cIdx]);
				exit(0);
			}
			cIdx++;
		}
		else if (memcmp(argument, "-help", 6) == 0 || memcmp(argument, "--help", 7) == 0)
		{
			arcMiner_printHelp();
			exit(0);
		}
		else
		{
			printf("'%s' is an unknown option.\nType arcMiner.exe --help for more info\n", argument);
			exit(-1);
		}
	}
	if (argc <= 1)
	{
		arcMiner_printHelp();
		exit(0);
	}

	return commandlineInput;
}
int main(int argc, char** argv)
{
	//AppLog::Log(General, "%u", 2);
	printf("\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\n");
	printf("\xBA  arcMiner (alpha)                                    \xBA\n");
	printf("\xBA  author: archit                                      \xBA\n");
	printf("\xBA  credit: jh00,pooler,authors of rapidjson,cpuminer   \xBA\n");
	printf("\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n");

	CommandLineOptions options = arcMiner_parseCommandline(argc, argv);
	NetworkHelpers::Start();

	Client.Username = options.WorkerName;
	Client.Password = options.WorkerPassword;
	Client.Stratum.Id = 1;
	Client.Algorithm = options.MiningAlgorithm;
	Client.MiningStartTime = -1;

	NetworkHelpers::GetIpFromUrl(options.Host, Client.Target.Ip);
	Client.Target.port = options.Port;

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
			for (int i = 0; i < options.Threads -1 ; i++)
			{
				thread* t = new thread(ScryptCPUMiner::BlockMine, ref(Client));
				threads.push_back(t);
			}
			/*while (work.Id == Client.CurrentJob.Id && Client.LoggedIn)
			{
				Sleep(500);
			}*/
			ScryptCPUMiner::BlockMine(Client);

			for (int i = 0; i < options.Threads-1; i++)
			{
				(*(threads[i])).join(); 
				delete (threads[i]);
			}
		}
		
		Sleep(50);
	}

}