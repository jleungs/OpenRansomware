#ifndef WIN_H
#define WIN_H
#include <windows.h>
#endif
#include <stdlib.h>
#include <stdio.h>

void die(const char *errmsg);
FARPROC adrof(HINSTANCE lib, char *funcname);
