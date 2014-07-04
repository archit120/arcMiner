#ifndef _H_STRATUM_HELPERS
#define _H_STRATUM_HELPERS

#include "global.h"

class StratumHelpers
{
public:
	static Target TargetFromDifficulty(double difficulty);
	static Target TargetFromDifficulty(double difficulty, Algorithms algorithm);
	static string GenerateLoginString();
	static string GenerateLoginString(MinerClient client);

};

#endif