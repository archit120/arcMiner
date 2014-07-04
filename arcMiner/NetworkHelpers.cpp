#include "NetworkHelpers.h"


bool NetworkHelpers::OpenConnection()
{
	return OpenConnection(Client.ClientSocket, Client.Target);
}
bool NetworkHelpers::OpenConnection(SOCKET& s, RequestTarget target)
{
	s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if( s == SOCKET_ERROR )
		return false;
	SOCKADDR_IN addr;
	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(target.port);
	addr.sin_addr.s_addr=inet_addr(target.IP.data());
	if(connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR_IN)))
	{
		return false;
	}
	return true;
}

bool NetworkHelpers::Connect()
{
	return Connect(Client);
}
bool NetworkHelpers::Connect(MinerClient& client)
{
	if(client.Connected)
		return true;
	if(!OpenConnection(client.ClientSocket, client.Target))
		return false;

	unsigned int nonblocking=1;
	unsigned int cbRet;
	WSAIoctl(client.ClientSocket, FIONBIO, &nonblocking, sizeof(nonblocking), NULL, 0, (LPDWORD)&cbRet, NULL, NULL);

	Client.Connected = true;
	
	return true;
}


bool NetworkHelpers::Send(string s)
{
	return NetworkHelpers::Send(Client, s);
}

bool NetworkHelpers::Send(MinerClient client, string s)
{
	if(!send(client.ClientSocket, s.data(), s.length(), 0))
		return true;
	return false;
}
