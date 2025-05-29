
// ScrBuffer implementation

#include "stdhdrs.h"

// Header

#include "rdDesktop.h"
#include "rdEncoder.h"
#include "rdEncodeRRE.h"
#include "rdEncodeCoRRE.h"
#include "rdEncodeHexT.h"
#include "rdEncodeZlib.h"
#include "rdEncodeTight.h"
#include "rdEncodeZlibHex.h"
#include "MinMax.h"

#include "rdBuffer.h"

// Implementation

rdBuffer::rdBuffer(rdDesktop *desktop)
{
	m_desktop = desktop;
	m_freemainbuff = FALSE;
	m_encoder = NULL;
	zlib_encoder_in_use = false;
	m_hold_zlib_encoder = NULL;
	tight_encoder_in_use = false;
	m_hold_tight_encoder = NULL;
	zlibhex_encoder_in_use = false;
	m_hold_zlibhex_encoder = NULL;

	m_compresslevel = 6;
	m_qualitylevel = -1;
	m_use_xcursor = FALSE;
	m_use_richcursor = FALSE;
	m_use_lastrect = FALSE;

	m_hcursor = NULL;

	m_mainbuff = NULL;
	m_backbuff = NULL;
	m_mainsize = 0;
	
	m_clientbuff = NULL;
	m_clientbuffsize = 0;
	m_clientfmtset = FALSE;

	// Initialise the screen buffers
	CheckBuffer();
}

rdBuffer::~rdBuffer()
{

	if (m_freemainbuff && m_mainbuff != NULL) {
		// We need to free the slow-blit buffer
		delete [] m_mainbuff;
		m_mainbuff = NULL;
	}
	if (m_backbuff != NULL) {
		delete [] m_backbuff;
		m_backbuff = NULL;
	}
	if (m_hold_zlib_encoder != NULL && m_hold_zlib_encoder != m_encoder) {
		m_hold_zlib_encoder->LogStats();
		delete m_hold_zlib_encoder;
		m_hold_zlib_encoder = NULL;
	}
	if (m_hold_tight_encoder != NULL && m_hold_tight_encoder != m_encoder) {
		m_hold_tight_encoder->LogStats();
		delete m_hold_tight_encoder;
		m_hold_tight_encoder = NULL;
	}
	if (m_hold_zlibhex_encoder != NULL && m_hold_zlibhex_encoder != m_encoder) {
		m_hold_zlibhex_encoder->LogStats();
		delete m_hold_zlibhex_encoder;
		m_hold_zlibhex_encoder = NULL;
	}
	if (m_encoder != NULL) {
		m_encoder->LogStats();
		delete m_encoder;
		m_encoder = NULL;
		m_hold_zlib_encoder = NULL;
		m_hold_tight_encoder = NULL;
		m_hold_zlibhex_encoder = NULL;
	}
	if (m_clientbuff != NULL) {
		delete m_clientbuff;
		m_clientbuff = NULL;
	}
	m_clientbuffsize = 0;
	m_mainsize = 0;
}

RECT
rdBuffer::GetSize()
{
	RECT rect;

	rect.left = 0;
	rect.top = 0;
	rect.right = m_scrinfo.framebufferWidth;
	rect.bottom = m_scrinfo.framebufferHeight;

	return rect;
}

rfbPixelFormat
rdBuffer::GetLocalFormat()
{
	return m_scrinfo.format;
}

BYTE *
rdBuffer::GetClientBuffer()
{
	return m_clientbuff;
}

BOOL
rdBuffer::GetRemotePalette(RGBQUAD *quadlist, UINT ncolours)
{
	// Try to get the RGBQUAD data from the encoder
	// This will only work if the remote client is palette-based,
	// in which case the encoder will be storing RGBQUAD data
	if (m_encoder == NULL)
	{
		rdlog.Print(LL_INTWARN, rdLOG("GetRemotePalette called but no encoder set\n"));
		return FALSE;
	}

	// Now get the palette data
	return m_encoder->GetRemotePalette(quadlist, ncolours);
}

BOOL
rdBuffer::CheckBuffer()
{
	// Get the screen format, in case it has changed
	m_desktop->FillDisplayInfo(&m_scrinfo);

	// If the client has not specified a pixel format then set one for it
	if (!m_clientfmtset) {
	    m_clientfmtset = TRUE;
	    m_clientformat = m_scrinfo.format;
	}

	// If the client has not selected an encoding then set one for it
	if (m_encoder == NULL) {
	    if (!SetEncoding(rfbEncodingRaw))
		return FALSE;
	}

	m_bytesPerRow = m_scrinfo.framebufferWidth * m_scrinfo.format.bitsPerPixel/8;

	// Check the client buffer is sufficient
	const UINT clientbuffsize =
	    m_encoder->RequiredBuffSize(m_scrinfo.framebufferWidth,
					m_scrinfo.framebufferHeight);
	if (m_clientbuffsize != clientbuffsize)
	{
	    if (m_clientbuff != NULL)
	    {
		delete [] m_clientbuff;
		m_clientbuff = NULL;
	    }
	    m_clientbuffsize = 0;

	    m_clientbuff = new BYTE [clientbuffsize];
	    if (m_clientbuff == NULL)
	    {		
		rdlog.Print(LL_INTERR, rdLOG("unable to allocate client buffer[%d]\n"), clientbuffsize);
		return FALSE;
	    }

	    m_clientbuffsize = clientbuffsize;

	    ZeroMemory(m_clientbuff, m_clientbuffsize);
	}

	// Check that the local format buffers are sufficient
	if ((m_mainsize != m_desktop->ScreenBuffSize()) || !m_freemainbuff)
	{
		if (m_freemainbuff) {
			// Slow blits were enabled - free the slow blit buffer
			if (m_mainbuff != NULL)
			{
				delete [] m_mainbuff;
				m_mainbuff = NULL;
			}
		}
		if (m_backbuff != NULL)
		{
			delete [] m_backbuff;
			m_backbuff = NULL;
		}
		m_mainsize = 0;

		// Check whether or not the rdDesktop is using fast blits
		m_mainbuff = (BYTE *)m_desktop->OptimisedBlitBuffer();
		if (m_mainbuff) {
			// Prevent us from freeing the DIBsection buffer
			m_freemainbuff = FALSE;
			rdlog.Print(LL_INTINFO, rdLOG("fast blits detected - using DIBsection buffer\n"));
		} else {
			// Create our own buffer to copy blits through
			m_freemainbuff = TRUE;
			if ((m_mainbuff = new BYTE [m_desktop->ScreenBuffSize()]) == NULL)
			{
				rdlog.Print(LL_INTERR, rdLOG("unable to allocate main buffer[%d]\n"), m_desktop->ScreenBuffSize());
				return FALSE;
			}
		}
		// Always create a back buffer
		if ((m_backbuff = new BYTE [m_desktop->ScreenBuffSize()]) == NULL)
		{
			rdlog.Print(LL_INTERR, rdLOG("unable to allocate back buffer[%d]\n"), m_desktop->ScreenBuffSize());
			return FALSE;
		}
		m_mainsize = m_desktop->ScreenBuffSize();

		// Clear the backbuffer
		memcpy(m_backbuff, m_mainbuff, m_desktop->ScreenBuffSize());

	}

	rdlog.Print(LL_INTINFO, rdLOG("local buffer=%d, remote buffer=%d\n"), m_mainsize, m_clientbuffsize);

	return TRUE;
}

// returns true if any *(p1+n) != *(p2+n) for 0<n<count-1
inline static bool
bytesdiff(BYTE *p1, BYTE *p2, int count) {
	for (int i=0; i<count; i++) {
		if (*(p1+i) != *(p2+i)) return true;
	}
	return false;
}

// New version of GetChangedRegion.  This version tries to avoid
// sending too much unnecessary data.
#pragma function(memcpy,memcmp)
void
rdBuffer::GetChangedRegion(rdRegion &rgn, RECT &rect)
{
        if (!FastCheckMainbuffer())
                return;

	const int BLOCK_SIZE = 32;
	const UINT bytesPerPixel = m_scrinfo.format.bitsPerPixel / 8;

	RECT new_rect;

	int x, y, ay, by;

	// DWORD align the incoming rectangle.  (bPP will be 8, 16 or 32)
	if (bytesPerPixel < 4) {
		if (bytesPerPixel == 1)				// 1 byte per pixel
			rect.left -= (rect.left & 3);	// round down to nearest multiple of 4
		else								// 2 bytes per pixel
			rect.left -= (rect.left & 1);	// round down to nearest multiple of 2
	}

	// Scan down the rectangle
	unsigned char *o_topleft_ptr = m_backbuff + (rect.top * m_bytesPerRow) + (rect.left * bytesPerPixel);
	unsigned char *n_topleft_ptr = m_mainbuff + (rect.top * m_bytesPerRow) + (rect.left * bytesPerPixel);
	for (y = rect.top; y<rect.bottom; y+=BLOCK_SIZE)
	{
		// Work out way down the bitmap
		unsigned char * o_row_ptr = o_topleft_ptr;
		unsigned char * n_row_ptr = n_topleft_ptr;

		const int blockbottom = Min(y+BLOCK_SIZE, rect.bottom);
		for (x = rect.left; x<rect.right; x+=BLOCK_SIZE)
		{
			// Work our way across the row
			unsigned char *n_block_ptr = n_row_ptr;
			unsigned char *o_block_ptr = o_row_ptr;

			const UINT blockright = Min(x+BLOCK_SIZE, rect.right);
			const UINT bytesPerBlockRow = (blockright-x) * bytesPerPixel;

			// Scan this block
			for (ay = y; ay < blockbottom; ay++)
			{
				if (memcmp(n_block_ptr, o_block_ptr, bytesPerBlockRow) != 0)
				{
					// A pixel has changed, so this block needs updating
					new_rect.top = y;
					new_rect.left = x;
					new_rect.right = blockright;
					new_rect.bottom = blockbottom;
					rgn.AddRect(new_rect);

					// Copy the changes to the back buffer
					for (by = ay; by < blockbottom; by++)
					{
						memcpy(o_block_ptr, n_block_ptr, bytesPerBlockRow);
						n_block_ptr+=m_bytesPerRow;
						o_block_ptr+=m_bytesPerRow;
					}

					break;
				}

				n_block_ptr += m_bytesPerRow;
				o_block_ptr += m_bytesPerRow;
			}

			o_row_ptr += bytesPerBlockRow;
			n_row_ptr += bytesPerBlockRow;
		}

		o_topleft_ptr += m_bytesPerRow * BLOCK_SIZE;
		n_topleft_ptr += m_bytesPerRow * BLOCK_SIZE;
	}
}

UINT
rdBuffer::GetNumCodedRects(RECT &rect)
{
	// Ask the encoder how many rectangles this update would become
	return m_encoder->NumCodedRects(rect);
}

void
rdBuffer::GrabRect(RECT &rect)
{
	if (!FastCheckMainbuffer())
		return;
	m_desktop->CaptureScreen(rect, m_mainbuff, m_mainsize);
}

void
rdBuffer::CopyRect(RECT &dest, POINT &source)
{
	// Copy the data from one region of the back-buffer to another!
	BYTE *srcptr = m_backbuff + (source.y * m_bytesPerRow) +
		(source.x * m_scrinfo.format.bitsPerPixel/8);
	BYTE *destptr = m_backbuff + (dest.top * m_bytesPerRow) +
		(dest.left * m_scrinfo.format.bitsPerPixel/8);
	const UINT bytesPerLine = (dest.right-dest.left)*(m_scrinfo.format.bitsPerPixel/8);
	if (dest.top < source.y)
	{
		for (int y=dest.top; y < dest.bottom; y++)
		{
			memmove(destptr, srcptr, bytesPerLine);
			srcptr+=m_bytesPerRow;
			destptr+=m_bytesPerRow;
		}
	}
	else
	{
		srcptr += (m_bytesPerRow * ((dest.bottom-dest.top)-1));
		destptr += (m_bytesPerRow * ((dest.bottom-dest.top)-1));
		for (int y=dest.bottom; y > dest.top; y--)
		{
			memmove(destptr, srcptr, bytesPerLine);
			srcptr-=m_bytesPerRow;
			destptr-=m_bytesPerRow;
		}
	}
}

RECT
rdBuffer::GrabMouse()
{
	if (FastCheckMainbuffer()) {
		m_desktop->CaptureScreen(m_desktop->MouseRect(), m_mainbuff, m_mainsize);
		m_desktop->CaptureMouse(m_mainbuff, m_mainsize);
	}
	return m_desktop->MouseRect();
}

BOOL
rdBuffer::SetClientFormat(rfbPixelFormat &format)
{
	rdlog.Print(LL_INTINFO, rdLOG("SetClientFormat called\n"));
	
	// Save the desired format
	m_clientfmtset = TRUE;
	m_clientformat = format;

	// Tell the encoder of the new format
	if (m_encoder != NULL)
		m_encoder->SetRemoteFormat(format);

	// Check that the output buffer is sufficient
	if (!CheckBuffer())
		return FALSE;

	return TRUE;
}

BOOL
rdBuffer::SetEncoding(CARD32 encoding)
{
	// Delete the old encoder
	if (m_encoder != NULL)
	{
		// If a Zlib-like encoders were in use, save corresponding object
		// (with dictionaries) for possible later use on this connection.
		if ( zlib_encoder_in_use )
		{
			m_hold_zlib_encoder = m_encoder;
		}
		else if ( tight_encoder_in_use )
		{
			m_hold_tight_encoder = m_encoder;
		}
		else if ( zlibhex_encoder_in_use )
		{
			m_hold_zlibhex_encoder = m_encoder;
		}
		else
		{
			m_encoder->LogStats();
			delete m_encoder;
		}
		m_encoder = NULL;
	}

	// Expect to not use the zlib encoder below.  However, this
	// is overriden if zlib was selected.
	zlib_encoder_in_use = false;
	tight_encoder_in_use = false;
	zlibhex_encoder_in_use = false;

	// Returns FALSE if the desired encoding cannot be used
	switch(encoding)
	{

	case rfbEncodingRaw:

		rdlog.Print(LL_INTINFO, rdLOG("raw encoder requested\n"));

		// Create a RAW encoder
		m_encoder = new rdEncoder;
		if (m_encoder == NULL)
			return FALSE;
		break;

	case rfbEncodingRRE:

		rdlog.Print(LL_INTINFO, rdLOG("RRE encoder requested\n"));

		// Create a RRE encoder
		m_encoder = new rdEncodeRRE;
		if (m_encoder == NULL)
			return FALSE;
		break;

	case rfbEncodingCoRRE:

		rdlog.Print(LL_INTINFO, rdLOG("CoRRE encoder requested\n"));

		// Create a CoRRE encoder
		m_encoder = new rdEncodeCoRRE;
		if (m_encoder == NULL)
			return FALSE;
		break;

	case rfbEncodingHextile:

		rdlog.Print(LL_INTINFO, rdLOG("Hextile encoder requested\n"));

		// Create a Hextile encoder
		m_encoder = new rdEncodeHexT;
		if (m_encoder == NULL)
			return FALSE;
		break;

	case rfbEncodingZlib:

		rdlog.Print(LL_INTINFO, rdLOG("Zlib encoder requested\n"));

		// Create a Zlib encoder, if needed.
		// If a Zlib encoder was used previously, then reuse it here
		// to maintain zlib dictionary synchronization with the viewer.
		if ( m_hold_zlib_encoder == NULL )
		{
			m_encoder = new rdEncodeZlib;
		}
		else
		{
			m_encoder = m_hold_zlib_encoder;
		}
		if (m_encoder == NULL)
			return FALSE;
		zlib_encoder_in_use = true;
		break;

	case rfbEncodingTight:

		rdlog.Print(LL_INTINFO, rdLOG("Tight encoder requested\n"));

		// Create a Tight encoder, if needed.
		// If a Tight encoder was used previously, then reuse it here
		// to maintain zlib dictionaries synchronization with the viewer.
		if ( m_hold_tight_encoder == NULL )
		{
			m_encoder = new rdEncodeTight;
		}
		else
		{
			m_encoder = m_hold_tight_encoder;
		}
		if (m_encoder == NULL)
			return FALSE;
		tight_encoder_in_use = true;
		break;

	case rfbEncodingZlibHex:

		rdlog.Print(LL_INTINFO, rdLOG("ZlibHex encoder requested\n"));

		// Create a ZlibHex encoder, if needed.
		// If a ZlibHex encoder was used previously, then reuse it here
		// to maintain zlib dictionary synchronization with the viewer.
		if ( m_hold_zlibhex_encoder == NULL )
		{
			m_encoder = new rdEncodeZlibHex;
		}
		else
		{
			m_encoder = m_hold_zlibhex_encoder;
		}
		if (m_encoder == NULL)
			return FALSE;
		zlibhex_encoder_in_use = true;
		break;

	default:
		// An unknown encoding was specified
		rdlog.Print(LL_INTERR, rdLOG("unknown encoder requested\n"));

		return FALSE;
	}

	// Initialise it and give it the pixel format
	m_encoder->Init();
	m_encoder->SetLocalFormat(
			m_scrinfo.format,
			m_scrinfo.framebufferWidth,
			m_scrinfo.framebufferHeight);
	m_encoder->SetCompressLevel(m_compresslevel);
	m_encoder->SetQualityLevel(m_qualitylevel);
	if (m_clientfmtset)
		if (!m_encoder->SetRemoteFormat(m_clientformat))
		{
			rdlog.Print(LL_INTERR, rdLOG("client pixel format is not supported\n"));

			return FALSE;
		}

	// Check that the client buffer is compatible
	return CheckBuffer();
}

void
rdBuffer::SetCompressLevel(int level)
{
	m_compresslevel = (level >= 0 && level <= 9) ? level : 6;
	if (m_encoder != NULL)
		m_encoder->SetCompressLevel(m_compresslevel);
}

void
rdBuffer::SetQualityLevel(int level)
{
	m_qualitylevel = (level >= 0 && level <= 9) ? level : -1;
	if (m_encoder != NULL)
		m_encoder->SetQualityLevel(m_qualitylevel);
}

void
rdBuffer::EnableXCursor(BOOL enable)
{
	m_use_xcursor = enable;
	if (m_encoder != NULL) {
		m_encoder->EnableXCursor(enable);
	}
	m_hcursor = NULL;
}

void
rdBuffer::EnableRichCursor(BOOL enable)
{
	m_use_richcursor = enable;
	if (m_encoder != NULL) {
		m_encoder->EnableRichCursor(enable);
	}
	m_hcursor = NULL;
}

void
rdBuffer::EnableLastRect(BOOL enable)
{
	m_use_lastrect = enable;
	if (m_encoder != NULL) {
		m_encoder->EnableLastRect(enable);
	}
}

void
rdBuffer::Clear(RECT &rect)
{
	if (!FastCheckMainbuffer())
		return;

	rdlog.Print(LL_INTINFO,
		rdLOG("clearing rectangle (%d, %d)-(%d, %d)\n"),
		rect.left, rect.top, rect.right, rect.bottom);

	// Update the contents of a region, to stop it from being marked as having changed
	BYTE *backptr = m_backbuff + (rect.top * m_bytesPerRow) + (rect.left * m_scrinfo.format.bitsPerPixel/8);
	BYTE *mainptr = m_mainbuff + (rect.top * m_bytesPerRow) + (rect.left * m_scrinfo.format.bitsPerPixel/8);
	const UINT bytesPerLine = (rect.right-rect.left)*(m_scrinfo.format.bitsPerPixel/8);
	for (int y=rect.top; y < rect.bottom; y++)
	{
		memcpy(backptr, mainptr, bytesPerLine);
		backptr+=m_bytesPerRow;
		mainptr+=m_bytesPerRow;
	}
}

// Routine to translate a rectangle between pixel formats
UINT
rdBuffer::TranslateRect(const RECT &rect, VSocket *outConn)
{
	if (!FastCheckMainbuffer())
		return 0;

	// Call the encoder to encode the rectangle into the client buffer...
	return m_encoder->EncodeRect(m_backbuff, outConn, m_clientbuff, rect);
}

// Verify that the fast blit buffer hasn't changed
inline BOOL
rdBuffer::FastCheckMainbuffer()
{
	VOID *tmp = m_desktop->OptimisedBlitBuffer();
	if (tmp && (m_mainbuff != tmp))
		return CheckBuffer();
	return TRUE;
}

// Check if cursor shape update should be sent
BOOL
rdBuffer::IsCursorUpdatePending()
{
	if (m_use_xcursor || m_use_richcursor) {
		HCURSOR temp_hcursor = m_desktop->GetCursor();
		if (temp_hcursor != m_hcursor) {
			m_hcursor = temp_hcursor;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL
rdBuffer::SendCursorShape(VSocket *outConn) {
	return m_encoder->SendCursorShape(outConn, m_desktop);
}

BOOL
rdBuffer::SendEmptyCursorShape(VSocket *outConn) {
	return m_encoder->SendEmptyCursorShape(outConn);
}

