#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

void die(const char *errmsg);
char *alphabet_table_lookup(const int indx[], int arrsize);
FARPROC adrof(HINSTANCE lib, char *funcname);
