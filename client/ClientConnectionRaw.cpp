
// Raw Encoding
//
// The bits of the ClientConnection object to do with Raw.

#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"

void ClientConnection::ReadRawRect(rfbFramebufferUpdateRectHeader *pfburh) {

	UINT numpixels = pfburh->r.w * pfburh->r.h;
    // this assumes at least one byte per pixel. Naughty.
	UINT numbytes = numpixels * m_minPixelBytes;
	// Read in the whole thing
    CheckBufferSize(numbytes);
	ReadExact(m_netbuf, numbytes);

	SETUP_COLOR_SHORTCUTS;

	{
		// No other threads can use bitmap DC
		omni_mutex_lock l(m_bitmapdcMutex);
		ObjectSelector b(m_hBitmapDC, m_hBitmap);							  \
		PaletteSelector p(m_hBitmapDC, m_hPalette);							  \

		// This big switch is untidy but fast
		switch (m_myFormat.bitsPerPixel) {
		case 8:
			SETPIXELS(m_netbuf, 8, pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h)
				break;
		case 16:
			SETPIXELS(m_netbuf, 16, pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h)
				break;
		case 24:
		case 32:
			SETPIXELS(m_netbuf, 32, pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h)            
				break;
		default:
			rdlog.Print(0, _T("Invalid number of bits per pixel: %d\n"), m_myFormat.bitsPerPixel);
			return;
		}
		
	}
}

