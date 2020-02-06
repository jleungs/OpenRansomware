#include "util.h"

FARPROC
adrof(HINSTANCE lib, char *funcname)
{
	FARPROC f;
	if (!(f = GetProcAddress(lib, funcname))) {
		char err[100];
		snprintf(err, 100, "Failed to GetProcAddress of %s", funcname);
		die(err);
	}
	return f;
}

void
die(const char *errmsg)
{
        fprintf(stderr, "%s\nGetLastError(): %X\n", errmsg, GetLastError());
        exit(-1);
}

