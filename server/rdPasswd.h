

// rdPasswd

// This header provides helpers for handling encrypted password data.
// The password handling routines found in rdauth.h should not be used directly

class rdPasswd;

#if (!defined(_WINrd_rdPASSWD))
#define _WINrd_rdPASSWD

#include "stdhdrs.h"
extern "C" {
#include "rdauth.h"
}

// Password handling helper class
class rdPasswd
{
public:

    // Password decryptor!
    class ToText
    {
    public:
	inline ToText(const char encrypted[MAXPWLEN])
	{
	    rdlog.Print(LL_INTINFO, rdLOG("PASSWD : ToText called\n"));
	    plaintext = rdDecryptPasswd((char *)encrypted);
	}
	inline ~ToText()
	{
	    if (plaintext != NULL)
	    {
		ZeroMemory(plaintext, strlen(plaintext));
		free(plaintext);
	    }
	}
	inline operator const char*() const {return plaintext;};
    private:
	char *plaintext;
    };

    class FromText
    {
    public:
	inline FromText(char *unencrypted)
	{
	    rdlog.Print(LL_INTINFO, rdLOG("PASSWD : FromText called\n"));
	    rdEncryptPasswd(unencrypted, encrypted);
	    ZeroMemory(unencrypted, strlen(unencrypted));
	}
	inline ~FromText()
	{
	}
	inline operator const char*() const {return encrypted;};
    private:
	char encrypted[MAXPWLEN];
    };

    class FromClear
    {
    public:
	inline FromClear()
	{
	    rdlog.Print(LL_INTINFO, rdLOG("PASSWD : FromClear called\n"));
	    rdEncryptPasswd("", encrypted);
	}
	inline ~FromClear()
	{
	}
	inline operator const char*() const {return encrypted;};
    private:
	char encrypted[MAXPWLEN];
    };
};

#endif
