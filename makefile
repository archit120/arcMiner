all:
	mkdir build
	cd arcMiner; \
	g++ -std=c++11 -Iinclude AppLog.cpp arcMiner.cpp Helpers.cpp NetworkHelpers.cpp ScryptCPUMiner.cpp StratumHelpers.cpp StratumNetwork.cpp StratumProtocol.cpp ThreadLock.cpp -c -O2; \
	gcc -Iinclude Crypto/scrypt-linux.c Crypto/sha2-linux.c -c; \
	g++ -o arcMiner AppLog.o arcMiner.o Helpers.o NetworkHelpers.o ScryptCPUMiner.o StratumHelpers.o StratumNetwork.o StratumProtocol.o ThreadLock.o Crypto/scrypt-linux.o Assembly/scrypt-x64.S Assembly/sha2-x64.S Crypto/sha2-linux.o 

	cp arcMiner/arcMiner build/arcMiner		