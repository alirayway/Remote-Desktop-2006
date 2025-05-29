

// rdHTTPConnect.cpp

// Implementation of the HTTP server class

#include "stdhdrs.h"
#include "VSocket.h"
#include "rdHTTPConnect.h"
#include "rdServer.h"
#include <omnithread.h>
#include "resource.h"

// HTTP messages / message formats
const char HTTP_MSG_OK[] = "HTTP/1.0 200 OK\n\n";

const char HTTP_FMT_INDEX[] =
"<HTML>\n"
"  <HEAD><TITLE>Tightrd desktop [%.256s]</TITLE></HEAD>\n"
"  <BODY>\n"
"    <APPLET CODE=rdViewer.class ARCHIVE=rdViewer.jar WIDTH=%d HEIGHT=%d>\n"
"      <PARAM NAME=\"PORT\" VALUE=\"%d\">\n"
"%.1024s"
"    </APPLET><BR>\n"
"    <A HREF=\"http://www.tightrd.com/\">www.Tightrd.com</A>\n"
"  </BODY>\n"
"</HTML>\n";

const char HTTP_MSG_NOSOCKCONN [] =
"<HTML>\n"
"  <HEAD><TITLE>Tightrd desktop</TITLE></HEAD>\n"
"  <BODY>\n"
"    <H1>Connections Disabled</H1>\n"
"    The requested desktop is not configured to accept incoming connections.\n"
"  </BODY>\n"
"</HTML>\n";

const char HTTP_MSG_BADPARAMS [] =
"<HTML>\n"
"  <HEAD><TITLE>Tightrd desktop</TITLE></HEAD>\n"
"  <BODY>\n"
"    <H1>Bad Parameters</H1>\n"
"    The sequence of applet parameters specified within the URL is invalid.\n"
"  </BODY>\n"
"</HTML>\n";

const char HTTP_MSG_NOSUCHFILE [] =
"HTTP/1.0 404 Not Found\n\n"
"<HTML>\n"
"  <HEAD><TITLE>404 Not Found</TITLE></HEAD>\n"
"  <BODY>\n"
"    <H1>Not Found</H1>\n"
"    The requested file could not be found.\n"
"  </BODY>\n"
"</HTML>\n";

// Filename to resource ID mappings for the Java class files:
typedef struct _FileToResourceMap {
	char *filename;
	char *type;
	int resourceID;
} FileMap;

const FileMap filemapping [] = {
	{"/rdViewer.jar", "JavaArchive", IDR_rdVIEWER_JAR},
	{"/AuthPanel.class", "JavaClass", IDR_AUTHPANEL_CLASS},
	{"/ClipboardFrame.class", "JavaClass", IDR_CLIPBOARDFRAME_CLASS},
	{"/DesCipher.class", "JavaClass", IDR_DESCIPHER_CLASS},
	{"/OptionsFrame.class", "JavaClass", IDR_OPTIONSFRAME_CLASS},
	{"/RfbProto.class", "JavaClass", IDR_RFBPROTO_CLASS},
	{"/rdCanvas.class", "JavaClass", IDR_rdCANVAS_CLASS},
	{"/rdViewer.class", "JavaClass", IDR_rdVIEWER_CLASS},
	{"/ButtonPanel.class", "JavaClass", IDR_BUTTONPANEL_CLASS},
	{"/RecordingFrame.class", "JavaClass", IDR_RECFRAME_CLASS},
	{"/SessionRecorder.class", "JavaClass", IDR_SESSIONREC_CLASS},
	{"/ReloginPanel.class", "JavaClass", IDR_RELOGINPANEL_CLASS},
	{"/SocketFactory.class", "JavaClass", IDR_SOCKFACTORY_CLASS},
	{"/HTTPConnectSocket.class", "JavaClass", IDR_HTTPCONNSOCK_CLASS},
	{"/HTTPConnectSocketFactory.class", "JavaClass", IDR_HTTPCONNSOCKFACTORY_CLASS}
};
const int filemappingsize		= 15;

//
// Connection thread -- one per each client connection.
//

class rdHTTPConnectThread : public omni_thread
{
public:
	// Init routine
	virtual BOOL Init(VSocket *socket, rdServer *server, BOOL allow_params);

	// Code to be executed by the thread
	virtual void run(void *arg);

	// Routines to handle HTTP requests
	virtual void DoHTTP(VSocket *socket);
	virtual BOOL ParseParams(const char *request, char *result, int max_bytes);
	BOOL ValidateString(char *str);
	virtual char *ReadLine(VSocket *socket, char delimiter, int max);

protected:
	// Fields used internally
	rdServer	*m_server;
	VSocket		*m_socket;
	BOOL		m_allow_params;
};

// Method implementations
BOOL rdHTTPConnectThread::Init(VSocket *socket, rdServer *server,
								BOOL allow_params)
{
	m_server = server;
	m_socket = socket;
	m_allow_params = allow_params;

	// Start the thread
	start();

	return TRUE;
}

// Code to be executed by the thread
void rdHTTPConnectThread::run(void *arg)
{
	rdlog.Print(LL_INTINFO, rdLOG("started HTTP client thread\n"));

	// Perform the transaction
	DoHTTP(m_socket);

	// And close the client
	m_socket->Shutdown();
	m_socket->Close();
	delete m_socket;

	rdlog.Print(LL_INTINFO, rdLOG("quitting HTTP client thread\n"));
}

void rdHTTPConnectThread::DoHTTP(VSocket *socket)
{
	char filename[1024];
	char *line;

	// Read in the HTTP header
	if ((line = ReadLine(socket, '\n', 1024)) == NULL)
		return;

	// Scan the header for the filename and free the storage
	int result = sscanf(line, "GET %s HTTP/", filename);
	delete [] line;
	if (result != 1)
		return;

	rdlog.Print(LL_CLIENTS, rdLOG("file %s requested\n"), filename);

	// Read in the rest of the browser's request data and discard...
	BOOL newline = TRUE;

	char c;
	for (;;) {
		if (!socket->ReadExact(&c, 1))
			return;
		if (c == '\n') {
			if (newline)
				break;
			newline = TRUE;
		} else {
			if (c >= ' ')
				newline = FALSE;
		}
	}

	rdlog.Print(LL_INTINFO, rdLOG("HTTP headers skipped\n"));

    if (filename[0] != '/') {
		rdlog.Print(LL_CONNERR, rdLOG("filename didn't begin with '/'\n"));
		socket->SendExact(HTTP_MSG_NOSUCHFILE, strlen(HTTP_MSG_NOSUCHFILE));
		return;
	}

	// Switch, dependent upon the filename:
	if (filename[1] == '\0' || (m_allow_params && filename[1] == '?'))
	{
		char indexpage[2048];

		rdlog.Print(LL_CLIENTS, rdLOG("sending main page\n"));
		if (filename[1] == '?')
			rdlog.Print(LL_INTWARN, rdLOG("applet parameters specified\n"));

		// Send the OK notification message to the client
		if (!socket->SendExact(HTTP_MSG_OK, strlen(HTTP_MSG_OK)))
			return;

		// Compose the index page
		if (m_server->SockConnected())
		{
			int width, height, depth;

			// Get the screen's dimensions
			m_server->GetScreenInfo(width, height, depth);

			// Get the name of this desktop
			char desktopname[MAX_COMPUTERNAME_LENGTH + 1];
			DWORD desktopnamelen = MAX_COMPUTERNAME_LENGTH + 1;
			if (GetComputerName(desktopname, &desktopnamelen))
			{
				// Make the name lowercase
				for (size_t x = 0; x < strlen(desktopname); x++)
				{
					desktopname[x] = tolower(desktopname[x]);
				}
			}
			else
			{
				strcpy(desktopname, "Winrd");
			}

			// Parse the applet parameters if specified within URL
			char params[1024] = "";
			if (filename[1] == '?') {
				if (!ParseParams(&filename[2], params, 1024)) {
					socket->SendExact(HTTP_MSG_BADPARAMS,
									  strlen(HTTP_MSG_BADPARAMS));
					return;
				}
			}

			// Send the java applet page
			sprintf(indexpage, HTTP_FMT_INDEX,
					desktopname, width, height+32, m_server->GetPort(), params);
		}
		else
		{
			// Send a "sorry, not allowed" page
			sprintf(indexpage, HTTP_MSG_NOSOCKCONN);
		}

		// Send the page
		if (socket->SendExact(indexpage, strlen(indexpage)))
			rdlog.Print(LL_INTINFO, rdLOG("sent page\n"));

		return;
	}

	// File requested was not the index so check the mappings
	// list for a different file.

	// Now search the mappings for the desired file
	for (int x=0; x < filemappingsize; x++)
	{
		if (strcmp(filename, filemapping[x].filename) == 0)
		{
			HRSRC resource;
			HGLOBAL resourcehan;
			char *resourceptr;
			int resourcesize;

			rdlog.Print(LL_INTINFO, rdLOG("requested file recognised\n"));

			// Find the resource here
			resource = FindResource(NULL,
					MAKEINTRESOURCE(filemapping[x].resourceID),
					filemapping[x].type
					);
			if (resource == NULL)
				return;

			// Get its size
			resourcesize = SizeofResource(NULL, resource);

			// Load the resource
			resourcehan = LoadResource(NULL, resource);
			if (resourcehan == NULL)
				return;

			// Lock the resource
			resourceptr = (char *)LockResource(resourcehan);
			if (resourceptr == NULL)
				return;

			rdlog.Print(LL_INTINFO, rdLOG("sending file...\n"));

			// Send the OK message
			if (!socket->SendExact(HTTP_MSG_OK, strlen(HTTP_MSG_OK)))
				return;

			// Now send the entirety of the data to the client
			if (!socket->SendExact(resourceptr, resourcesize))
				return;

			rdlog.Print(LL_INTINFO, rdLOG("file successfully sent\n"));

			return;
		}
	}

	// Send the NoSuchFile notification message to the client
	if (!socket->SendExact(HTTP_MSG_NOSUCHFILE, strlen(HTTP_MSG_NOSUCHFILE)))
		return;
}

//
// Parse the request tail after the '?' character, and format a sequence
// of <param> tags for the index HTML page with embedded applet.
//

BOOL rdHTTPConnectThread::ParseParams(const char *request,
									   char *result, int max_bytes)
{
	char param_request[128];
	char param_formatted[196];

	result[0] = '\0';
	int cur_bytes = 0;

	const char *tail = request;
	for (;;) {
		// Copy individual "name=value" string into a buffer
		char *delim_ptr = strchr((char *)tail, '&');
		if (delim_ptr == NULL) {
			if (strlen(tail) >= sizeof(param_request)) {
				return FALSE;
			}
			strcpy(param_request, tail);
		} else {
			int len = delim_ptr - tail;
			if (len >= sizeof(param_request)) {
				return FALSE;
			}
			memcpy(param_request, tail, len);
			param_request[len] = '\0';
		}

		// Split the request into parameter name and value
		char *value_str = strchr(&param_request[1], '=');
		if (value_str == NULL) {
			return FALSE;
		}
		*value_str++ = '\0';
		if (strlen(value_str) == 0) {
			return FALSE;
		}

		// Validate both parameter name and value
		if (!ValidateString(param_request) || !ValidateString(value_str)) {
			return FALSE;
		}

		// Prepare HTML-formatted representation of the name=value pair
		int len = sprintf(param_formatted,
						  "      <PARAM NAME=\"%s\" VALUE=\"%s\">\n",
						  param_request, value_str);
		if (cur_bytes + len + 1 > max_bytes) {
			return FALSE;
		}
		strcat(result, param_formatted);
		cur_bytes += len;

		// Go to the next parameter
		if (delim_ptr == NULL) {
			break;
		}
		tail = delim_ptr + 1;
	}
	return TRUE;
}

//
// Check if the string consists only of alphanumeric characters, '+' signs,
// underscores, and dots. Replace all '+' signs with spaces.
//

BOOL rdHTTPConnectThread::ValidateString(char *str)
{
	for (char *ptr = str; *ptr != '\0'; ptr++) {
		if (!isalnum(*ptr) && *ptr != '_' && *ptr != '.') {
			if (*ptr == '+') {
				*ptr = ' ';
			} else {
				return FALSE;
			}
		}
	}
	return TRUE;
}

char *rdHTTPConnectThread::ReadLine(VSocket *socket, char delimiter, int max)
{
	// Allocate the maximum required buffer
	char *buffer = new char[max+1];
	int buffpos = 0;

	// Read in data until a delimiter is read
	for (;;)
	{
		char c;

		if (!socket->ReadExact(&c, 1))
		{
			delete [] buffer;
			return NULL;
		}

		if (c == delimiter)
		{
			buffer[buffpos] = 0;
			return buffer;
		}

		buffer[buffpos] = c;
		buffpos++;

		if (buffpos == (max-1))
		{
			buffer[buffpos] = 0;
			return buffer;
		}
	}
}

//
// Listening thread.
//

class rdHTTPListenThread : public omni_thread
{
public:
	// Init routine
	virtual BOOL Init(VSocket *socket, rdServer *server, BOOL allow_params);

	// Code to be executed by the thread
	virtual void *run_undetached(void * arg);

	// Fields used internally
	BOOL		m_shutdown;

protected:
	rdServer	*m_server;
	VSocket		*m_listen_socket;
	BOOL		m_allow_params;
};

BOOL rdHTTPListenThread::Init(VSocket *listen_socket, rdServer *server,
							   BOOL allow_params)
{
	m_server = server;
	m_listen_socket = listen_socket;
	m_allow_params = allow_params;

	// Start the thread
	m_shutdown = FALSE;
	start_undetached();

	return TRUE;
}

// Code to be executed by the thread
void *rdHTTPListenThread::run_undetached(void * arg)
{
	rdlog.Print(LL_INTINFO, rdLOG("started HTTP server thread\n"));

	// Go into a loop, listening for connections on the given socket
	for (;;) {
		// Accept an incoming connection
		VSocket *new_socket = m_listen_socket->Accept();
		if (new_socket == NULL || m_shutdown)
			break;

		// Start a client thread for this connection
		rdlog.Print(LL_CLIENTS, rdLOG("HTTP client connected\n"));
		omni_thread *m_thread = new rdHTTPConnectThread;
		if (m_thread == NULL)
			break;
		((rdHTTPConnectThread *)m_thread)->Init(new_socket, m_server,
												 m_allow_params);
	}

	rdlog.Print(LL_INTINFO, rdLOG("quitting HTTP server thread\n"));

	return NULL;
}

//
// The rdSockConnect class implementation
//

rdHTTPConnect::rdHTTPConnect()
{
	m_listen_thread = NULL;
}

BOOL rdHTTPConnect::Init(rdServer *server, UINT listen_port, BOOL allow_params)
{
	// Save the port number
	m_listen_port = listen_port;

	// Create the listening socket
	if (!m_listen_socket.Create())
		return FALSE;

	// Bind it
	if (!m_listen_socket.Bind(m_listen_port, server->LoopbackOnly()))
		return FALSE;

	// Set it to listen
	if (!m_listen_socket.Listen())
		return FALSE;

	// Create the new thread
	m_listen_thread = new rdHTTPListenThread;
	if (m_listen_thread == NULL)
		return FALSE;

	// And start it running
	return ((rdHTTPListenThread *)m_listen_thread)->Init(&m_listen_socket, server,
														  allow_params);
}

rdHTTPConnect::~rdHTTPConnect()
{
	m_listen_socket.Shutdown();

	// Join with our lovely thread
	if (m_listen_thread != NULL) {
		// *** This is a hack to force the listen thread out of the accept call,
		// because Winsock accept semantics are broken.
		((rdHTTPListenThread *)m_listen_thread)->m_shutdown = TRUE;

		VSocket tmp_socket;
		tmp_socket.Create();
		tmp_socket.Connect("localhost", m_listen_port);
		tmp_socket.Close();

		void *returnval;
		m_listen_thread->join(&returnval);
		m_listen_thread = NULL;

		m_listen_socket.Close();
    }
}

