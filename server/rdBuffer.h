
// rdBuffer object

// The rdBuffer object provides a client-local copy of the screen
// It can tell the client which bits have changed in a given region
// It uses the specified rdDesktop to read screen data from

class rdBuffer;

#if !defined(_WINrd_rdBUFFER)
#define _WINrd_rdBUFFER
#pragma once

// Includes

#include "rdDesktop.h"
#include "rdEncoder.h"
#include "rdRegion.h"
#include "RectList.h"

// Class definition

class rdBuffer
{
// Methods
public:
	// Create/Destroy methods
	rdBuffer(rdDesktop *desktop);
	~rdBuffer();

	// BUFFER INFO
	RECT GetSize();
	rfbPixelFormat GetLocalFormat();
	BYTE *GetClientBuffer();
	UINT GetClientBuffSize();
	BOOL GetRemotePalette(RGBQUAD *quadbuff, UINT ncolours);

	// BUFFER MANIPULATION
	BOOL CheckBuffer();

	// SCREEN SCANNING
	void Clear(RECT &rect);
	void GetChangedRegion(rdRegion &rgn, RECT &rect);											
	UINT GetNumCodedRects(RECT &rect);

	// SCREEN CAPTURE
	void GrabRect(RECT &rect);
	void CopyRect(RECT &dest, POINT &source);
	RECT GrabMouse();
	BOOL SetClientFormat(rfbPixelFormat &format);

	// CONFIGURING ENCODER
	void SetCompressLevel(int level);
	void SetQualityLevel(int level);
	void EnableXCursor(BOOL enable);
	void EnableRichCursor(BOOL enable);
	void EnableLastRect(BOOL enable);
	BOOL IsLastRectEnabled() { return m_use_lastrect; }

	// ENCODING
	BOOL SetEncoding(CARD32 encoding);
	UINT TranslateRect(const RECT &rect, VSocket *outConn);

	// SENDING CURSOR SHAPE UPDATES
	BOOL IsCursorUpdatePending();
	BOOL SendCursorShape(VSocket *outConn);
	BOOL SendEmptyCursorShape(VSocket *outConn);

// Implementation
protected:

	// Routine to verify the mainbuff handle hasn't changed
	inline BOOL FastCheckMainbuffer();
	
	BYTE		*m_mainbuff;
	BOOL		m_freemainbuff;
	BYTE		*m_backbuff;
	UINT		m_mainsize;

	BYTE		*m_clientbuff;
	UINT		m_clientbuffsize;
	BOOL		m_clientfmtset;

	UINT		m_bytesPerRow;

	rfbServerInitMsg	m_scrinfo;
	rfbPixelFormat		m_clientformat;
	rfbTranslateFnType	m_transfunc;

	rdDesktop	   *m_desktop;
	rdEncoder	   *m_encoder;
	bool           zlib_encoder_in_use;
	rdEncoder     *m_hold_zlib_encoder;
	bool           tight_encoder_in_use;
	rdEncoder     *m_hold_tight_encoder;
	bool           zlibhex_encoder_in_use;
	rdEncoder     *m_hold_zlibhex_encoder;

	// These variables mirror similar variables from rdEncoder class.
	// They are necessary because rdEncoder instance may be created after
	// their values were set.
	int				m_compresslevel;
	int				m_qualitylevel;
	BOOL			m_use_xcursor;
	BOOL			m_use_richcursor;
	BOOL			m_use_lastrect;

	HCURSOR			m_hcursor;		// Used to track cursor shape changes
};

#endif // _WINrd_rdBUFFER
