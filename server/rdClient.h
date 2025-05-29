

// rdClient.h

// rdClient class handles the following functions:
// - Recieves requests from the connected client and
//   handles them
// - Handles incoming updates properly, using a rdBuffer
//   object to keep track of screen changes
// It uses a rdBuffer and is passed the rdDesktop and
// rdServer to communicate with.

class rdClient;
typedef SHORT rdClientId;

#if (!defined(_WINrd_rdCLIENT))
#define _WINrd_rdCLIENT

#include <list>

typedef std::list<rdClientId> rdClientList;

// Includes
#include "stdhdrs.h"
#include "VSocket.h"
#include <omnithread.h>

// Custom
#include "rectlist.h"
#include "rdDesktop.h"
#include "rdRegion.h"
#include "rdBuffer.h"
#include "rdKeymap.h"

// The rdClient class itself

class rdClient
{
public:
	// Constructor/destructor
	rdClient();
	~rdClient();

	// Allow the client thread to see inside the client object
	friend class rdClientThread;

	// Init
	virtual BOOL Init(rdServer *server,
						VSocket *socket,
						BOOL auth,
						BOOL shared,
						rdClientId newid);

	// Kill
	// The server uses this to close the client socket, causing the
	// client thread to fail, which in turn deletes the client object
	virtual void Kill();

	// Client manipulation functions for use by the server
	virtual void SetBuffer(rdBuffer *buffer);

	// Update handling functions
	virtual void TriggerUpdate();
	virtual void UpdateMouse();
	virtual void UpdateRect(RECT &rect);
	virtual void UpdateRegion(rdRegion &region);
	virtual void CopyRect(RECT &dest, POINT &source);
	virtual void UpdateClipText(LPSTR text);
	virtual void UpdatePalette();

	// Functions for setting & getting the client settings
	virtual void SetTeleport(BOOL teleport) {m_teleport = teleport;};
	virtual void EnableKeyboard(BOOL enable) {m_keyboardenabled = enable;};
	virtual void EnablePointer(BOOL enable) {m_pointerenabled = enable;};
	virtual void SetCapability(int capability) {m_capability = capability;};

	virtual BOOL IsTeleport() {return m_teleport;};
	virtual BOOL IsKeyboardEnabled() {return m_keyboardenabled;};
	virtual BOOL IsPointerEnabled() {return m_pointerenabled;};
	virtual int GetCapability() {return m_capability;};
	virtual const char *GetClientName();
	virtual rdClientId GetClientId() {return m_id;};

	// Update routines
protected:
	BOOL SendUpdate();
	BOOL SendRFBMsg(CARD8 type, BYTE *buffer, int buflen);
	void CheckRects(rdRegion &rgn, rectlist &rects);
	void ClearRects(rdRegion &rgn, rectlist &rects);
	void GrabRegion(rdRegion &rgn);
	BOOL SendRectangles(rectlist &rects);
	BOOL SendRectangle(RECT &rect);
	BOOL SendCopyRect(RECT &dest, POINT &source);
	BOOL SendCursorShapeUpdate();
	BOOL SendCursorPosUpdate();
	BOOL SendLastRect();
	BOOL SendPalette();

	void PollWindow(HWND hwnd);

	// Internal stuffs
protected:
	// Per-client settings
	BOOL			m_teleport;
	BOOL			m_keyboardenabled;
	BOOL			m_pointerenabled;
	int				m_capability;
	BOOL			m_copyrect_use;
	rdClientId		m_id;

	// The screen buffer
	rdBuffer		*m_buffer;

	// The server
	rdServer		*m_server;

	// The socket
	VSocket			*m_socket;
	char			*m_client_name;

	// The client thread
	omni_thread		*m_thread;

	// Flag to indicate whether the client is ready for RFB messages
	BOOL			m_protocol_ready;

	// User input information
	RECT			m_oldmousepos;
	BOOL			m_mousemoved;
	rfbPointerEventMsg	m_ptrevent;
	rdKeymap		m_keymap;

	// Support for cursor shape updates (XCursor, RichCursor encodings)
	BOOL			m_cursor_update_pending;
	BOOL			m_cursor_update_sent;
	BOOL			m_cursor_pos_changed;
	HCURSOR			m_hcursor;
	POINT			m_cursor_pos;

	// Region structures used when preparing updates
	// - region of rects which may have changed since last update
	// - region for which incremental data is requested
	// - region for which full data is requested
	rdRegion		m_changed_rgn;
	rdRegion		m_incr_rgn;
	rdRegion		m_full_rgn;
	omni_mutex		m_regionLock;

	BOOL			m_copyrect_set;
	RECT			m_copyrect_rect;
	POINT			m_copyrect_src;

	BOOL			m_updatewanted;
	RECT			m_fullscreen;

	// When the local display is palettized, it sometimes changes...
	BOOL			m_palettechanged;

	// Information used in polling mode!
	RECT			m_qtrscreen;
	UINT			m_pollingcycle;
	BOOL			m_remoteevent;

	BOOL			m_use_PointerPos;
};

#endif
