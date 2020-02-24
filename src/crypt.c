#include "crypt.h"
#include "util.h"

void
rsa_gen_key(HINSTANCE *hinst, Keypair *k)
{
	BCRYPT_ALG_HANDLE alghandle = 0;
	/* BCryptOpenAlgorithmProvider */
	if (adrof(*hinst,"BCryptOpenAlgorithmProvider")(&alghandle, BCRYPT_RSA_ALGORITHM, 0, 0))
			die("Failed call to BCryptOpenAlgorithmProvider");
	/* BCryptGenerateKeyPair */
	if (adrof(*hinst, "BCryptGenerateKeyPair")(alghandle, &k->khandle, 2048, 0))
			die("Failed call to BCryptGenerateKeyPair");
	/* BCryptFinalizeKeyPair */
	if (adrof(*hinst, "BCryptFinalizeKeyPair")(k->khandle, 0))
			die("Failed call to BCryptFinalizeKeyPair");
	/* BCryptCloseAlgorithmProvider */
	adrof(*hinst, "BCryptCloseAlgorithmProvider")(alghandle, 0);
}

void
ktob(HINSTANCE *hinst, Keypair *k, short unsigned int *blobtype)
{
	/* BCryptExportKey */
    if (adrof(*hinst, "BCryptExportKey")(k->khandle, 0, blobtype, 0, 0, &k->size, 0))
        die("Failed to call BCryptExportKey");
    if (!(k->blob = malloc(k->size)))
        die("Failed to malloc keyblob");
    /* BCryptExportKey */
    if (adrof(*hinst, "BCryptExportKey")(k->khandle, 0, blobtype, k->blob, k->size, &k->size, 0))
        die("Failed to call BCryptExportKey");
}
