#include "util.h"

const char alphabet_table[] = {
	/* To not have strings in binary, use indexof.py to lookup */
  /* 0    1    2    3    4    5    6    7    8    9    10   11   12 */
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  /* 13   14   15   16   17   18   19   20   21   22   23   24   25 */
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  /* 26   27   28   29   30   31   32   33   34   35   36   37   38 */
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  /* 39   40   41   42   43   44   45   46   47   48   49   50   51 */
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  /* 52   53   54   55   56   57   58   59   60   61   62*/
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'
};

char *
alphabet_table_lookup(const int indx[], int arrsize)
{
    int i;
    char *s;
    /* to get size of array */
    for (i=0; i < arrsize; i++)
        s[i] = alphabet_table[indx[i]];
    s[i] = '\0';
    return s;
}

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

