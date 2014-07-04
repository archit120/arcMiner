#ifndef _H_GLOBAL
#define _H_GLOBAL

#include <iostream>
#include <stdint.h>

#pragma comment(lib,"Ws2_32.lib")
#include <Winsock2.h>
#include <ws2tcpip.h>


using namespace std;

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

struct RequestTarget
{
	string IP;
	int port;
};

struct MinerClient
{
	 Algorithms Algorithm;
	 SOCKET ClientSocket;
	 string Username;
	 string Password;
	 bool Connected;
	 RequestTarget Target;
};

extern MinerClient Client;

#endif

