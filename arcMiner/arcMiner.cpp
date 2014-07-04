#include "global.h"

MinerClient Client;

int main()
{
	cout << "Hello";
	Client.Algorithm = Algorithms::Keccak;
	try
	{
		throw invalid_argument("");	
	}
	catch(std::exception e)
	{
		cout << e.what();
	}
}