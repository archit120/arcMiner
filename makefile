all:
	g++ -std=c++11 -IarcMiner/include arcMiner/AppLog.cpp arcMiner/arcMiner.cpp arcMiner/Helpers.cpp arcMiner/NetworkHelpers.cpp arcMiner/ScryptCPUMiner.cpp arcMiner/StratumHelpers.cpp arcMiner/StratumNetwork.cpp arcMiner/StratumProtocol.cpp arcMiner/ThreadLock.cpp -c -O2
	gcc arcMiner/Crypto/scrypt-linux.c arcMiner/Crypto/sha2-linux.c -c -O2
	g++ -o arcMinerd AppLog.o arcMiner.o Helpers.o NetworkHelpers.o ScryptCPUMiner.o StratumHelpers.o StratumNetwork.o StratumProtocol.o ThreadLock.o scrypt-linux.o arcMiner/Assembly/scrypt-x64.S arcMiner/Assembly/sha2-x64.S sha2-linux.o 
	