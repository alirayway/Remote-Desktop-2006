
// XCursor and RichCursor encodings
//
// Support for cursor shape updates for ClientConnection class.

#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"

void ClientConnection::ReadCursorShape(rfbFramebufferUpdateRectHeader *pfburh) {

	rdlog.Print(6, _T("Receiving cursor shape update, cursor %dx%d\n"),
				 (int)pfburh->r.w, (int)pfburh->r.h);

	int bytesPerRow = (pfburh->r.w + 7) / 8;
	int bytesMaskData = bytesPerRow * pfburh->r.h;
	int bytesSourceData =
		pfburh->r.w * pfburh->r.h * (m_myFormat.bitsPerPixel / 8);
	CheckBufferSize(bytesMaskData);

	SoftCursorFree();

	if (pfburh->r.w * pfburh->r.h == 0)
		return;

	// Ignore cursor shape updates if requested by user
	if (m_opts.m_ignoreShapeUpdates) {
		int bytesToSkip = (pfburh->encoding == rfbEncodingXCursor) ?
			(6 + 2 * bytesMaskData) : (bytesSourceData + bytesMaskData);
		CheckBufferSize(bytesToSkip);
		ReadExact(m_netbuf, bytesToSkip);
		return;
	}

	// Read cursor pixel data.

	rcSource = new COLORREF[pfburh->r.w * pfburh->r.h];

	if (pfburh->encoding == rfbEncodingXCursor) {
		CARD8 xcolors[6];
		ReadExact((char *)xcolors, 6);
		COLORREF rcolors[2];
		rcolors[1] = PALETTERGB(xcolors[0], xcolors[1], xcolors[2]);
		rcolors[0] = PALETTERGB(xcolors[3], xcolors[4], xcolors[5]);

		ReadExact(m_netbuf, bytesMaskData);
		int x, y, n, b;
		int i = 0;
		for (y = 0; y < pfburh->r.h; y++) {
			for (x = 0; x < pfburh->r.w / 8; x++) {
				b = m_netbuf[y * bytesPerRow + x];
				for (n = 7; n >= 0; n--)
					rcSource[i++] = rcolors[b >> n & 1];
			}
			for (n = 7; n >= 8 - pfburh->r.w % 8; n--) {
				rcSource[i++] = rcolors[m_netbuf[y * bytesPerRow + x] >> n & 1];
			}
		}
	} else {
		// rfb.EncodingRichCursor
		CheckBufferSize(bytesSourceData);
		ReadExact(m_netbuf, bytesSourceData);
		SETUP_COLOR_SHORTCUTS;
		char *p = m_netbuf;
		for (int i = 0; i < pfburh->r.w * pfburh->r.h; i++) {
			switch (m_myFormat.bitsPerPixel) {
			case 8:
				rcSource[i] = COLOR_FROM_PIXEL8_ADDRESS(p);
				p++;
				break;
			case 16:
				rcSource[i] = COLOR_FROM_PIXEL16_ADDRESS(p);
				p += 2;
				break;
			case 32:
				rcSource[i] = COLOR_FROM_PIXEL32_ADDRESS(p);
				p += 4;
				break;
			}
		}
	}

	// Read and decode mask data.

	ReadExact(m_netbuf, bytesMaskData);

	rcMask = new bool[pfburh->r.w * pfburh->r.h];

	int x, y, n, b;
	int i = 0;
	for (y = 0; y < pfburh->r.h; y++) {
		for (x = 0; x < pfburh->r.w / 8; x++) {
			b = m_netbuf[y * bytesPerRow + x];
			for (n = 7; n >= 0; n--)
				rcMask[i++] = (b >> n & 1) != 0;
		}
		for (n = 7; n >= 8 - pfburh->r.w % 8; n--) {
			rcMask[i++] = (m_netbuf[y * bytesPerRow + x] >> n & 1) != 0;
		}
	}

	// Set remaining data associated with cursor.

	omni_mutex_lock l(m_cursorMutex);

	rcWidth = pfburh->r.w;
	rcHeight = pfburh->r.h;
	rcHotX = (pfburh->r.x < rcWidth) ? pfburh->r.x : rcWidth - 1;
	rcHotY = (pfburh->r.y < rcHeight) ? pfburh->r.y : rcHeight - 1;

	{
		omni_mutex_lock l(m_bitmapdcMutex);
		ObjectSelector b1(m_hBitmapDC, m_hBitmap);
		PaletteSelector ps1(m_hBitmapDC, m_hPalette);
		m_hSavedAreaDC = CreateCompatibleDC(m_hBitmapDC);
		m_hSavedAreaBitmap =
			CreateCompatibleBitmap(m_hBitmapDC, rcWidth, rcHeight);
	}

	SoftCursorSaveArea();
	SoftCursorDraw();

	rcCursorHidden = false;
	rcLockSet = false;

	prevCursorSet = true;
}

void ClientConnection::ReadCursorPos(rfbFramebufferUpdateRectHeader *pfburh)
{
	int x = (int)pfburh->r.x;
	if (x >= m_si.framebufferWidth)
		x = m_si.framebufferWidth - 1;
	int y = (int)pfburh->r.y;
	if (y >= m_si.framebufferHeight)
		y = m_si.framebufferHeight - 1;

	SoftCursorMove(x, y);
}

//
// SoftCursorLockArea(). This method should be used to prevent
// collisions between simultaneous framebuffer update operations and
// cursor drawing operations caused by movements of pointing device.
// The parameters denote a rectangle where mouse cursor should not
// be drawn. Every next call to this function expands locked area so
// previous locks remain active.
//

void ClientConnection::SoftCursorLockArea(int x, int y, int w, int h) {

	omni_mutex_lock l(m_cursorMutex);

	if (!prevCursorSet)
		return;

	if (!rcLockSet) {
		rcLockX = x;
		rcLockY = y;
		rcLockWidth = w;
		rcLockHeight = h;
		rcLockSet = true;
	} else {
		int newX = (x < rcLockX) ? x : rcLockX;
		int newY = (y < rcLockY) ? y : rcLockY;
		rcLockWidth = (x + w > rcLockX + rcLockWidth) ?
			(x + w - newX) : (rcLockX + rcLockWidth - newX);
		rcLockHeight = (y + h > rcLockY + rcLockHeight) ?
			(y + h - newY) : (rcLockY + rcLockHeight - newY);
		rcLockX = newX;
		rcLockY = newY;
	}

	if (!rcCursorHidden && SoftCursorInLockedArea()) {
		SoftCursorRestoreArea();
		rcCursorHidden = true;
	}
}

//
// SoftCursorUnlockScreen(). This function discards all locks
// performed since previous SoftCursorUnlockScreen() call.
//

void ClientConnection::SoftCursorUnlockScreen() {

	omni_mutex_lock l(m_cursorMutex);

	if (!prevCursorSet)
		return;

	if (rcCursorHidden) {
		SoftCursorSaveArea();
		SoftCursorDraw();
		rcCursorHidden = false;
	}
	rcLockSet = false;
}

//
// SoftCursorMove(). Moves soft cursor in particular location. This
// function respects locking of screen areas so when the cursor is
// moved in the locked area, it becomes invisible until
// SoftCursorUnlockScreen() method is called.
//

void ClientConnection::SoftCursorMove(int x, int y) {

	omni_mutex_lock l(m_cursorMutex);

	if (prevCursorSet && !rcCursorHidden) {
		SoftCursorRestoreArea();
		rcCursorHidden = true;
	}

	rcCursorX = x;
	rcCursorY = y;

	if (prevCursorSet && !(rcLockSet && SoftCursorInLockedArea())) {
		SoftCursorSaveArea();
		SoftCursorDraw();
		rcCursorHidden = false;
	}
}

 //
 // Free all data associated with cursor.
 //

void ClientConnection::SoftCursorFree() {

	omni_mutex_lock l(m_cursorMutex);

	if (prevCursorSet) {
		if (!rcCursorHidden)
			SoftCursorRestoreArea();
		DeleteObject(m_hSavedAreaBitmap);
		DeleteDC(m_hSavedAreaDC);
		delete[] rcSource;
		delete[] rcMask;
		prevCursorSet = false;
	}
}

//////////////////////////////////////////////////////////////////
//
// Low-level methods implementing software cursor functionality.
//

//
// Check if cursor is within locked part of screen.
//

bool ClientConnection::SoftCursorInLockedArea() {

    return (rcLockX < rcCursorX - rcHotX + rcWidth &&
			rcLockY < rcCursorY - rcHotY + rcHeight &&
			rcLockX + rcLockWidth > rcCursorX - rcHotX &&
			rcLockY + rcLockHeight > rcCursorY - rcHotY);
}

//
// Save screen data in memory buffer.
//

void ClientConnection::SoftCursorSaveArea() {

	RECT r;
	SoftCursorToScreen(&r, NULL);
	int x = r.left;
	int y = r.top;
	int w = r.right - r.left;
	int h = r.bottom - r.top;

	omni_mutex_lock l(m_bitmapdcMutex);
	ObjectSelector b1(m_hBitmapDC, m_hBitmap);
	PaletteSelector ps1(m_hBitmapDC, m_hPalette);
	ObjectSelector b2(m_hSavedAreaDC, m_hSavedAreaBitmap);
	PaletteSelector ps2(m_hSavedAreaDC, m_hPalette);

	if (!BitBlt(m_hSavedAreaDC, 0, 0, w, h, m_hBitmapDC, x, y, SRCCOPY)) {
		rdlog.Print(0, _T("Error saving screen under cursor\n"));
	}
}

//
// Restore screen data saved in memory buffer.
//

void ClientConnection::SoftCursorRestoreArea() {

	RECT r;
	SoftCursorToScreen(&r, NULL);
	int x = r.left;
	int y = r.top;
	int w = r.right - r.left;
	int h = r.bottom - r.top;

	omni_mutex_lock l(m_bitmapdcMutex);
	ObjectSelector b1(m_hBitmapDC, m_hBitmap);
	PaletteSelector ps1(m_hBitmapDC, m_hPalette);
	ObjectSelector b2(m_hSavedAreaDC, m_hSavedAreaBitmap);
	PaletteSelector ps2(m_hSavedAreaDC, m_hPalette);

	if (!BitBlt(m_hBitmapDC, x, y, w, h, m_hSavedAreaDC, 0, 0, SRCCOPY)) {
		rdlog.Print(0, _T("Error restoring screen under cursor\n"));
	}

	InvalidateScreenRect(&r);
}

//
// Draw cursor.
//

void ClientConnection::SoftCursorDraw() {

	int x, y, x0, y0;
	int offset;

	omni_mutex_lock l(m_bitmapdcMutex);
	ObjectSelector b(m_hBitmapDC, m_hBitmap);
	PaletteSelector p(m_hBitmapDC, m_hPalette);

	SETUP_COLOR_SHORTCUTS;

	for (y = 0; y < rcHeight; y++) {
		y0 = rcCursorY - rcHotY + y;
		if (y0 >= 0 && y0 < m_si.framebufferHeight) {
			for (x = 0; x < rcWidth; x++) {
				x0 = rcCursorX - rcHotX + x;
				if (x0 >= 0 && x0 < m_si.framebufferWidth) {
					offset = y * rcWidth + x;
					if (rcMask[offset]) {
						SETPIXEL(m_hBitmapDC, x0, y0, rcSource[offset]);
					}
				}
			}
		}
	}

	RECT r;
	SoftCursorToScreen(&r, NULL);
	InvalidateScreenRect(&r);
}

//
// Calculate position, size and offset for the part of cursor
// located inside framebuffer bounds.
//

void ClientConnection::SoftCursorToScreen(RECT *screenArea, POINT *cursorOffset) {

	int cx = 0, cy = 0;

	int x = rcCursorX - rcHotX;
	int y = rcCursorY - rcHotY;
	int w = rcWidth;
	int h = rcHeight;

	if (x < 0) {
		cx = -x;
		w -= cx;
		x = 0;
	} else if (x + w > m_si.framebufferWidth) {
		w = m_si.framebufferWidth - x;
	}
	if (y < 0) {
		cy = -y;
		h -= cy;
		y = 0;
	} else if (y + h > m_si.framebufferHeight) {
		h = m_si.framebufferHeight - y;
	}

	if (w < 0) {
		cx = 0; x = 0; w = 0;
	}
	if (h < 0) {
		cy = 0; y = 0; h = 0;
	}

	if (screenArea != NULL) {
		SetRect(screenArea, x, y, x + w, y + h);
	}
	if (cursorOffset != NULL) {
		cursorOffset->x = cx;
		cursorOffset->y = cy;
	}
}

