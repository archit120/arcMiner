#ifndef _H_GLOBAL
#define _H_GLOBAL

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <time.h>
#include <map>
#include <thread>

#include <chrono>
#include <thread>

//Cross platform sleep


#if defined(_MSC_VER)
#pragma comment(lib,"Ws2_32.lib")
#include <Winsock2.h>
#include <ws2tcpip.h>

#include <rapidjson\rapidjson.h>
#include <rapidjson\document.h>

/*#include "crypto\sha2.h"
#include "crypto\scrypt.h"*/

#include "Crypto\CryptConfig.h"
#else
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "Crypto/CryptConfig.h"

#define Sleep(x) usleep((x)*1000)
#define _strdup strdup
#define SOCKET int
#define stricmp strcasecmp
#define GetTickCount() (uint32_t) (time(NULL) - 1383638888) * 1000 // A quick hack for time_t overflow

#endif

#define MinerVersion "arcMiner alpha"


//Needer for Pooler's cpuminer code


using namespace std;
using namespace rapidjson;

//Stupid hack?!
#ifdef _MSC_VER 
#define snprintf c99_snprintf

inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
	
	int count = -1;

	if (size != 0)
		count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
	if (count == -1)
		count = _vscprintf(format, ap);

	return count;
}


inline int c99_snprintf(char* str, size_t size, const char* format, ...)
{
	int count;
	va_list ap;

	va_start(ap, format);
	count = c99_vsnprintf(str, size, format, ap);
	va_end(ap);

	return count;
}
#endif


struct ThreadLockSection{
#if defined(_MSC_VER)
	CRITICAL_SECTION cs;
#else
	pthread_mutex_t cs;
#endif

	bool Initialized;
};


#include "AppLog.h"
#include "ThreadLock.h"

enum Protocol{
	Stratum,
	Xpt
};

enum Algorithms
{
	SHA256,
	Scrypt,
	Keccak,
	Momentum,
	X11,
	X13
};

struct Target
{
	uint32_t data[8];
};

struct IP
{
	char data[32];
};

struct RequestTarget
{
	IP Ip;
	volatile int port;
};

struct StratumShare
{
	string Id;
	char ENonce2[4];
	char Nonce[4];
	char Ntime[4];
};

struct StratumClient
{
	size_t ENonce2Size;
	char Nonce1[8];
	volatile int Nonce1Size;
	ThreadLockSection cs_share;
	vector<StratumShare> Shares;
	volatile double Difficulty;
	volatile double NextDifficulty;
	volatile int Id;
	map<int, string> IdMap; //Used for mathing returns of function calls
	vector<string> PendingNotifications; // Some stratum servers send messages before autorization
};

struct Hash
{
	unsigned char data[32];
};

struct Job
{
	string Id;
	Hash PrevHash;
	//Hash MerkleRoot;
	Target ShareTarget;
	size_t CoinbaseSize;
	unsigned char *Coinbase;
	unsigned char *ENonce2;

	vector<Hash> TransactionHashes;

	unsigned char Version[4];
	unsigned char Nbits[4];
	unsigned char Ntime[4];

};

struct WorkBlob
{
	char Blob[128];
	Target ShareTarget;
	volatile int Length;

	int* NoncePointer;
	volatile size_t NonceSize;

	int* NtimePointer;
	volatile size_t NtimeSize;

	char ENonce2[4];
	string Id;


};

struct MinerClient
{
	 Algorithms Algorithm;
	 SOCKET ClientSocket;
	 string Username;
	 string Password;
	 volatile bool Connected;
	 volatile bool LoggedIn;
	 volatile bool WorkAvailaible; 
	 volatile bool LogInFailed; 
	 RequestTarget Target;
	 StratumClient Stratum;
	 WorkBlob Work;
	 Job CurrentJob;
	 Protocol CurrentProtocol;
	 ThreadLockSection cs_Job;
	 volatile uint64_t TotalHashCount;
	 volatile uint64_t TotalCollisionCount;
	 volatile uint64_t UniqueGenerator;
	 volatile uint64_t MiningStartTime;

	 volatile uint32_t SharesAccepted;
	 volatile uint32_t SharesRejected;
};

extern MinerClient GlobalClient;
extern volatile uint32_t GlobalUniqueGenerator;
extern volatile uint64_t GlobalTotalHashCount;
extern volatile uint64_t GlobalTotalCollisionCount;
extern uint64_t GlobalMiningStartTime;
#endif

