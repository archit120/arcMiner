#include "AppLog.h"

bool AppLog::Log(AppLogLevel level, char* string, ...)
{
	int count;
	/*va_list ap;
	char st[1024 * 5];
	va_start(ap, string);
	count = snprintf(st, strlen(string), string, ap);
	va_end(ap);
	printf("[%s]: %s\n", "time", st);*/
	return true;
}
