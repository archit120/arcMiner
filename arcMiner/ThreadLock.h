#ifndef _H_THREADLOCK
#define _H_THREADLOCK

#include "global.h"

class ThreadLock
{
public:
	static bool Initialize(ThreadLockSection ths);
	static bool Enter(ThreadLockSection ths);
	static bool Leave(ThreadLockSection ths);
};


#endif