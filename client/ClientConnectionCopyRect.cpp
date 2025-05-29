//
// CopyRect Encoding
//
// The bits of the ClientConnection object to do with CopyRect.

#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"

void ClientConnection::ReadCopyRect(rfbFramebufferUpdateRectHeader *pfburh) {
	rfbCopyRect cr;
	ReadExact((char *) &cr, sz_rfbCopyRect);
	cr.srcX = Swap16IfLE(cr.srcX); 
	cr.srcY = Swap16IfLE(cr.srcY);

	// If *Cursor encoding is used, we should extend our "cursor lock area"
	// (previously set to destination rectangle) to the source rect as well.
	SoftCursorLockArea(cr.srcX, cr.srcY, pfburh->r.w, pfburh->r.h);

	omni_mutex_lock l(m_bitmapdcMutex);									  
	ObjectSelector b(m_hBitmapDC, m_hBitmap);							  
	PaletteSelector p(m_hBitmapDC, m_hPalette);							  

	if (!BitBlt(
		m_hBitmapDC,pfburh->r.x, pfburh->r.y, pfburh->r.w, pfburh->r.h,
		m_hBitmapDC, cr.srcX, cr.srcY, SRCCOPY)) {
		rdlog.Print(0, _T("Error in blit in ClientConnection::CopyRect\n"));
	}
}
