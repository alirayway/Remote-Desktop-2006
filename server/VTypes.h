
// VTypes.h

// RFB V3.0

// Datatypes used by the VGui system

#if (!defined(_ATT_VTYPES_DEFINED))
#define _ATT_VTYPES_DEFINED

////////////////////////////
// Numeric data types

////////////////////////////
// Fixed size (derived from rfb.h)

typedef unsigned int VCard32;
typedef unsigned short VCard16;
typedef unsigned char VCard8;
typedef int VInt32;
typedef short VInt16;
typedef char VInt8;

////////////////////////////
// Variable size
//		These will always be at least as big as the largest
//		fixed-size data-type

typedef VCard32 VCard;
typedef VInt32 VInt;

////////////////////////////
// Useful functions on integers

static inline VInt Max(VInt x, VInt y) {if (x>y) return x; else return y;}
static inline VInt Min(VInt x, VInt y) {if (x<y) return x; else return y;}

////////////////////////////
// Boolean

typedef int VBool;
const VBool VTrue = -1;
const VBool VFalse = 0;

////////////////////////////
// Others

typedef char VChar;
#if (!defined(NULL))
#define NULL 0
#endif

////////////////////////////
// Compound data types

// #include "rfbgui/VPoint.h"
// #include "rfbgui/VRect.h"
typedef VChar * VString;

#endif // _ATT_VTYPES_DEFINED





