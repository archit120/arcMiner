#ifndef _H_GLOBAL
#define _H_GLOBAL

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <time.h>

#pragma comment(lib,"Ws2_32.lib")
#include <Winsock2.h>
#include <ws2tcpip.h>

#include <rapidjson\rapidjson.h>
#include <rapidjson\document.h>

#include "sha2.h"

using namespace std;
using namespace rapidjson;

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
	int port;
};

struct StratumShare
{

};

struct StratumClient
{
	size_t ENonce2Size;
	char Nonce1[8];
	int Nonce1Size;
	CRITICAL_SECTION cs_share;
	vector<StratumShare> Shares;
};

struct Hash
{
	unsigned char data[32];
};

struct Job
{
	string Id;
	Hash PrevHash;
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
	int Length;
	int NoncePointer;
	size_t NonceSize;
};

struct MinerClient
{
	 Algorithms Algorithm;
	 SOCKET ClientSocket;
	 string Username;
	 string Password;
	 bool Connected;
	 RequestTarget Target;
	 StratumClient Stratum;
	 WorkBlob Work;
	 Job CurrentJob;
	 CRITICAL_SECTION cs_Job;
};

extern MinerClient Client;
extern volatile uint32_t UniqueGenerator;
extern volatile uint64_t TotalHashCount;
extern uint32 MiningStartTime;
#endif

