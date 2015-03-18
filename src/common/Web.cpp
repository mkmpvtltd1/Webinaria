#include "StdAfx.h"
#include "Web.h"

#include <wininet.h>
#pragma comment (lib, "Wininet.lib")

#include <shellapi.h>

#include "Branding.h"

int Web::DownloadToString(const TCHAR *url, char *buffer, int bufSize)
{
	int result = -1;

	HINTERNET internet = InternetOpen(_T("Webinaria/1.0"), INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
	if (internet)
	{
		DWORD context = 0;
		HINTERNET hurl = InternetOpenUrl(internet, url, 0, 0, 
			INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_RESYNCHRONIZE, context);

		if (hurl)
		{
			ZeroMemory(buffer, bufSize);

			DWORD read = 0;
			result = (!InternetReadFile(hurl, buffer, bufSize, &read)) ? 0 : read;

			InternetCloseHandle(hurl);
		}

		InternetCloseHandle(internet);
	}

	return result;
}

bool Web::GetCurrentVersion(TCHAR *buffer, int bufSize)
{
	char version[1024];
	if (-1 != DownloadToString(Branding::Instance()->UpdateUrl(), version, _countof(version)))
	{
		CA2T version_t(version);
		_tcscpy_s(buffer, bufSize, version_t);

		//	Skip all chars which are non-numeric and not dots.
		TCHAR *p = buffer;
		while (*p)
		{
			if (!_tcschr(_T("01234567890."), *p))
			{
				*p = _T('\0');
				break;
			}

			p++;
		}

		return true;
	}

	return false;
}

void Web::OpenPage(const TCHAR *url)
{
	ShellExecute(0, _T("open"), url, 0, 0, SW_SHOWNORMAL);
}

void Web::OpenForums()
{
	OpenPage(Branding::Instance()->HelpUrl());
}

void Web::OpenUpdates()
{
	OpenPage(Branding::Instance()->DownloadUpdatesUrl());
}

void Web::OpenUploads()
{
	OpenPage(Branding::Instance()->PublishUrl());
}
