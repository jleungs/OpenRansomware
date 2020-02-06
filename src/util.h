#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#define	PUBKEYSIZE_RSID		3

typedef PVOID BCRYPT_ALG_HANDLE;
typedef PVOID BCRYPT_KEY_HANDLE;

/* Defnitions found in bcrypt.h | https://www.microsoft.com/en-us/download/details.aspx?id=30688 */
#define BCRYPT_PUBLIC_KEY_BLOB      L"PUBLICBLOB"
#define BCRYPT_PRIVATE_KEY_BLOB     L"PRIVATEBLOB"
#define BCRYPT_RSAPUBLIC_BLOB		L"RSAPUBLICBLOB"
#define BCRYPT_RSAPRIVATE_BLOB		L"RSAPRIVATEBLOB"

#define BCRYPT_RSA_ALGORITHM         L"RSA"

void die(const char *errmsg);
FARPROC adrof(HINSTANCE lib, char *funcname);
