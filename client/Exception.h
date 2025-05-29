

#ifndef EXCEPTION_H__
#define EXCEPTION_H__

#pragma once

// Exceptions used in rdviewer

class Exception  
{
public:
	Exception(const char *info = NULL);
	virtual void Report();
	virtual ~Exception();
	char *m_info;
};

// This indicates something that the catcher should close 
// the connection quietly.
// Report() will display a TRACE message
class QuietException : public Exception {
public:
	QuietException(const char *info = NULL);
	virtual void Report();
	virtual ~QuietException();
};

// This indicates something the user should be told about.
// In situations of ambiguity, the 'close' parameter can be used
// to specify whether or not the connection is closed as a result.
// In general it will be.
// Report() will display a message box
class WarningException : public Exception {
public:
	WarningException(const char *info = NULL, bool close = true);
	virtual void Report();
	virtual ~WarningException();
	bool m_close;
};

// This is serious stuff - similar to an assert - we may not use?
// Report will display an important message box. Connection definitely
// closed.
class ErrorException : public Exception {
public:
	ErrorException(const char *info = NULL);
	virtual void Report();
	virtual ~ErrorException();
};

// This indicates something the user should be told about.
// In situations of ambiguity, the 'close' parameter can be used
// to specify whether or not the connection is closed as a result.
// In general it will be.
// Report() will display a message box
class AuthException : public WarningException {
public:
	AuthException(const char *info = NULL, bool close = true);
	virtual void Report();
	virtual ~AuthException();
};

#endif // EXCEPTION_H__

