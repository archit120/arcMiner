#include "StratumProtocol.h"
#include "StratumNetwork.h"

bool StratumProtocol::Login()
{
	return StratumProtocol::Login(Client);
}
bool StratumProtocol::Login(MinerClient& client)
{
	StratumNetwork::Connect(client);

	string s;

	while(true)
	{
		if(!StratumNetwork::Receive(client, s))
			return false;

		if(!s.length())
			Sleep(50);
		else
			break;
	}

	return true;
}

/*bool StratumProtocol::GetWork(WorkBlob& work)
{
	return true;
}*/

bool StratumProtocol::GetWork(MinerClient& client, WorkBlob& work)
{
	return true;
}

