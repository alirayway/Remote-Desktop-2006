

// rdEncodeHexT object

// The rdEncodeHexT object uses a compression encoding to send rectangles
// to a client

class rdEncodeHexT;

#if !defined(_WINrd_ENCODEHEXTILE)
#define _WINrd_ENCODEHEXTILE
#pragma once

#include "rdEncoder.h"

// Class definition

class rdEncodeHexT : public rdEncoder
{
// Fields
public:

// Methods
public:
	// Create/Destroy methods
	rdEncodeHexT();
	~rdEncodeHexT();

	virtual void Init();
	virtual const char* GetEncodingName() { return "Hextile"; }

	virtual UINT RequiredBuffSize(UINT width, UINT height);
	virtual UINT NumCodedRects(RECT &rect);

	virtual UINT EncodeRect(BYTE *source, BYTE *dest, const RECT &rect);

protected:
	virtual UINT EncodeHextiles8(BYTE *source, BYTE *dest,
		int x, int y, int w, int h);
	virtual UINT EncodeHextiles16(BYTE *source, BYTE *dest,
		int x, int y, int w, int h);
	virtual UINT EncodeHextiles32(BYTE *source, BYTE *dest,
		int x, int y, int w, int h);

// Implementation
protected:
};

#endif // _WINrd_ENCODEHEXTILE

