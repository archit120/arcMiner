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

###how to use
- -o, -O                        The miner will connect to this url. You can specify a port after the url using -o url:port
- -u                            The username (workername) used for login
- -p   							The password used for login
- -t <num>                      The number of threads for mining (default is set to number of cores)
-  -a <algo>                     Name of the mining POW in case of stratum server. Possible values - scrypt
- -pr <protocol>                Server connection protocol to be used. Possible values - stratum

Example usage:
	arcMiner.exe -o http://poolurl.com:10034 -u workername.pts_1 -p workerpass -t 4 -pr stratum -a scrypt

Credits: archit, pooler(scrypt mining code), jh00(inspired from xptMiner)

###How to compile
The project was built using Visual Studio 2013, it may compile in earlier versions but I can not gaurante. Assembly modules have been compiled using GCC on MingW64. Also rapidjson is needed as an additional dependency.

###License
The files under crypto folder originally belong to pooler's cpuminer and hence are covered under GPLv2. Everything else can be used in any way you like, just give me the due credits. 







