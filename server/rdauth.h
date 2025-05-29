

/* 
 * rdauth.h - describes the functions provided by the rdauth library.
 */

#define MAXPWLEN 8
#define CHALLENGESIZE 16

extern int rdEncryptPasswd(char *passwd, char *fname);
extern char *rdDecryptPasswd(char *fname);
extern void rdRandomBytes(unsigned char *bytes);
extern void rdEncryptBytes(unsigned char *bytes, const char *passwd);
