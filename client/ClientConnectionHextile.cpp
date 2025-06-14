
// Hextile Encoding
//
// The bits of the ClientConnection object to do with Hextile.

#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"

void ClientConnection::ReadHextileRect(rfbFramebufferUpdateRectHeader *pfburh)
{
	switch (m_myFormat.bitsPerPixel) {
	case 8:
		HandleHextileEncoding8(pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h);
		break;
	case 16:
		HandleHextileEncoding16(pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h);
		break;
	case 32:
		HandleHextileEncoding32(pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h);
		break;
	}
}


#define DEFINE_HEXTILE(bpp)                                                   \
void ClientConnection::HandleHextileEncoding##bpp(int rx, int ry, int rw, int rh)                    \
{                                                                             \
    CARD##bpp bg, fg;                                                         \
	COLORREF bgcolor, fgcolor;												  \
    int i;                                                                    \
    CARD8 *ptr;                                                               \
    int x, y, w, h;                                                           \
    int sx, sy, sw, sh;                                                       \
    CARD8 subencoding;                                                        \
    CARD8 nSubrects;                                                          \
																			  \
    CheckBufferSize( 16 * 16 * bpp );										  \
	SETUP_COLOR_SHORTCUTS;                                                    \
                                                                              \
    for (y = ry; y < ry+rh; y += 16) {                                        \
		omni_mutex_lock l(m_bitmapdcMutex);									  \
		ObjectSelector b(m_hBitmapDC, m_hBitmap);							  \
		PaletteSelector p(m_hBitmapDC, m_hPalette);							  \
		for (x = rx; x < rx+rw; x += 16) {                                    \
            w = h = 16;                                                       \
            if (rx+rw - x < 16)                                               \
                w = rx+rw - x;                                                \
            if (ry+rh - y < 16)                                               \
                h = ry+rh - y;                                                \
                                                                              \
            ReadExact((char *)&subencoding, 1);                               \
                                                                              \
            if (subencoding & rfbHextileRaw) {                                \
                ReadExact(m_netbuf, w * h * (bpp / 8));                       \
                SETPIXELS(m_netbuf, bpp, x,y,w,h)                             \
                continue;                                                     \
            }                                                                 \
                                                                              \
		    if (subencoding & rfbHextileBackgroundSpecified) {                \
                ReadExact((char *)&bg, (bpp/8));                              \
				bgcolor = COLOR_FROM_PIXEL##bpp##_ADDRESS(&bg);  			  \
			}																  \
            FillSolidRect(x,y,w,h,bgcolor);                                   \
                                                                              \
            if (subencoding & rfbHextileForegroundSpecified)  {               \
                ReadExact((char *)&fg, (bpp/8));                              \
				fgcolor = COLOR_FROM_PIXEL##bpp##_ADDRESS(&fg);				  \
			}                                                                 \
                                                                              \
            if (!(subencoding & rfbHextileAnySubrects)) {                     \
                continue;                                                     \
            }                                                                 \
                                                                              \
            ReadExact( (char *)&nSubrects, 1) ;                               \
                                                                              \
            ptr = (CARD8 *)m_netbuf;                                          \
                                                                              \
            if (subencoding & rfbHextileSubrectsColoured) {                   \
				                                                              \
                ReadExact( m_netbuf, nSubrects * (2 + (bpp / 8)));            \
                                                                              \
                for (i = 0; i < nSubrects; i++) {                             \
                    fgcolor = COLOR_FROM_PIXEL##bpp##_ADDRESS(ptr);           \
					ptr += (bpp/8);                                           \
                    sx = *ptr >> 4;                                           \
                    sy = *ptr++ & 0x0f;                                       \
                    sw = (*ptr >> 4) + 1;                                     \
                    sh = (*ptr++ & 0x0f) + 1;                                 \
                    FillSolidRect(x+sx, y+sy, sw, sh, fgcolor);               \
                }                                                             \
                                                                              \
            } else {                                                          \
                ReadExact(m_netbuf, nSubrects * 2);								\
                                                                              \
                for (i = 0; i < nSubrects; i++) {                             \
                    sx = *ptr >> 4;                                           \
                    sy = *ptr++ & 0x0f;                                       \
                    sw = (*ptr >> 4) + 1;                                     \
                    sh = (*ptr++ & 0x0f) + 1;                                 \
                    FillSolidRect(x+sx, y+sy, sw, sh, fgcolor);               \
                }                                                             \
            }                                                                 \
        }                                                                     \
    }                                                                         \
                                                                              \
}

DEFINE_HEXTILE(8)
DEFINE_HEXTILE(16)
DEFINE_HEXTILE(32)


