#include "NetworkHelpers.h"


bool NetworkHelpers::Start()
{	
#ifdef _MSC_VER
	WSADATA wsa;
	int r = WSAStartup(MAKEWORD(2,2),&wsa);
	if(!r)
		return true;

	cout << "NetworkHelpers::Start() failed with : " << WSAGetLastError() << endl;
	return false;
#else
	return true;
#endif
}

bool NetworkHelpers::GetIpFromUrl(char* URL, IP& ip)
{
	//Already compatible with linux?

	hostent* hostInfo = gethostbyname(URL);
	if( hostInfo == NULL )
	{
		cout << "Cannot resolve '" << URL << "'. Is it a valid URL?\n" << endl;
		return false;
	} 
	
	void** ipListPtr = (void**)hostInfo->h_addr_list;
	uint32_t ipu = 0xFFFFFFFF;
	if( ipListPtr[0] )
	{
		ipu = *(uint32_t*)ipListPtr[0];
	}
	sprintf(ip.data, "%d.%d.%d.%d", ((ipu>>0)&0xFF), ((ipu>>8)&0xFF), ((ipu>>16)&0xFF), ((ipu>>24)&0xFF));
	return true;
}

bool NetworkHelpers::OpenConnection()
{
	return OpenConnection(GlobalClient.ClientSocket, GlobalClient.Target);
}
bool NetworkHelpers::OpenConnection(SOCKET& s, RequestTarget target)
{
	//Should just work again
	s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if( s == SOCKET_ERROR )
	{
		cout << "NetworkHelpers::OpenConnection failed in creating socket with " << WSAGetLastError() << endl;
		return false;
	}
#ifndef _MSC_VER
#define SOCKADDR_IN sockaddr_in
#endif

	SOCKADDR_IN addr;
	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(target.port);
	addr.sin_addr.s_addr=inet_addr(target.Ip.data);

	int r = connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR_IN));
#ifdef _MSC_VER
	if(r)
	{		
		cout << "NetworkHelpers::OpenConnection failed in connecting socket with " << WSAGetLastError() << endl;
		return false;
	}
#endif
	return true;
}

bool NetworkHelpers::Connect()
{
	return Connect(GlobalClient);
}
bool NetworkHelpers::Connect(MinerClient& client)
{
	if(client.Connected)
		return true;
	if(!OpenConnection(client.ClientSocket, client.Target))
		return false;
#ifdef _MSC_VER
	unsigned int nonblocking=1;
	unsigned int cbRet;
	WSAIoctl(client.ClientSocket, FIONBIO, &nonblocking, sizeof(nonblocking), NULL, 0, (LPDWORD)&cbRet, NULL, NULL);
#else
	int flags, err;
	flags = fcntl(clientSocket, F_GETFL, 0);
	flags |= O_NONBLOCK;
	err = fcntl(clientSocket, F_SETFL, flags); //ignore errors for now..
#endif

	client.Connected = true;
	
	return true;
}


bool NetworkHelpers::Send(string s)
{
	return NetworkHelpers::Send(GlobalClient, s);
}

bool NetworkHelpers::Send(MinerClient client, string s)
{
	int bytes = send(client.ClientSocket, s.c_str(), s.length(), 0);
	if(bytes> 0)
		return true;
	cout << "NetworkHelpers::Send failed with " << WSAGetLastError() << endl;
	return false;
}


bool NetworkHelpers::Receive(int bytesToReceive, string& s)
{
	return NetworkHelpers::Receive(GlobalClient, bytesToReceive, s);
}

bool NetworkHelpers::Receive(MinerClient& client, int bytesToReceive,string& s)
{
	char c[256];
	memset(c, 0, 256);
	s = "";
	for(int i = 0;i<=bytesToReceive/256;i++)
	{
		int toReceive = i < bytesToReceive/256 ? 256 : bytesToReceive - (i * 256);
		signed int r = recv(client.ClientSocket, c, toReceive, 0);
		if( r <= 0 )
		{
#ifdef _MSC_VER
			int e = WSAGetLastError();
			// receive error, is it a real error or just because of non blocking sockets?
			if( e != WSAEWOULDBLOCK )
			{
				printf("NetworkHelpers::Receive failed with %d\n", e);
				client.Connected = false;
				client.LoggedIn = false;
				return false;
			}
#else
			if (errno != EAGAIN || r == 0)
			{
				xprintf("NetworkHelpers::Receive failed with %d\n", e);
				client.Connected = false;
				client.LoggedIn = false;
				return false;
			}
#endif
			return true;
		}
		s.append(c);
	}
	return true;
}
