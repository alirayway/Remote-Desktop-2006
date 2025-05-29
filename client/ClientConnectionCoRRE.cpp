
// CoRRE (Compact Rising Rectangle Encoding)
//
// The bits of the ClientConnection object to do with CoRRE.

#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"

void ClientConnection::ReadCoRRERect(rfbFramebufferUpdateRectHeader *pfburh)
{
	// An RRE rect is always followed by a background color
	// For speed's sake we read them together into a buffer.
	char tmpbuf[sz_rfbRREHeader+4];			// biggest pixel is 4 bytes long
    rfbRREHeader *prreh = (rfbRREHeader *) tmpbuf;
	CARD8 *pcolor = (CARD8 *) tmpbuf + sz_rfbRREHeader;
	ReadExact(tmpbuf, sz_rfbRREHeader + m_minPixelBytes);

	prreh->nSubrects = Swap32IfLE(prreh->nSubrects);

	SETUP_COLOR_SHORTCUTS;
    COLORREF color;
    switch (m_myFormat.bitsPerPixel) {
        case 8:
            color = COLOR_FROM_PIXEL8_ADDRESS(pcolor); break;
        case 16:
			color = COLOR_FROM_PIXEL16_ADDRESS(pcolor); break;
        case 24:
        case 32:
            color = COLOR_FROM_PIXEL32_ADDRESS(pcolor); break;
    }

    // Draw the background of the rectangle
	{
		omni_mutex_lock l(m_bitmapdcMutex);
		ObjectSelector b(m_hBitmapDC, m_hBitmap);
		PaletteSelector ps(m_hBitmapDC, m_hPalette);

		FillSolidRect(pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h, color);
	}

    if (prreh->nSubrects == 0) return;

	// Draw the sub-rectangles
    rfbCoRRERectangle *pRect;
	rfbRectangle rect;

	// The size of an CoRRE subrect including color info
	int subRectSize = m_minPixelBytes + sz_rfbCoRRERectangle;

	// Read subrects into the buffer 
	CheckBufferSize(subRectSize * prreh->nSubrects);
    ReadExact(m_netbuf, subRectSize * prreh->nSubrects);
	BYTE *p = (BYTE *) m_netbuf;

	// No other threads can use bitmap DC
	omni_mutex_lock l(m_bitmapdcMutex);
	ObjectSelector b(m_hBitmapDC, m_hBitmap);
	PaletteSelector ps(m_hBitmapDC, m_hPalette);

	for (CARD32 i = 0; i < prreh->nSubrects; i++) {
		pRect = (rfbCoRRERectangle *) (p + m_minPixelBytes);
			
		switch (m_myFormat.bitsPerPixel) {
		case 8:
			color = COLOR_FROM_PIXEL8_ADDRESS(p); break;
		case 16:
			color = COLOR_FROM_PIXEL16_ADDRESS(p); break;
		case 32:
			color = COLOR_FROM_PIXEL32_ADDRESS(p); break;
		};
			
		rect.x = pRect->x + pfburh->r.x;
		rect.y = pRect->y + pfburh->r.y;
		rect.w = pRect->w;
		rect.h = pRect->h;

		FillSolidRect(rect.x, rect.y, rect.w, rect.h, color);
		p += subRectSize;
	}
}
