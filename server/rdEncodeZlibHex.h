

// rdEncodeZlibHex object

// The rdEncodeZlibHex object uses a compression encoding to send rectangles
// to a client.  As with the hextile encoding, all rectangles are broken down
// into a matrix of 16x16 (or smaller at bottom/right) tiles, which are 
// individually encoded with a subencoding mechanism.  This encoding addds
// the ability to apply zlib compression to the raw and other hextile
// subencodings.

class rdEncodeZlibHex;

#if !defined(_WINrd_ENCODEZLIBHEX)
#define _WINrd_ENCODEZLIBHEX
#pragma once

#include "rdEncoder.h"

#include "zlib/zlib.h"

// Minimum zlib rectangle size in bytes.  Anything smaller will
// not compress well due to overhead.
#define rd_ENCODE_ZLIBHEX_MIN_COMP_SIZE (17)

// Flag used to mark our compressors as uninitialized.
#define ZLIBHEX_COMP_UNINITED (-1)

// Size of the smallest update portion sent independently across
// the network.  This encoder can transmit partial updates to
// improve latency issues with performance.
#define rd_ENCODE_ZLIBHEX_MIN_DATAXFER (1400)

// Class definition

class rdEncodeZlibHex : public rdEncoder
{
// Fields
public:

// Methods
public:
	// Create/Destroy methods
	rdEncodeZlibHex();
	~rdEncodeZlibHex();

	void Init();
	virtual const char* GetEncodingName() { return "ZlibHex"; }

	virtual UINT RequiredBuffSize(UINT width, UINT height);
	virtual UINT NumCodedRects(RECT &rect);

	// virtual UINT EncodeRect(BYTE *source, BYTE *dest, const RECT &rect);
	virtual UINT EncodeRect(BYTE *source, VSocket *outConn, BYTE *dest, const RECT &rect);

protected:
	virtual UINT zlibCompress(BYTE *from_buf, BYTE *to_buf, UINT length, struct z_stream_s *compressor);

	virtual UINT EncodeHextiles8(BYTE *source, BYTE *dest,
		VSocket *outConn, int x, int y, int w, int h);
	virtual UINT EncodeHextiles16(BYTE *source, BYTE *dest,
		VSocket *outConn, int x, int y, int w, int h);
	virtual UINT EncodeHextiles32(BYTE *source, BYTE *dest,
		VSocket *outConn, int x, int y, int w, int h);

// Implementation
protected:
	BYTE		      *m_buffer;
	int			       m_bufflen;
	struct z_stream_s  compStreamRaw;
	struct z_stream_s  compStreamEncoded;
};

#endif // _WINrd_ENCODEHEXTILE

