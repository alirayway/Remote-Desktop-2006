

// rdEncoder object

// The rdEncoder object encodes regions of a display buffer for sending
// to a client

class rdEncoder;

#if !defined(RFBENCODER_DEFINED)
#define RFBENCODER_DEFINED
#pragma once

#include "rdBuffer.h"
#include "translate.h"

// Class definition

class rdEncoder
{
// Fields
public:

// Methods
public:
	// Create/Destroy methods
	rdEncoder();
	virtual ~rdEncoder();

	// Initialisation
	virtual void Init();

	// A method to return the encoding name, used by the LogStats() method
	virtual const char* GetEncodingName() { return "Raw"; }

	// Central method for outputing encoding statistics
	virtual void LogStats();

	// Encoder stats used by the buffer object
	virtual UINT RequiredBuffSize(UINT width, UINT height);
	virtual UINT NumCodedRects(RECT &rect);

	// Translation & encoding routines
	//  - Source is the base address of the ENTIRE SCREEN buffer.
	//    The Translate routine compensates automatically for the desired rectangle.
	//  - Dest is the base address to encode the rect to.  The rect will be encoded
	//    into a contiguous region of the buffer.
	virtual void Translate(BYTE *source, BYTE *dest, const RECT &rect);
	virtual UINT EncodeRect(BYTE *source, BYTE *dest, const RECT &rect);
	virtual UINT EncodeRect(BYTE *source, VSocket *outConn, BYTE *dest, const RECT &rect);

	// Additional translation function for cursor shape data (overloaded!)
	void Translate(BYTE *source, BYTE *dest, int w, int h, int bytesPerRow);

	// Translation handling
	BOOL SetLocalFormat(rfbPixelFormat &pixformat, int width, int height);
	BOOL SetRemoteFormat(rfbPixelFormat &pixformat);

	// Configuring encoder
	void SetCompressLevel(int level);
	void SetQualityLevel(int level);
	void EnableXCursor(BOOL enable) { m_use_xcursor = enable; }
	void EnableRichCursor(BOOL enable) { m_use_richcursor = enable; }
	void EnableLastRect(BOOL enable) { m_use_lastrect = enable; }

	// Colour map handling
	BOOL GetRemotePalette(RGBQUAD *quadlist, UINT ncolours);

	// Supporting cursor shape updates
	BOOL SendEmptyCursorShape(VSocket *outConn);
	BOOL SendCursorShape(VSocket *outConn, rdDesktop *desktop);

protected:
	BOOL SetTranslateFunction();

	// Supporting cursor shape updates
	BOOL SendXCursorShape(VSocket *outConn, BYTE *mask, int xhot,int yhot,int width,int height);
	BOOL SendRichCursorShape(VSocket *outConn, BYTE *mbits, BYTE *cbits, int xhot,int yhot,int width,int height);
	void FixCursorMask(BYTE *mbits, BYTE *cbits, int width, int height, int width_bytes);

// Implementation
protected:
	rfbTranslateFnType	m_transfunc;			// Translator function
	char*				m_transtable;			// Colour translation LUT
	char*				m_localpalette;			// Palette info if client is palette-based
	rfbPixelFormat		m_localformat;			// Pixel Format info
	rfbPixelFormat		m_remoteformat;			// Client pixel format info
	rfbPixelFormat		m_transformat;			// Internal format used for translation (usually == client format)
	int					m_bytesPerRow;			// Number of bytes per row locally
	int					dataSize;				// Total size of raw data encoded
	int					rectangleOverhead;		// Total size of rectangle header data
	int					encodedSize;			// Total size of encoded data
	int					transmittedSize;		// Total amount of data sent

	int					m_compresslevel;		// Encoding-specific compression level (if needed).
	int					m_qualitylevel;			// Image quality level for lossy JPEG compression.
	BOOL				m_use_xcursor;			// XCursor cursor shape updates allowed.
	BOOL				m_use_richcursor;		// RichCursor cursor shape updates allowed.
	BOOL				m_use_lastrect;			// LastRect pseudo-encoding allowed.
};

#endif // rdENCODER_DEFINED
