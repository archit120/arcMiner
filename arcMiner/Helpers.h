#ifndef _H_HELPERS
#define _H_HELPERS

#include "global.h"

class Helpers
{
public:
	static bool PrintBinaryInHex(char* binary, size_t len);
	static bool HexToBinary(char* hex, char* binary, size_t len);
	static bool BinaryToHex(char* hex, char* binary, size_t len);
	static uint32_t BigEndian32Decode(const void *buf);
	static bool GenerateExtraNonce( char* nonce, size_t len);
};

#endif