
// rdDesktop object

// The rdDesktop object handles retrieval of data from the
// display buffer.  It also uses the RFBLib DLL to supply
// information on mouse movements and screen updates to
// the server

class rdDesktop;

#if !defined(_WINrd_rdDESKTOP)
#define _WINrd_rdDESKTOP
#pragma once

// Include files
#include "stdhdrs.h"

#include "rdServer.h"
#include "rdRegion.h"
#include "RectList.h"
#include "translate.h"
#include <omnithread.h>

// Constants
extern const UINT RFB_SCREEN_UPDATE;
extern const UINT RFB_COPYRECT_UPDATE;
extern const UINT RFB_MOUSE_UPDATE;
extern const char szDesktopSink[];

#define MAX_REG_ENTRY_LEN             (80)

// Class definition

class rdDesktop
{

// Fields
public:

// Methods
public:
	// Make the desktop thread & window proc friends
	friend class rdDesktopThread;
	friend LRESULT CALLBACK DesktopWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	// Create/Destroy methods
	rdDesktop();
	~rdDesktop();

	BOOL Init(rdServer *pSrv);

	// Routine to signal a rdServer to trigger an update
	void RequestUpdate();

	// Screen translation, capture, info
	void FillDisplayInfo(rfbServerInitMsg *scrInfo);
	void Translate(
		rfbTranslateFnType	translator,
		char				*dest,
		char				*scrBuff,
		rfbPixelFormat		cli_pf,
		const RECT			&rect
		);
	void CaptureScreen(RECT &UpdateArea, BYTE *scrBuff, UINT scrBuffSize);
	rectlist ChangedAreas(
		RECT				&ChangedArea,
		rectlist			&existingRects,
		BYTE				*scrBuff,
		BYTE				*oldBuff
		);
	int ScreenBuffSize();
	HWND Window() { return m_hwnd; }

	// Mouse related
	void CaptureMouse(BYTE *scrBuff, UINT scrBuffSize);
	BOOL GetRichCursorData(BYTE *databuf, HCURSOR hcursor, int width, int height);
	RECT MouseRect();
	void SetCursor(HCURSOR cursor);
	HCURSOR GetCursor() { return m_hcursor; }

	// Clipboard manipulation
	void SetClipText(LPSTR text);

	// Method to obtain the DIBsection buffer if fast blits are enabled
	// If they're disabled, it'll return NULL
	inline VOID *OptimisedBlitBuffer() {return m_DIBbits;};

	BOOL	m_initialClipBoardSeen;

	// Implementation
protected:

	// Routines to hook and unhook us
	BOOL Startup();
	BOOL Shutdown();
	
	// Init routines called by the child thread
	BOOL InitDesktop();
	void KillScreenSaver();
	void KillWallpaper();
	void RestoreWallpaper();

	void ChangeResNow();
	void DisablePattern();
	void DisableIfRegSystemParameter(char *regName, int spiCommand, int spiParamInt, void* spiParamPtr, int spiUpdate);
	void SetupDisplayForConnection();
	BOOL OptimizeDisplayForConnection();
	void ResetDisplayToNormal();

	BOOL InitBitmap();
	BOOL InitWindow();
	BOOL ThunkBitmapInfo();
	BOOL SetPixFormat();
	BOOL SetPixShifts();
	BOOL InitHooks();
	BOOL SetPalette();

	void CopyToBuffer(RECT &rect, BYTE *scrBuff, UINT scrBuffSize);
	void CalcCopyRects();

	// Routine to attempt enabling optimised DIBsection blits
	void EnableOptimisedBlits();

	// Convert a bit mask eg. 00111000 to max=7, shift=3
	static void MaskToMaxAndShift(DWORD mask, CARD16 &max, CARD8 &shift);
	
	// Enabling & disabling clipboard handling
	void SetClipboardActive(BOOL active) {m_clipboard_active = active;};

	// DATA

	// Generally useful stuff
	rdServer 		*m_server;
	omni_thread 	*m_thread;
	HWND			m_hwnd;
	UINT			m_timerid;
	HWND			m_hnextviewer;
	BOOL			m_clipboard_active;

	// device contexts for memory and the screen
	HDC				m_hmemdc;
	HDC				m_hrootdc;

	// New and old bitmaps
	HBITMAP			m_membitmap;
	omni_mutex		m_bitbltlock;

	RECT			m_bmrect;
	struct _BMInfo {
		BOOL			truecolour;
		BITMAPINFO		bmi;
		// Colormap info - comes straight after BITMAPINFO - **HACK**
		RGBQUAD			cmap[256];
	} m_bminfo;

	// Screen info
	rfbServerInitMsg	m_scrinfo;

	// These are the red, green & blue masks for a pixel
	DWORD			m_rMask, m_gMask, m_bMask;

	// This is always handy to have
	int				m_bytesPerRow;

	// Handle of the default cursor
	HCURSOR			m_hcursor;
	// Handle of the basic arrow cursor
	HCURSOR			m_hdefcursor;
	// The mousemoved flag & current mouse position
	BOOL			m_cursormoved;
	RECT			m_cursorpos;

	// Boolean flag to indicate when the display resolution has changed
	BOOL			m_displaychanged;

	// Extra vars used for the DIBsection optimisation
	VOID			*m_DIBbits;
	BOOL			m_formatmunged;

	DEVMODE			*lpDevMode; // *** used for res changes - Jeremy Peaks
	long			origPelsWidth; // *** To set the original resolution
	long			origPelsHeight; // *** - Jeremy Peaks
};

#endif // _WINrd_rdDESKTOP
