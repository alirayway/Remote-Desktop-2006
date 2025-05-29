

// rdRegion object

// The rdRegion object turns a number of rectangular regions
// into a list of distinct, non-overlapping regions.

class rdRegion;

#if !defined(rdREGION_DEFINED)
#define rdREGION_DEFINED
#pragma once

#include "stdhdrs.h"
#include <list>
#include "RectList.h"

// Class definition
class rdRegion
{

// Fields
public:

// Methods
public:
	// Create/Destroy methods
	rdRegion();
	~rdRegion();

	void AddRect(RECT &rect);				// Add another rectangle to the regions
	void SubtractRect(RECT &rect);			// Subtract a rectangle from the regions
	void Clear();							// Clear the current set of rectangles
	inline BOOL IsEmpty() {					// Is the region empty?
		return region == NULL;
	}
	void Combine(rdRegion &rgn);			// Combine with another region
	void Intersect(rdRegion &rgn);			// Intersect with another region
	void Subtract(rdRegion &rgn);			// Subtract another region from this one

	// Rectangle retrieval routines - return FALSE if no rects returned!
	// Note that these routines ADD rectangles to existing lists...
	BOOL Rectangles(rectlist &rects);					// Just return the rects
	BOOL Rectangles(rectlist &rects, RECT &cliprect);	// Return all rects within the clip region	

// Implementation
protected:
	HRGN region;							// Region used internally
};

#endif // rdREGION_DEFINED
