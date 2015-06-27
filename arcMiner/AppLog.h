#ifndef _H_APPLOG
#define _H_APPLOG

#include "global.h"

enum AppLogLevel{
	General
};

class AppLog
{
public:
	static bool Log(AppLogLevel level, char* string, ...);
};

#endif