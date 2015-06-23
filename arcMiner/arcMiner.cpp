#include "global.h"
#include "StratumProtocol.h"
#include "StratumHelpers.h"
#include "NetworkHelpers.h"
#include "Helpers.h"

MinerClient GlobalClient;
MinerClient Client;
volatile uint32_t UniqueGenerator;
volatile uint64_t TotalHashCount;
uint32 MiningStartTime;

int main()
{
	uint32_t a = 3770024567;
	a = Helpers::BigEndian32Decode((const char*)&a);
	/*uint32_t a = 4352345234;
	a = le32dec(&a);
	a=le32dec(&a);
	a=be32dec(&a);
	a=be32dec(&a);
	a=be32dec(&a);
	a=le32dec(&a);*/

	/*Document doc;
	doc.Parse("{\"params\": [\"b3ba\", \"7dcf1304b04e79024066cd9481aa464e2fe17966e19edf6f33970e1fe0b60277\", \"01000000010000000000000000000000000000000000000000000000000000000000000000ffffffff270362f401062f503253482f049b8f175308\", \"0d2f7374726174756d506f6f6c2f000000000100868591052100001976a91431482118f1d7504daf1c001cbfaf91ad580d176d88ac00000000\", [\"57351e8569cb9d036187a79fd1844fd930c1309efcd16c46af9bb9713b6ee734\", \"936ab9c33420f187acae660fcdb07ffdffa081273674f0f41e6ecc1347451d23\"], \"00000002\", \"1b44dfdb\", \"53178f9b\", true], \"id\": null, \"method\": \"mining.notify\"}");
	MinerClient cl;
	Helpers::HexToBinary("f8002c90", cl.Stratum.Nonce1, 4);
	cl.Stratum.Nonce1Size = 4;
	cl.Stratum.ENonce2Size = 4;
	StratumHelpers::StratumNotify(cl, doc);
	WorkBlob w;
	StratumProtocol::GetWork(cl, w);
	
	char d[161];
	Helpers::BinaryToHex(d, w.Blob, 80);
	printf("%s\n", d);*/
	NetworkHelpers::Start();

	Client.Username = "yrestest.a";
	Client.Password = "b";
	Client.Stratum.Id = 1;

	NetworkHelpers::GetIpFromUrl("global.wemineltc.com", Client.Target.Ip);
	Client.Target.port = 3333;

	StratumProtocol::Login(Client);

	StratumProtocol::StratumThreadStarter(Client); 

	while (!Client.LoggedIn){ Sleep(50); }
	WorkBlob work;
	StratumProtocol::GetWork(Client, work);

	char d[161];
	Helpers::BinaryToHex(d, work.Blob, 80);
	printf("%s\n", d); 

}