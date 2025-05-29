

// rdEncodeRRE object

// The rdEncodeRRE object uses a compression encoding to send rectangles
// to a client

class rdEncodeRRE;

#if !defined(_WINrd_ENCODERRRE)
#define _WINrd_ENCODERRE
#pragma once

#include "rdEncoder.h"

// Class definition

class rdEncodeRRE : public rdEncoder
{
// Fields
public:

// Methods
public:
	// Create/Destroy methods
	rdEncodeRRE();
	~rdEncodeRRE();

	virtual void Init();
	virtual const char* GetEncodingName() { return "RRE"; }

	virtual UINT RequiredBuffSize(UINT width, UINT height);
	virtual UINT NumCodedRects(RECT &rect);

	virtual UINT EncodeRect(BYTE *source, BYTE *dest, const RECT &rect);

// Implementation
protected:
	BYTE		*m_buffer;
	size_t		m_bufflen;
};

#endif // _WINrd_ENCODERRE

