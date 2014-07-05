#include "global.h"
#include "StratumProtocol.h"
#include "NetworkHelpers.h"

MinerClient Client;

int main()
{
	NetworkHelpers::Start();

	Client.Username = "archit.dog1";
	Client.Password = "x";

	NetworkHelpers::GetIpFromUrl("ypool.net", Client.Target.Ip);
	Client.Target.port = 9090;

	StratumProtocol::Login(Client);


}