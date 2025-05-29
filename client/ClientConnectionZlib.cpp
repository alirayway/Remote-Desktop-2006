
// ZLIB Encoding
//
// The bits of the ClientConnection object to do with zlib.

#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"
#include "zlib\zlib.h"

void ClientConnection::ReadZlibRect(rfbFramebufferUpdateRectHeader *pfburh) {

	UINT numpixels = pfburh->r.w * pfburh->r.h;
    // this assumes at least one byte per pixel. Naughty.
	UINT numRawBytes = numpixels * m_minPixelBytes;
	UINT numCompBytes;
	int inflateResult;

	rfbZlibHeader hdr;

	// Read in the rfbZlibHeader
	ReadExact((char *)&hdr, sz_rfbZlibHeader);

	numCompBytes = Swap32IfLE(hdr.nBytes);

	// Read in the compressed data
    CheckBufferSize(numCompBytes);
	ReadExact(m_netbuf, numCompBytes);

	// Verify enough buffer space for screen update.
	CheckZlibBufferSize(numRawBytes);

	m_decompStream.next_in = (unsigned char *)m_netbuf;
	m_decompStream.avail_in = numCompBytes;
	m_decompStream.next_out = m_zlibbuf;
	m_decompStream.avail_out = numRawBytes;
	m_decompStream.data_type = Z_BINARY;
		
	// Insure the inflator is initialized
	if ( m_decompStreamInited == false ) {
		m_decompStream.total_in = 0;
		m_decompStream.total_out = 0;
		m_decompStream.zalloc = Z_NULL;
		m_decompStream.zfree = Z_NULL;
		m_decompStream.opaque = Z_NULL;

		inflateResult = inflateInit( &m_decompStream );
		if ( inflateResult != Z_OK ) {
			rdlog.Print(0, _T("zlib inflateInit error: %d\n"), inflateResult);
			return;
		}
		m_decompStreamInited = true;
	}

	// Decompress screen data
	inflateResult = inflate( &m_decompStream, Z_SYNC_FLUSH );
	if ( inflateResult < 0 ) {
		rdlog.Print(0, _T("zlib inflate error: %d\n"), inflateResult);
		return;
	}

	SETUP_COLOR_SHORTCUTS;

	{
		// No other threads can use bitmap DC
		omni_mutex_lock l(m_bitmapdcMutex);
		ObjectSelector b(m_hBitmapDC, m_hBitmap);							  \
		PaletteSelector p(m_hBitmapDC, m_hPalette);							  \

		// This big switch is untidy but fast
		switch (m_myFormat.bitsPerPixel) {
		case 8:
			SETPIXELS(m_zlibbuf, 8, pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h)
				break;
		case 16:
			SETPIXELS(m_zlibbuf, 16, pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h)
				break;
		case 24:
		case 32:
			SETPIXELS(m_zlibbuf, 32, pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h)            
				break;
		default:
			rdlog.Print(0, _T("Invalid number of bits per pixel: %d\n"), m_myFormat.bitsPerPixel);
			return;
		}
		
	}
}

