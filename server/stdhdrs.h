

// #define VC_EXTRALEAN
#define STRICT

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <crtdbg.h>

// LOGGING SUPPORT

#include "Log.h"
extern Log rdlog;

// No logging at all
#define LL_NONE		0
// Log server startup/shutdown
#define LL_STATE	0
// Log connect/disconnect
#define LL_CLIENTS	1
// Log connection errors (wrong pixfmt, etc)
#define LL_CONNERR	0
// Log socket errors
#define LL_SOCKERR	4
// Log internal errors
#define LL_INTERR	0

// Log internal warnings
#define LL_INTWARN	8
// Log internal info
#define LL_INTINFO	9
// Log socket errors
#define LL_SOCKINFO	10
// Log everything, including internal table setup, etc.
#define LL_ALL		10

// Macros for sticking in the current file name
#define rdLOG(s)	(__FILE__ " : " s)
