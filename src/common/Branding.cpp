#include "StdAfx.h"
#include "Branding.h"

#include "Folders.h"

const TCHAR *g_KeyBrandingRegistry = _T("SOFTWARE\\Webinaria");
const TCHAR *g_ValDataFolder = _T("DataFolder");
const TCHAR *g_ValHomepageUrl = _T("HomepageUrl");
const TCHAR *g_ValHelpUrl = _T("HelpUrl");
const TCHAR *g_ValUpdateUrl = _T("UpdateUrl");
const TCHAR *g_ValDownloadUpdatesUrl = _T("DownloadUpdatesUrl");
const TCHAR *g_ValPublishUrl = _T("PublishUrl");
const TCHAR *g_ValRecorderWindowTitle = _T("RecorderWindowTitle");
const TCHAR *g_ValEditorWindowTitle = _T("EditorWindowTitle");

const TCHAR *g_DataFolderDefault = _T("Webinaria Files");
const TCHAR *g_HomepageUrlDefault = _T("http://www.webinaria.com");
const TCHAR *g_HelpUrlDefault = _T("http://www.webinaria.com/forum/");
const TCHAR *g_UpdateUrlDefault = _T("http://www.webinaria.com/version.txt");
const TCHAR *g_DownloadUpdatesUrlDefault = _T("http://www.webinaria.com/record.php");
const TCHAR *g_PublishUrlDefault = _T("http://www.webinaria.com/publish.php");
const TCHAR *g_RecorderWindowTitleDefault = _T("Webinaria");
const TCHAR *g_EditorWindowTitleDefault = _T("Webinaria Editor");

Branding::Branding()
{
	HKEY key = 0;
	bool needToCloseKey = 
		(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_KeyBrandingRegistry, 0, KEY_READ, &key));

	ReadStringFromRegistry(key, g_ValDataFolder, &dataFolder, g_DataFolderDefault);
	ReadStringFromRegistry(key, g_ValHomepageUrl, &homepageUrl, g_HomepageUrlDefault);
	ReadStringFromRegistry(key, g_ValHelpUrl, &helpUrl, g_HelpUrlDefault);
	ReadStringFromRegistry(key, g_ValUpdateUrl, &updateUrl, g_UpdateUrlDefault);
	ReadStringFromRegistry(key, g_ValDownloadUpdatesUrl, &downloadUpdatesUrl, g_DownloadUpdatesUrlDefault);
	ReadStringFromRegistry(key, g_ValPublishUrl, &publishUrl, g_PublishUrlDefault);
	ReadStringFromRegistry(key, g_ValRecorderWindowTitle, &recorderWindowTitle, g_RecorderWindowTitleDefault);
	ReadStringFromRegistry(key, g_ValEditorWindowTitle, &editorWindowTitle, g_EditorWindowTitleDefault);

	Folders::FromMine(mainIconPath, _T("app.ico"));
	Folders::FromMine(trayIconPath, _T("tray.ico"));
	Folders::FromRecorderUI(videoSmallLogoPath, _T("videologosmall.bmp"));
	Folders::FromRecorderUI(videoLargeLogoPath, _T("videologolarge.bmp"));

	if (needToCloseKey)
		RegCloseKey(key);
}

Branding::~Branding()
{
    delete[] dataFolder;
    delete[] homepageUrl;
    delete[] helpUrl;
    delete[] downloadUpdatesUrl;
    delete[] updateUrl;
    delete[] publishUrl;
    delete[] recorderWindowTitle;
    delete[] editorWindowTitle;
}

Branding* Branding::Instance()
{
	static Branding instance;
	return &instance;
}

void Branding::ReadStringFromRegistry(HKEY key, const TCHAR *valueName, TCHAR **value, const TCHAR *defaultValue)
{
	bool getDefaultValue = true;

	if (key)
	{
		DWORD bufferSize = 0, type = REG_SZ;
		if (ERROR_SUCCESS == RegQueryValueEx(key, valueName, 0, &type, 0, &bufferSize) && REG_SZ == type)
		{
			*value = new TCHAR[bufferSize];
			if (ERROR_SUCCESS == RegQueryValueEx(key, valueName, 0, &type, (LPBYTE)*value, &bufferSize))
				getDefaultValue = false;
			else
				delete[] (*value);
		}
	}

	if (getDefaultValue)
	{
		size_t length = _tcslen(defaultValue) + 1;
		*value = new TCHAR[length];
		_tcscpy_s(*value, length, defaultValue);
	}
}

bool Branding::FileExists(const TCHAR *fileName) const
{
	HANDLE file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE != file)
	{
		CloseHandle(file);
		return true;
	}

	return false;
}

bool Branding::IsVideoLogoBranded() const
{
	return FileExists(VideoSmallLogoPath()) && FileExists(VideoLargeLogoPath());
}
