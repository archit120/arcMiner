#ifndef _H_GLOBAL
#define _H_GLOBAL

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

#pragma comment(lib,"Ws2_32.lib")
#include <Winsock2.h>
#include <ws2tcpip.h>

#include <rapidjson\rapidjson.h>
#include <rapidjson\document.h>

#include "sha2.h"

using namespace std;
using namespace rapidjson;

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

struct StratumClient
{
	size_t ENonce2Size;
	uint64_t Nonce1;
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

	vector<Hash> merkle;

	unsigned char version[4];
	unsigned char nbits[4];
	unsigned char ntime[4];

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
};
extern MinerClient Client;

#endif

