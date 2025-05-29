

/* 
 * rdauth.h - describes the functions provided by the rdauth library.
 */

#define MAXPWLEN 8
#define CHALLENGESIZE 16

void rdEncryptBytes(unsigned char *bytes, char *passwd);
void rdEncryptPasswd(unsigned char *encryptedPasswd, char *passwd);
char *rdDecryptPasswd(const unsigned char *encryptedPasswd);


