

// rdEncodeZlib object

// The rdEncodeZlib object uses a zlib based compression encoding to send rectangles
// to a client

class rdEncodeZlib;

#if !defined(_WINrd_ENCODEZLIB)
#define _WINrd_ENCODEZLIB
#pragma once

#include "rdEncoder.h"

#include "zlib/zlib.h"

// Minimum zlib rectangle size in bytes.  Anything smaller will
// not compress well due to overhead.
#define rd_ENCODE_ZLIB_MIN_COMP_SIZE (17)

// Set maximum zlib rectangle size in pixels.  Always allow at least
// two scan lines.
#define ZLIB_MAX_RECT_SIZE (8*1024)
#define ZLIB_MAX_SIZE(min) ((( min * 2 ) > ZLIB_MAX_RECT_SIZE ) ? ( min * 2 ) : ZLIB_MAX_RECT_SIZE )


// Class definition

class rdEncodeZlib : public rdEncoder
{
// Fields
public:

// Methods
public:
	// Create/Destroy methods
	rdEncodeZlib();
	~rdEncodeZlib();

	virtual void Init();
	virtual const char* GetEncodingName() { return "Zlib"; }

	virtual UINT RequiredBuffSize(UINT width, UINT height);
	virtual UINT NumCodedRects(RECT &rect);

	virtual UINT EncodeRect(BYTE *source, VSocket *outConn, BYTE *dest, const RECT &rect);
	virtual UINT EncodeOneRect(BYTE *source, BYTE *dest, const RECT &rect);

// Implementation
protected:
	BYTE		      *m_buffer;
	int			       m_bufflen;
	struct z_stream_s  compStream;
	bool               compStreamInited;
};

#endif // _WINrd_ENCODEZLIB

