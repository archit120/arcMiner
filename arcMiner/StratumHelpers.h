#ifndef _H_STRATUM_HELPERS
#define _H_STRATUM_HELPERS

#include "global.h"

class StratumHelpers
{
public:
	static Target TargetFromDifficulty(double difficulty);
	static Target TargetFromDifficulty(double difficulty, Algorithms algorithm);
	static bool GenerateLoginString(string& s);
	static bool GenerateLoginString(MinerClient client, string& s);

};

#endif