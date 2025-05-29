
#include "rdviewerApp.h"
#include "Daemon.h"
#include "Flasher.h"

class rdviewerApp32 : public rdviewerApp {
public:
	rdviewerApp32(HINSTANCE hInstance, PSTR szCmdLine);

	void NewConnection();
	void NewConnection(TCHAR *host, int port);
	void NewConnection(SOCKET sock);
	void NewConnection(TCHAR *configFile);

	~rdviewerApp32();
private:
	// Set up registry for program's sounds
	void RegisterSounds();
	Flasher *m_pflasher;
	Daemon  *m_pdaemon;
};

