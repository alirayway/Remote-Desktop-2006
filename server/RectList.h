

// RectList header file

#include "stdhdrs.h"
#include <list>

// The rectlist class
typedef std::list<RECT> rectlist; 

// Operators on RECTs.
bool operator <(const RECT _X, const RECT _Y);
bool operator >(const RECT _X, const RECT _Y);
bool operator ==(const RECT _X, const RECT _Y);
bool operator !=(const RECT _X, const RECT _Y);
