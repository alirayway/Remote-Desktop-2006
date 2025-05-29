
// rdEncodeCoRRE object

// The rdEncodeCoRRE object uses a compression encoding to send rectangles
// to a client

class rdEncodeCoRRE;

#if !defined(_WINrd_ENCODECORRRE)
#define _WINrd_ENCODECORRE
#pragma once

#include "rdEncoder.h"

// Class definition

class rdEncodeCoRRE : public rdEncoder
{
// Fields
public:

// Methods
public:
	// Create/Destroy methods
	rdEncodeCoRRE();
	~rdEncodeCoRRE();

	virtual void Init();
	virtual const char* GetEncodingName() { return "CoRRE"; }

	virtual UINT RequiredBuffSize(UINT width, UINT height);
	virtual UINT NumCodedRects(RECT &rect);

	virtual UINT EncodeRect(BYTE *source, BYTE *dest, const RECT &rect);
	virtual void SetCoRREMax(BYTE width, BYTE height);
protected:
	virtual UINT InternalEncodeRect(BYTE *source, BYTE *dest, const RECT &rect);
	virtual UINT EncodeSmallRect(BYTE *source, BYTE *dest, const RECT &rect);

// Implementation
protected:
	BYTE		*m_buffer;
	size_t		m_bufflen;

	// Maximum height & width for CoRRE
	int			m_maxwidth;
	int			m_maxheight;

	// Last-update stats for CoRRE
	UINT		m_encodedbytes, m_rectbytes;
	UINT		m_lastencodedbytes, m_lastrectbytes;
	int			m_maxadjust;
	int			m_threshold;
	BOOL		m_statsready;
};

#endif // _WINrd_ENCODECORRE

