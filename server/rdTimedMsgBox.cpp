
// rdTimedMsgBox

// rdTimedMsgBox::Do spawns an omni-thread to draw the message
// box and wait a few seconds before returning, leaving the message-box displayed
// until Winrd quits.

#include "stdhdrs.h"
#include "omnithread.h"

#include "rdTimedMsgBox.h"

// The message-box delay
const UINT TIMED_MSGBOX_DELAY = 4000;

// The rdTimedMsgBoxThread class

class rdTimedMsgBoxThread : public omni_thread
{
public:
	rdTimedMsgBoxThread(const char *caption, const char *title, UINT type)
	{
		m_type = type;
		m_caption = strdup(caption);
		m_title = strdup(title);
	};
	virtual ~rdTimedMsgBoxThread()
	{
		if (m_caption != NULL)
			free(m_caption);
		if (m_title != NULL)
			free(m_title);
	};
	virtual void run(void *)
	{
		// Create the desired dialog box
		if (m_caption == NULL)
			return;
		MessageBox(NULL, m_caption, m_title, m_type | MB_OK);
	};
	char *m_caption;
	char *m_title;
	UINT m_type;
};

// The main rdTimedMsgBox class

void
rdTimedMsgBox::Do(const char *caption, const char *title, UINT type)
{
	// Create the thread object
	rdTimedMsgBoxThread *thread = new rdTimedMsgBoxThread(caption, title, type);
	if (thread == NULL)
		return;

	// Start the thread object
	thread->start();

	// And wait a few seconds
	Sleep(TIMED_MSGBOX_DELAY);
}
