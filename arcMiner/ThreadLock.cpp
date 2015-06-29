#include "ThreadLock.h"
#include "global.h"

bool ThreadLock::Initialize(ThreadLockSection ths)
{
#if defined(_MSC_VER)
	InitializeCriticalSection(&ths.cs);
#else
	pthread_mutex_init(&ths.cs, NULL);
#endif

	return true;
}

bool ThreadLock::Enter(ThreadLockSection ths)
{
	if (!ths.Initialized)
		return false;
#if defined(_MSC_VER)
	EnterCriticalSection(&ths.cs);
#else
	pthread_mutex_lock(&ths.cs);
#endif

	return true;
}


bool ThreadLock::Leave(ThreadLockSection ths)
{
	if (!ths.Initialized)
		return false;
#if defined(_MSC_VER)
	LeaveCriticalSection(&ths.cs);
#else
	pthread_mutex_unlock(&ths.cs);
#endif
	return true;

}
