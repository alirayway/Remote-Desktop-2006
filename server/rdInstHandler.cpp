

// rdInstHandler.cpp

// Implementation of the class used to ensure that only
// one instance is running

#include "stdhdrs.h"
#include "rdInstHandler.h"

// Name of the mutex

const char mutexname [] = "Winrd_Win32_Instance_Mutex";

// The class methods

BOOL
rdInstHandler::Init()
{
	// Create the named mutex
	HANDLE mutex = CreateMutex(NULL, FALSE, mutexname);
	if (mutex == NULL)
		return FALSE;

	// Check that the mutex didn't already exist
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return FALSE;

	return TRUE;
}
