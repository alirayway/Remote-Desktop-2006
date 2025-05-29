
// Some routines used internally to minimise and maximise integers
#include "MinMax.h"

inline int Max(int x, int y)
{
	if (x>y)
		return x;
	else
		return y;
}

inline int Min(int x, int y)
{
	if (x>y)
		return y;
	else
		return x;
}
