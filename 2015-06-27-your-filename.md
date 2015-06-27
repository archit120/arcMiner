## arcMiner - Mutli algo, multi protocol miner

arcMiner is a new miner built form the base up in C++. It has been designed to be as clean as possible so that it's easy for other people to build miners upon and to increase reliability in hte long term. Right now it only supports scrypt on stratum but hopefully in future, many more POWs and connection protocols

###Compatible OS - Windows x64

###features
- fast 64 bit scrypt mining using pooler's cpuminer code
- connection to stratum servers

###todo
- support xpt
- support momentum,x11,keccak etc
- proper logging support

Credits: archit, pooler(scrypt mining code), jh00(inspired from xptMiner)

###How to compile
The project was built using Visual Studio 2013, it may compile in earlier versions but I can not gaurante. Assembly modules have been compiled using GCC on MingW64. Also rapidjson is needed as an additional dependency.

###License
The files under crypto folder originally belong to pooler's cpuminer and hence are covered under GPLv2. Everything else can be used in any way you like, just give me the due credits. 







