

// rdRegion implementation
// This implementation uses the system region handling routines
// to speed things up and give the best results

#include "stdhdrs.h"

// Header

#include "rdRegion.h"

// Implementation

rdRegion::rdRegion()
{
	region = NULL;
}

rdRegion::~rdRegion()
{
	Clear();
}

void rdRegion::AddRect(RECT &new_rect)
{
	HRGN newregion;

	if (region == NULL)
	{
		// Create the region and set it to contain this rectangle
		region = CreateRectRgnIndirect(&new_rect);
	}
	else
	{
		// Create a new region containing the appropriate rectangle
		newregion = CreateRectRgnIndirect(&new_rect);

		// Merge it into the existing region
		if (CombineRgn(region, region, newregion, RGN_OR) == NULLREGION)
			Clear();

		// Now delete the temporary region
		DeleteObject(newregion);
	}
}

void rdRegion::SubtractRect(RECT &new_rect)
{
	HRGN newregion;

	if (region == NULL)
		return;

	// Create a new region containing the appropriate rectangle
	newregion = CreateRectRgnIndirect(&new_rect);

	// Remove it from the existing region
	if (CombineRgn(region, region, newregion, RGN_DIFF) == NULLREGION)
		Clear();

	// Now delete the temporary region
	DeleteObject(newregion);
}

void rdRegion::Clear()
{
	// Set the region to be empty
	if (region != NULL)
	{
		DeleteObject(region);
		region = NULL;
	}
}

void
rdRegion::Combine(rdRegion &rgn)
{
	if (rgn.region == NULL)
		return;
	if (region == NULL)
	{
		region = CreateRectRgn(0, 0, 0, 0);
		if (region == NULL)
			return;

		// Copy the specified region into this one...
		if (CombineRgn(region, rgn.region, 0, RGN_COPY) == NULLREGION)
			Clear();
		return;
	}

	// Otherwise, combine the two
	if (CombineRgn(region, region, rgn.region, RGN_OR) == NULLREGION)
		Clear();
}

void
rdRegion::Intersect(rdRegion &rgn)
{
	if (rgn.region == NULL)
		return;
	if (region == NULL)
		return;

	// Otherwise, intersect the two
	if (CombineRgn(region, region, rgn.region, RGN_AND) == NULLREGION)
		Clear();
}

void
rdRegion::Subtract(rdRegion &rgn)
{
	if (rgn.region == NULL)
		return;
	if (region == NULL)
		return;

	// Otherwise, intersect the two
	if (CombineRgn(region, region, rgn.region, RGN_DIFF) == NULLREGION)
		Clear();
}



// Return all the rectangles
BOOL rdRegion::Rectangles(rectlist &rects)
{
	int buffsize;
	DWORD x;
	RGNDATA *buff;

	// If the region is empty then return empty rectangle list
	if (region == NULL)
		return FALSE;

	// Get the size of buffer required
	buffsize = GetRegionData(region, NULL, 0);
	buff = (RGNDATA *) new BYTE [buffsize];
	if (buff == NULL)
		return FALSE;

	// Now get the region data
	if (GetRegionData(region, buffsize, buff))
	{
		for (x=0; x<(buff->rdh.nCount); x++)
		{
			// Obtain the rectangles from the list
			RECT *rect = (RECT *) (((BYTE *) buff) + sizeof(RGNDATAHEADER) + x * sizeof(RECT));
			rects.push_front(*rect);
		}
	}

	// Delete the temporary buffer
	delete [] buff;

	// Return whether there are any rects!
	return !rects.empty();
}

// Return rectangles clipped to a certain area
BOOL rdRegion::Rectangles(rectlist &rects, RECT &cliprect)
{
	rdRegion cliprgn;

	// Create the clip-region
	cliprgn.AddRect(cliprect);

	// Calculate the intersection with this region
	cliprgn.Intersect(*this);

	return cliprgn.Rectangles(rects);
}
