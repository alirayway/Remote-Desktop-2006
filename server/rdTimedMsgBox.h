
// rdTimedMsgBox

// Static class used to generate message boxes which allow the program's
// execution to continue after a set delay (4 seconds at present)

class rdTimedMsgBox;

#if(!defined(_WINrd_rdTIMEDMSGBOX))
#define _WINrd_rdTIMEDMSGBOX

class rdTimedMsgBox
{
public:
	// Bring up a message box, wait for two seconds, then return
	static void Do(const char *caption, const char *title, UINT type);
};

#endif

