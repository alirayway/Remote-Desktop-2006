
/*
 *  Functions for rd password management and authentication.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rdauth.h"
#include "d3des.h"

/*
 *   We use a fixed key to store passwords, since we assume that our local
 *   file system is secure but nonetheless don't want to store passwords
 *   as plaintext.
 */

static unsigned char s_fixedkey[8] = {23,82,107,6,35,78,88,7};

/*
 *   Encrypt a password and store it in a file.
 */
int
rdEncryptPasswd(char *passwd, char *encryptedPasswd)
{
    int i;

    /* pad password with nulls */

    for (i = 0; i < MAXPWLEN; i++) {
	if (i < (int)strlen(passwd)) {
	    encryptedPasswd[i] = passwd[i];
	} else {
	    encryptedPasswd[i] = 0;
	}
    }

    /* Do encryption in-place - this way we overwrite our copy of the plaintext
       password */

    deskey(s_fixedkey, EN0);
    des(encryptedPasswd, encryptedPasswd);

    return 8;
}

/*
 *   Decrypt a password.  Returns a pointer to a newly allocated
 *   string containing the password or a null pointer if the password could
 *   not be retrieved for some reason.
 */
char *
rdDecryptPasswd(char *inouttext)
{
    unsigned char *passwd = (unsigned char *)malloc(9);

    deskey(s_fixedkey, DE1);
    des(inouttext, passwd);

    passwd[8] = 0;

    return (char *)passwd;
}

/*
 *   Generate a set of random bytes for use in challenge-response authentication.
 */
void
rdRandomBytes(unsigned char *where) {
  int i;
  unsigned int seed = (unsigned int) time(0);

  srand(seed);
  for (i=0; i < CHALLENGESIZE; i++) {
    where[i] = (unsigned char)(rand() & 255);    
  }
}

/*
 *   Encrypt some bytes in memory using a password.
 */
void
rdEncryptBytes(unsigned char *where, const char *passwd)
{
    unsigned char key[8];
    int i;

    /* key is simply password padded with nulls */

    for (i = 0; i < 8; i++) {
	if (i < (int)strlen(passwd)) {
	    key[i] = passwd[i];
	} else {
	    key[i] = 0;
	}
    }

    deskey(key, EN0);

    for (i = 0; i < CHALLENGESIZE; i += 8) {
	des(where+i, where+i);
    }
}
