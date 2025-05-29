

#include "stdhdrs.h"
#include "Exception.h"

Exception::Exception(const char *info)
{
	assert(info != NULL);
	m_info = new char[strlen(info)+1];
	strcpy(m_info, info);
}

Exception::~Exception()
{
	delete [] m_info;
}

void Exception::Report()
{
	assert(false);
}

// ---------------------------------------


QuietException::QuietException(const char *info) : Exception(info)
{

}

QuietException::~QuietException()
{

}

void QuietException::Report()
{
#ifdef _MSC_VER
	_RPT1(_CRT_WARN, "Warning : %s\n", m_info);
#endif
}

// ---------------------------------------

WarningException::WarningException(const char *info, bool close) : Exception(info)
{
	m_close = close;
}

WarningException::~WarningException()
{

}

void WarningException::Report()
{
#ifdef _MSC_VER
	_RPT1(_CRT_WARN, "Warning : %s\n", m_info);
#endif
	MessageBox(NULL, m_info, "Tightrd info", MB_OK| MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);
}

// ---------------------------------------

ErrorException::ErrorException(const char *info) : Exception(info)
{

}

ErrorException::~ErrorException()
{

}

void ErrorException::Report()
{
#ifdef _MSC_VER
	_RPT1(_CRT_WARN, "Warning : %s\n", m_info);
#endif
	MessageBox(NULL, m_info, "Tightrd info", MB_OK | MB_ICONSTOP | MB_SETFOREGROUND | MB_TOPMOST);
}

// ---------------------------------------

AuthException::AuthException(const char *info, bool close) : WarningException(info)
{
	m_close = close;
}

AuthException::~AuthException()
{

}

void AuthException::Report()
{
#ifdef _MSC_VER
	_RPT1(_CRT_WARN, "Warning : %s\n", m_info);
#endif
	MessageBox(NULL, m_info, "Tightrd authentication info", MB_OK| MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TOPMOST);
}
