
// Hextile Encoding
//
// The bits of the ClientConnection object to do with Hextile.

#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"
#include "zlib\zlib.h"

bool ClientConnection::zlibDecompress(unsigned char *from_buf, unsigned char *to_buf, unsigned int count, unsigned int size, z_stream *decompressor)
{
	int inflateResult;

	decompressor->next_in = from_buf;
	decompressor->avail_in = count;
	decompressor->next_out = to_buf;
	decompressor->avail_out = size;
	decompressor->data_type = Z_BINARY;
		
	// Insure the inflator is initialized
	if ( decompressor->total_in == ZLIBHEX_DECOMP_UNINITED ) {
		decompressor->total_in = 0;
		decompressor->total_out = 0;
		decompressor->zalloc = Z_NULL;
		decompressor->zfree = Z_NULL;
		decompressor->opaque = Z_NULL;

		inflateResult = inflateInit( decompressor );
		if ( inflateResult != Z_OK ) {
			rdlog.Print(0, _T("zlib inflateInit error: %d\n"), inflateResult);
			return false;
		}

	}

	// Decompress screen data
	inflateResult = inflate( decompressor, Z_SYNC_FLUSH );
	if ( inflateResult < 0 ) {
		rdlog.Print(0, _T("zlib inflate error: %d\n"), inflateResult);
		return false;
	}

	return true;
}

void ClientConnection::ReadZlibHexRect(rfbFramebufferUpdateRectHeader *pfburh)
{
	switch (m_myFormat.bitsPerPixel) {
	case 8:
		HandleZlibHexEncoding8(pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h);
		break;
	case 16:
		HandleZlibHexEncoding16(pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h);
		break;
	case 32:
		HandleZlibHexEncoding32(pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h);
		break;
	}
}

COLORREF bgcolor = 0;
COLORREF fgcolor = 0;

#define DEFINE_HEXTILE(bpp)														\
void ClientConnection::HandleZlibHexEncoding##bpp(int rx, int ry, int rw, int rh)		\
{																				\
    int x, y, w, h;																\
    CARD8 subencoding;															\
    CARD16 nCompData;															\
																				\
    CheckBufferSize((( 16 * 16 + 2 ) * bpp / 8 ) + 20 );											\
    CheckZlibBufferSize((( 16 * 16 + 2 ) * bpp / 8 ) + 20 );										\
	SETUP_COLOR_SHORTCUTS;														\
																				\
    for (y = ry; y < ry+rh; y += 16) {											\
		omni_mutex_lock l(m_bitmapdcMutex);										\
		ObjectSelector b(m_hBitmapDC, m_hBitmap);								\
		PaletteSelector p(m_hBitmapDC, m_hPalette);								\
		for (x = rx; x < rx+rw; x += 16) {										\
            w = h = 16;															\
            if (rx+rw - x < 16)													\
                w = rx+rw - x;													\
            if (ry+rh - y < 16)													\
                h = ry+rh - y;													\
																				\
            ReadExact((char *)&subencoding, 1);									\
																				\
            if (subencoding & rfbHextileRaw) {									\
                ReadExact(m_netbuf, w * h * (bpp / 8));							\
                SETPIXELS(m_netbuf, bpp, x,y,w,h)								\
                continue;														\
            }																	\
																				\
            if (subencoding & rfbHextileZlibRaw) {								\
                ReadExact((char *)&nCompData, 2);								\
                nCompData = Swap16IfLE(nCompData);								\
                ReadExact(m_netbuf, nCompData);									\
		        if (zlibDecompress((unsigned char *)m_netbuf, m_zlibbuf, nCompData, ((w*h+2)*(bpp/8)), &m_decompStreamRaw)) {  \
                    SETPIXELS(m_zlibbuf, bpp, x,y,w,h);							\
				}																\
                continue;														\
            }																	\
																				\
            if (subencoding & rfbHextileZlibHex) {								\
                ReadExact((char *)&nCompData, 2);								\
                nCompData = Swap16IfLE(nCompData);								\
                ReadExact(m_netbuf, nCompData);									\
		        if (zlibDecompress((unsigned char *)m_netbuf, m_zlibbuf, nCompData, ((w*h+2)*(bpp/8)+20), &m_decompStreamEncoded)) {  \
                    HandleZlibHexSubencodingBuf##bpp(x, y, w, h, subencoding, m_zlibbuf);							\
				}																\
                continue;														\
            }																	\
			else {																\
				HandleZlibHexSubencodingStream##bpp(x, y, w, h, subencoding);	\
			}																	\
																				\
        }																		\
    }																			\
																				\
}																				\
																				\
void ClientConnection::HandleZlibHexSubencodingStream##bpp(int x, int y, int w, int h, int subencoding)		\
{																				\
    CARD##bpp bg, fg;															\
    int i;																		\
    CARD8 *ptr;																	\
    int sx, sy, sw, sh;															\
    CARD8 nSubrects;															\
																				\
	SETUP_COLOR_SHORTCUTS;														\
																				\
	if (subencoding & rfbHextileBackgroundSpecified) {							\
		ReadExact((char *)&bg, (bpp/8));										\
		bgcolor = COLOR_FROM_PIXEL##bpp##_ADDRESS(&bg);							\
	}																			\
	FillSolidRect(x,y,w,h,bgcolor);												\
																				\
	if (subencoding & rfbHextileForegroundSpecified)  {							\
		ReadExact((char *)&fg, (bpp/8));										\
		fgcolor = COLOR_FROM_PIXEL##bpp##_ADDRESS(&fg);							\
	}																			\
																				\
	if (!(subencoding & rfbHextileAnySubrects)) {								\
		return;																	\
	}																			\
																				\
	ReadExact( (char *)&nSubrects, 1);											\
																				\
	ptr = (CARD8 *)m_netbuf;													\
																				\
	if (subencoding & rfbHextileSubrectsColoured) {								\
																				\
		ReadExact( m_netbuf, nSubrects * (2 + (bpp / 8)));						\
																				\
		for (i = 0; i < nSubrects; i++) {										\
			fgcolor = COLOR_FROM_PIXEL##bpp##_ADDRESS(ptr);						\
			ptr += (bpp/8);														\
			sx = *ptr >> 4;														\
			sy = *ptr++ & 0x0f;													\
			sw = (*ptr >> 4) + 1;												\
			sh = (*ptr++ & 0x0f) + 1;											\
			FillSolidRect(x+sx, y+sy, sw, sh, fgcolor);							\
		}																		\
																				\
	} else {																	\
		ReadExact(m_netbuf, nSubrects * 2);										\
																				\
		for (i = 0; i < nSubrects; i++) {										\
			sx = *ptr >> 4;														\
			sy = *ptr++ & 0x0f;													\
			sw = (*ptr >> 4) + 1;												\
			sh = (*ptr++ & 0x0f) + 1;											\
			FillSolidRect(x+sx, y+sy, sw, sh, fgcolor);							\
		}																		\
	}																			\
}																				\
																				\
																				\
void ClientConnection::HandleZlibHexSubencodingBuf##bpp(int x, int y, int w, int h, int subencoding, unsigned char *buffer)		\
{																				\
	CARD##bpp bg, fg;															\
	int i;																		\
	CARD8 *ptr;																	\
	int sx, sy, sw, sh;															\
	CARD8 nSubrects;															\
	int bufIndex = 0;															\
																				\
	SETUP_COLOR_SHORTCUTS;														\
																				\
	if (subencoding & rfbHextileBackgroundSpecified) {							\
		bg = *((CARD##bpp *)(buffer + bufIndex));								\
		bufIndex += (bpp/8);													\
		/* ReadExact((char *)&bg, (bpp/8)); */									\
		bgcolor = COLOR_FROM_PIXEL##bpp##_ADDRESS(&bg);							\
	}																			\
	FillSolidRect(x,y,w,h,bgcolor);												\
																				\
	if (subencoding & rfbHextileForegroundSpecified)  {							\
		fg = *((CARD##bpp *)(buffer + bufIndex));								\
		bufIndex += (bpp/8);													\
		/* ReadExact((char *)&fg, (bpp/8)); */									\
		fgcolor = COLOR_FROM_PIXEL##bpp##_ADDRESS(&fg);							\
	}																			\
																				\
	if (!(subencoding & rfbHextileAnySubrects)) {								\
		return;																	\
	}																			\
																				\
	nSubrects = *((CARD8 *)(buffer + bufIndex));									\
	bufIndex += 1;																\
	/* ReadExact( (char *)&nSubrects, 1); */									\
																				\
	ptr = (CARD8 *)(buffer + bufIndex);											\
																				\
	if (subencoding & rfbHextileSubrectsColoured) {								\
																				\
		/* ReadExact( m_netbuf, nSubrects * (2 + (bpp / 8))); */				\
																				\
		for (i = 0; i < nSubrects; i++) {										\
			fgcolor = COLOR_FROM_PIXEL##bpp##_ADDRESS(ptr);						\
			ptr += (bpp/8);														\
			sx = *ptr >> 4;														\
			sy = *ptr++ & 0x0f;													\
			sw = (*ptr >> 4) + 1;												\
			sh = (*ptr++ & 0x0f) + 1;											\
			FillSolidRect(x+sx, y+sy, sw, sh, fgcolor);							\
		}																		\
																				\
	} else {																	\
		/* ReadExact(m_netbuf, nSubrects * 2); */								\
																				\
		for (i = 0; i < nSubrects; i++) {										\
			sx = *ptr >> 4;														\
			sy = *ptr++ & 0x0f;													\
			sw = (*ptr >> 4) + 1;												\
			sh = (*ptr++ & 0x0f) + 1;											\
			FillSolidRect(x+sx, y+sy, sw, sh, fgcolor);							\
		}																		\
	}																			\
}


DEFINE_HEXTILE(8)
DEFINE_HEXTILE(16)
DEFINE_HEXTILE(32)


