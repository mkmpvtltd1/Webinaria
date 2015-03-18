#include "StdAfx.h"
#include "Error.h"

namespace WebinariaApplication
{
	// Default constructor
	CError::CError(void)
	{
	}

	// Virtual destructor
	CError::~CError(void)
	{
	}

	// Opens a Message box with a error message in it.  The user can select the OK button to continue
	void CError::ErrMsg(HWND hMainWnd,LPTSTR szFormat,...)
	{
		static TCHAR szBuffer[2048]={0};
		const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
		const int LASTCHAR = NUMCHARS - 1;

		// Format the input string
		va_list pArgs;
		va_start(pArgs, szFormat);

		// Use a bounded buffer size to prevent buffer overruns.  Limit count to
		// character size minus one to allow for a NULL terminating character.
		_vsntprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
		va_end(pArgs);

		// Ensure that the formatted string is NULL-terminated
		szBuffer[LASTCHAR] = TEXT('\0');

		MessageBox(hMainWnd, szBuffer, L"Error", MB_TASKMODAL );
	}
}