

// rdEncoder - Object used to encode data for RFB

#include "rdEncoder.h"
#include "rdBuffer.h"

// Pixel format used internally when the client is palette-based & server is truecolour

static const rfbPixelFormat BGR233Format = {
    8, 8, 0, 1, 7, 7, 3, 0, 3, 6
};

// The base (RAW) encoder class

rdEncoder::rdEncoder()
{
	ZeroMemory(&m_remoteformat, sizeof(m_remoteformat));
	ZeroMemory(&m_localformat, sizeof(m_localformat));
	ZeroMemory(&m_transformat, sizeof(m_transformat));
	m_transtable = NULL;
	m_localpalette = NULL;
	m_bytesPerRow = 0;
	m_compresslevel = 6;
	m_qualitylevel = -1;
	m_use_xcursor = FALSE;
	m_use_richcursor = FALSE;
	m_use_lastrect = FALSE;
}

rdEncoder::~rdEncoder()
{
	if (m_transtable != NULL)
	{
		free(m_transtable);
		m_transtable = NULL;
	}
	if (m_localpalette != NULL)
	{
		free(m_localpalette);
		m_localpalette = NULL;
	}
}

void
rdEncoder::Init()
{
	dataSize = 0;
	rectangleOverhead = 0;
	encodedSize = 0;
	transmittedSize = 0;
}

void
rdEncoder::LogStats()
{
	rdlog.Print(LL_INTINFO, rdLOG("%s encoder stats: data=%d, overhead=%d, "
									"encoded=%d, sent=%d\n"),
				 GetEncodingName(),
				 dataSize, rectangleOverhead, encodedSize, transmittedSize);

	if (dataSize != 0) {
		rdlog.Print(LL_INTINFO, rdLOG("%s encoder efficiency: %.3f%%\n"),
					 GetEncodingName(),
					 (double)((double)((dataSize - transmittedSize) * 100) / dataSize));
	}
}

UINT
rdEncoder::RequiredBuffSize(UINT width, UINT height)
{
	return sz_rfbFramebufferUpdateRectHeader +
		(width * height * m_remoteformat.bitsPerPixel)/8;
}

UINT
rdEncoder::NumCodedRects(RECT &rect)
{
	return 1;
}

// Translate a rectangle
inline void
rdEncoder::Translate(BYTE *source, BYTE *dest, const RECT &rect)
{
	// Calculate where in the source rectangle to read from
	BYTE *sourcepos = (BYTE *)(source + (m_bytesPerRow * rect.top)+(rect.left * (m_localformat.bitsPerPixel / 8)));

	// Call the translation function
	(*m_transfunc) (m_transtable,
					&m_localformat,
					&m_transformat,
					(char *)sourcepos,
					(char *)dest,
					m_bytesPerRow,
					rect.right-rect.left,
					rect.bottom-rect.top
					);
}

// Translate a rectangle (using arbitrary m_bytesPerRow value,
// always translating from the beginning of the source pixel array)
// NOTE: overloaded function!
inline void
rdEncoder::Translate(BYTE *source, BYTE *dest, int w, int h, int bytesPerRow)
{
	// Call the translation function
	(*m_transfunc) (m_transtable, &m_localformat, &m_transformat,
					(char *)source, (char *)dest, bytesPerRow, w, h);
}

// Encode a rectangle
inline UINT
rdEncoder::EncodeRect(BYTE *source, BYTE *dest, const RECT &rect)
{

	const rectW = rect.right - rect.left;
	const rectH = rect.bottom - rect.top;


	// Create the header for the update in the destination area
	rfbFramebufferUpdateRectHeader *surh = (rfbFramebufferUpdateRectHeader *)dest;
	surh->r.x = (CARD16) rect.left;
	surh->r.y = (CARD16) rect.top;
	surh->r.w = (CARD16) rectW;
	surh->r.h = (CARD16) rectH;
	surh->r.x = Swap16IfLE(surh->r.x);
	surh->r.y = Swap16IfLE(surh->r.y);
	surh->r.w = Swap16IfLE(surh->r.w);
	surh->r.h = Swap16IfLE(surh->r.h);
	surh->encoding = Swap32IfLE(rfbEncodingRaw);

	// Update raw encoding statistics
	rectangleOverhead += sz_rfbFramebufferUpdateRectHeader;
	dataSize += ( rectW * rectH * m_remoteformat.bitsPerPixel) / 8;
	encodedSize += ( rectW * rectH * m_remoteformat.bitsPerPixel) / 8;
	transmittedSize += sz_rfbFramebufferUpdateRectHeader + ( rectW * rectH * m_remoteformat.bitsPerPixel) / 8;

	// Translate the data in place in the output buffer
	Translate(source, dest + sz_rfbFramebufferUpdateRectHeader, rect);

	// Return the buffer size
	return sz_rfbFramebufferUpdateRectHeader +
		(rectW*rectH*m_remoteformat.bitsPerPixel) / 8;
}

// Encode a rectangle directly to the output stream.
// This implementation may not be the best, but it will work with all
// of the existing EncodeRect(BYTE *, BYTE *, const RECT &) implementations.
// Note, that the returned value is that of any data in the dest buffer that
// was not yet transmitted on the outConn.
// The primary justification for adding this method is to allow encodings to
// transmit partial data during the encoding process.  This can improve
// performance considerably for slower (more complex) encoding algorithms.
inline UINT
rdEncoder::EncodeRect(BYTE *source, VSocket *outConn, BYTE *dest, const RECT &rect)
{

	return EncodeRect(source, dest, rect);

}

BOOL
rdEncoder::GetRemotePalette(RGBQUAD *quadlist, UINT ncolours)
{
	rdlog.Print(LL_INTINFO, rdLOG("remote palette data requested\n"));

	// If the local server is palette-based then call SetTranslateFunction
	// to update the palette-to-truecolour mapping:
	if (!m_localformat.trueColour)
	{
		if (!SetTranslateFunction())
			return FALSE;
	}

	// If the client is truecolour then don't fill in the palette buffer...
	if (m_remoteformat.trueColour)
		return FALSE;

	// If the server is truecolour then fake BGR233
	if (m_localformat.trueColour)
	{
		// Fake BGR233...
		rdlog.Print(LL_INTINFO, rdLOG("generating BGR233 palette data\n"));

		int ncolours = 1 << m_transformat.bitsPerPixel;
		if (m_localpalette != NULL)
			free(m_localpalette);
		m_localpalette = (char *)malloc(ncolours * sizeof(RGBQUAD));
		
		if (m_localpalette != NULL)
		{
			RGBQUAD *colour = (RGBQUAD *)m_localpalette;

			for (int i=0; i<ncolours; i++)
			{
				colour[i].rgbBlue = (((i >> m_transformat.blueShift) & m_transformat.blueMax) * 255) / m_transformat.blueMax;
				colour[i].rgbRed = (((i >> m_transformat.redShift) & m_transformat.redMax) * 255) / m_transformat.redMax;
				colour[i].rgbGreen = (((i >> m_transformat.greenShift) & m_transformat.greenMax) * 255) / m_transformat.greenMax;
			}
		}
	}
	else
	{
		// Set up RGBQUAD rfbPixelFormat info
		rdlog.Print(LL_INTINFO, rdLOG("generating 8-bit palette data\n"));

		rfbPixelFormat remote;
		remote.trueColour = TRUE;
		remote.bitsPerPixel = 32;
		remote.depth = 24;
		remote.bigEndian = FALSE;
		remote.redMax = remote.greenMax = remote.blueMax = 255;
		remote.redShift = 16;
		remote.greenShift = 8;
		remote.blueShift = 0;

		// We get the ColourMapSingleTableFns procedure to handle retrieval of the
		// palette for us, to avoid replicating the code!
		(*rfbInitColourMapSingleTableFns[remote.bitsPerPixel / 16])
			(&m_localpalette, &m_localformat, &remote);
	}

	// Did we create some palette info?
	if (m_localpalette == NULL)
	{
		rdlog.Print(LL_INTERR, rdLOG("failed to obtain colour map data!\n"));
		return FALSE;
	}

	// Copy the data into the RGBQUAD buffer
	memcpy(quadlist, m_localpalette, ncolours*sizeof(RGBQUAD));

	return TRUE;
}

BOOL
rdEncoder::SetTranslateFunction()
{
	rdlog.Print(LL_INTINFO, rdLOG("SetTranslateFunction called\n"));

	// By default, the actual format translated to matches the client format
	m_transformat = m_remoteformat;

    // Check that bits per pixel values are valid

    if ((m_transformat.bitsPerPixel != 8) &&
		(m_transformat.bitsPerPixel != 16) &&
		(m_transformat.bitsPerPixel != 32))
    {
		rdlog.Print(LL_CONNERR,
			rdLOG("only 8, 16 or 32 bits supported remotely - %d requested\n"),
			m_transformat.bitsPerPixel
			);

		return FALSE;
    }

    if ((m_localformat.bitsPerPixel != 8) &&
		(m_localformat.bitsPerPixel != 16) &&
		(m_localformat.bitsPerPixel != 32))
    {
		rdlog.Print(LL_CONNERR,
			rdLOG("only 8, 16 or 32 bits supported locally - %d in use\n"),
			m_localformat.bitsPerPixel
			);

		return FALSE;
    }

	if (!m_transformat.trueColour && (m_transformat.bitsPerPixel != 8))
	{
		rdlog.Print(LL_CONNERR, rdLOG("only 8-bit palette format supported remotely\n"));
		return FALSE;
	}
	if (!m_localformat.trueColour && (m_localformat.bitsPerPixel != 8))
	{
		rdlog.Print(LL_CONNERR, rdLOG("only 8-bit palette format supported locally\n"));
		return FALSE;
	}

	// Now choose the translation function to use

	// We don't do remote palettes unless they're 8-bit
    if (!m_transformat.trueColour)
	{
		// Is the local format the same?
		if (!m_localformat.trueColour &&
			(m_localformat.bitsPerPixel == m_transformat.bitsPerPixel))
		{
			// Yes, so don't do any encoding
			rdlog.Print(LL_INTINFO, rdLOG("no encoding required - both 8-bit palettized\n"));

			m_transfunc = rfbTranslateNone;

			// The first time the client sends an update, it will call
			// GetRemotePalette to get the palette information required
			return TRUE;
		}
		else if (m_localformat.trueColour)
		{
			// Local side is truecolour, remote is palettized
			rdlog.Print(LL_INTINFO, rdLOG("local truecolour, remote palettized.  using BGR233 palette\n"));

			// Fill out the translation table as if writing to BGR233
			m_transformat = BGR233Format;

			// Continue on down to the main translation section
		}
		else
		{
			// No, so not supported yet...
			rdlog.Print(LL_CONNERR, rdLOG("unknown local pixel format in use!\n"));
			return FALSE;
		}
	}

	// REMOTE FORMAT IS TRUE-COLOUR

	// Handle 8-bit palette-based local data
	if (!m_localformat.trueColour)
	{
		// 8-bit palette to truecolour...

		// Yes, so pick the right translation function!
		rdlog.Print(LL_INTINFO, rdLOG("using 8-bit colourmap to truecolour translation\n"));

		m_transfunc = rfbTranslateWithSingleTableFns
			[m_localformat.bitsPerPixel / 16]
			[m_transformat.bitsPerPixel / 16];

		(*rfbInitColourMapSingleTableFns[m_transformat.bitsPerPixel / 16])
			(&m_transtable, &m_localformat, &m_transformat);
		return m_transtable != NULL;
	}

	// If we reach here then we're doing truecolour to truecolour

	// Are the formats identical?
    if (PF_EQ(m_transformat,m_localformat))
	{
		// Yes, so use the null translation function
		rdlog.Print(LL_INTINFO, rdLOG("no translation required\n"));

		m_transfunc = rfbTranslateNone;

		return TRUE;
    }

	// Is the local display a 16-bit one
    if (m_localformat.bitsPerPixel == 16)
	{
		// Yes, so use a single lookup-table
		rdlog.Print(LL_INTINFO, rdLOG("single LUT used\n"));

		m_transfunc = rfbTranslateWithSingleTableFns
			[m_localformat.bitsPerPixel / 16]
			[m_transformat.bitsPerPixel / 16];

		(*rfbInitTrueColourSingleTableFns[m_transformat.bitsPerPixel / 16])
			(&m_transtable, &m_localformat, &m_transformat);
    }
	else
	{
		// No, so use three tables - one for each of R, G, B.
		rdlog.Print(LL_INTINFO, rdLOG("triple LUT used\n"));

		m_transfunc = rfbTranslateWithRGBTablesFns
			[m_localformat.bitsPerPixel / 16]
			[m_transformat.bitsPerPixel / 16];

		(*rfbInitTrueColourRGBTablesFns[m_transformat.bitsPerPixel / 16])
			(&m_transtable, &m_localformat, &m_transformat);
    }

	return m_transtable != NULL;
}

BOOL
rdEncoder::SetLocalFormat(rfbPixelFormat &pixformat, int width, int height)
{
	// Work out the bytes per row at the local end - useful
	m_bytesPerRow = width * pixformat.bitsPerPixel/8;

	// Save the pixel format
	m_localformat = pixformat;

	// Don't call SetTranslateFunction() if remote format is not set yet.
	if (m_remoteformat.depth == 0)
		return TRUE;

	return SetTranslateFunction();
}

BOOL
rdEncoder::SetRemoteFormat(rfbPixelFormat &pixformat)
{
	// Save the client pixel format
	m_remoteformat = pixformat;

	return SetTranslateFunction();
}

void
rdEncoder::SetCompressLevel(int level)
{
	m_compresslevel = (level >= 0 && level <= 9) ? level : 6;
}

void
rdEncoder::SetQualityLevel(int level)
{
	m_qualitylevel = (level >= 0 && level <= 9) ? level : -1;
}

//
// New code implementing cursor shape updates.
//

BOOL
rdEncoder::SendEmptyCursorShape(VSocket *outConn)
{
	rfbFramebufferUpdateRectHeader hdr;
	hdr.r.x = Swap16IfLE(0);
	hdr.r.y = Swap16IfLE(0);
	hdr.r.w = Swap16IfLE(0);
	hdr.r.h = Swap16IfLE(0);
	if (m_use_xcursor) {
		hdr.encoding = Swap32IfLE(rfbEncodingXCursor);
	} else {
		hdr.encoding = Swap32IfLE(rfbEncodingRichCursor);
	}

	return outConn->SendQueued((char *)&hdr, sizeof(hdr));
}

BOOL
rdEncoder::SendCursorShape(VSocket *outConn, rdDesktop *desktop)
{
	// Make sure the function is used correctly
	if (!m_use_xcursor && !m_use_richcursor)
		return FALSE;

	// Check mouse cursor handle
	HCURSOR hcursor = desktop->GetCursor();
	if (hcursor == NULL) {
		rdlog.Print(LL_INTINFO, rdLOG("cursor handle is NULL.\n"));
		return FALSE;
	}

	// Get cursor info
	ICONINFO IconInfo;
	if (!GetIconInfo(hcursor, &IconInfo)) {
		rdlog.Print(LL_INTINFO, rdLOG("GetIconInfo() failed.\n"));
		return FALSE;
	}
	BOOL isColorCursor = FALSE;
	if (IconInfo.hbmColor != NULL) {
		isColorCursor = TRUE;
		DeleteObject(IconInfo.hbmColor);
	}
	if (IconInfo.hbmMask == NULL) {
		rdlog.Print(LL_INTINFO, rdLOG("cursor bitmap handle is NULL.\n"));
		return FALSE;
	}

	// Check bitmap info for the cursor
	BITMAP bmMask;
	if (!GetObject(IconInfo.hbmMask, sizeof(BITMAP), (LPVOID)&bmMask)) {
		rdlog.Print(LL_INTINFO, rdLOG("GetObject() for bitmap failed.\n"));
		DeleteObject(IconInfo.hbmMask);
		return FALSE;
	}
	if (bmMask.bmPlanes != 1 || bmMask.bmBitsPixel != 1) {
		rdlog.Print(LL_INTINFO, rdLOG("incorrect data in cursor bitmap.\n"));
		DeleteObject(IconInfo.hbmMask);
		return FALSE;
	}

	// Get monochrome bitmap data for cursor
	// NOTE: they say we should use GetDIBits() instead of GetBitmapBits().
	BYTE *mbits = new BYTE[bmMask.bmWidthBytes * bmMask.bmHeight];
	if (mbits == NULL)
		return FALSE;

	BOOL success = GetBitmapBits(IconInfo.hbmMask,
								 bmMask.bmWidthBytes * bmMask.bmHeight, mbits);
	DeleteObject(IconInfo.hbmMask);

	if (!success) {
		rdlog.Print(LL_INTINFO, rdLOG("GetBitmapBits() failed.\n"));
		delete[] mbits;
		return FALSE;
	}

	// Compute cursor dimensions
	int width = bmMask.bmWidth;
	int height = (isColorCursor) ? bmMask.bmHeight : bmMask.bmHeight/2;

	// Call appropriate routine to send cursor shape update
	if (!isColorCursor && m_use_xcursor) {
		FixCursorMask(mbits, NULL, width, bmMask.bmHeight, bmMask.bmWidthBytes);
		success = SendXCursorShape(outConn, mbits,
								   IconInfo.xHotspot, IconInfo.yHotspot,
								   width, height);
	}
	else if (m_use_richcursor) {
		int cbits_size = width * height * 4;
		BYTE *cbits = new BYTE[cbits_size];
		if (cbits == NULL) {
			delete[] mbits;
			return FALSE;
		}
		if (!desktop->GetRichCursorData(cbits, hcursor, width, height)) {
			rdlog.Print(LL_INTINFO, rdLOG("rdDesktop::GetRichCursorData() failed.\n"));
			delete[] mbits;
			delete[] cbits;
			return FALSE;
		}
		FixCursorMask(mbits, cbits, width, height, bmMask.bmWidthBytes);
		success = SendRichCursorShape(outConn, mbits, cbits,
									  IconInfo.xHotspot, IconInfo.yHotspot,
									  width, height);
		delete[] cbits;
	}
	else {
		success = FALSE;	// FIXME: We could convert RichCursor -> XCursor.
	}

	// Cleanup
	delete[] mbits;

	return success;
}

BOOL
rdEncoder::SendXCursorShape(VSocket *outConn, BYTE *mask,
							 int xhot, int yhot, int width, int height)
{
	rfbFramebufferUpdateRectHeader hdr;
	hdr.r.x = Swap16IfLE(xhot);
	hdr.r.y = Swap16IfLE(yhot);
	hdr.r.w = Swap16IfLE(width);
	hdr.r.h = Swap16IfLE(height);
	hdr.encoding = Swap32IfLE(rfbEncodingXCursor);

	BYTE colors[6] = { 0, 0, 0, 0xFF, 0xFF, 0xFF };
	int maskRowSize = (width + 7) / 8;
	int maskSize = maskRowSize * height;

	if ( !outConn->SendQueued((char *)&hdr, sizeof(hdr)) ||
		 !outConn->SendQueued((char *)colors, 6) ||
		 !outConn->SendQueued((char *)&mask[maskSize], maskSize) ||
		 !outConn->SendQueued((char *)mask, maskSize) ) {
		return FALSE;
	}
	return TRUE;
}

BOOL
rdEncoder::SendRichCursorShape(VSocket *outConn, BYTE *mbits, BYTE *cbits,
								int xhot, int yhot, int width, int height)
{
	rfbFramebufferUpdateRectHeader hdr;
	hdr.r.x = Swap16IfLE(xhot);
	hdr.r.y = Swap16IfLE(yhot);
	hdr.r.w = Swap16IfLE(width);
	hdr.r.h = Swap16IfLE(height);
	hdr.encoding = Swap32IfLE(rfbEncodingRichCursor);

	// Cet cursor image in local pixel format
	int srcbuf_rowsize = width * (m_localformat.bitsPerPixel / 8);
	while (srcbuf_rowsize % sizeof(DWORD))
		srcbuf_rowsize++;	// Actually, this should never happen

	// Translate image to client pixel format
	int dstbuf_size = width * height * (m_remoteformat.bitsPerPixel / 8);
	BYTE *dstbuf = new BYTE[dstbuf_size];
	Translate(cbits, dstbuf, width, height, srcbuf_rowsize);

	// Send the data
	int mask_rowsize = (width + 7) / 8;
	int mask_size = mask_rowsize * height;
	if ( !outConn->SendQueued((char *)&hdr, sizeof(hdr)) ||
		 !outConn->SendQueued((char *)dstbuf, dstbuf_size) ||
		 !outConn->SendQueued((char *)mbits, mask_size) ) {
		delete[] dstbuf;
		return FALSE;
	}
	delete[] dstbuf;
	return TRUE;
}

void
rdEncoder::FixCursorMask(BYTE *mbits, BYTE *cbits,
						  int width, int height, int width_bytes)
{
	int packed_width_bytes = (width + 7) / 8;

	// Pack and invert bitmap data (mbits)
	int x, y;
	for (y = 0; y < height; y++)
		for (x = 0; x < packed_width_bytes; x++)
			mbits[y * packed_width_bytes + x] = ~mbits[y * width_bytes + x];

	// Replace "inverted background" bits with black color to ensure
	// cross-platform interoperability. Not beautiful but necessary code.
	if (cbits == NULL) {
		BYTE m, c;
		height /= 2;
		for (y = 0; y < height; y++) {
			for (x = 0; x < packed_width_bytes; x++) {
				m = mbits[y * packed_width_bytes + x];
				c = mbits[(height + y) * packed_width_bytes + x];
				mbits[y * packed_width_bytes + x] |= ~(m | c);
				mbits[(height + y) * packed_width_bytes + x] |= ~(m | c);
			}
		}
	} else {
		int bytes_pixel = m_localformat.bitsPerPixel / 8;
		int bytes_row = width * bytes_pixel;
		while (bytes_row % sizeof(DWORD))
			bytes_row++;	// Actually, this should never happen

		BYTE bitmask;
		int b1, b2;
		for (y = 0; y < height; y++) {
			bitmask = 0x80;
			for (x = 0; x < width; x++) {
				if ((mbits[y * packed_width_bytes + x / 8] & bitmask) == 0) {
					for (b1 = 0; b1 < bytes_pixel; b1++) {
						if (cbits[y * bytes_row + x * bytes_pixel + b1] != 0) {
							mbits[y * packed_width_bytes + x / 8] ^= bitmask;
							for (b2 = b1; b2 < bytes_pixel; b2++)
								cbits[y * bytes_row + x * bytes_pixel + b2] = 0x00;
							break;
						}
					}
				}
				if ((bitmask >>= 1) == 0)
					bitmask = 0x80;
			}
		}
	}
}

