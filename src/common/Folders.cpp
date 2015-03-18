#include "StdAfx.h"
#include "Folders.h"

#include "VersionInfo.h"
#include "Branding.h"

#include <shlobj.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

///
/// Folders.
///
const TCHAR* Folders::My()
{
    return VersionInfo::ModuleVersion().ModulePath.c_str();
}

const TCHAR* Folders::Home()
{
    static TCHAR path[MAX_PATH] = { 0 };
    static bool initialized = false;

    if (!initialized && 
        SUCCEEDED(SHGetFolderPath(0, CSIDL_PERSONAL, 0, SHGFP_TYPE_CURRENT, path)) && 
		PathAppend(path, Branding::Instance()->DataFolder()))
    {
        initialized = true;
    }

    return path;
}

bool Folders::BuildPathFromBase(TCHAR *path, const TCHAR *base, const TCHAR *subPath)
{
    _tcscpy(path, base);
    return PathAppend(path, subPath);
}

const TCHAR* Folders::RecorderUI()
{
    static TCHAR path[MAX_PATH] = { 0 };
    static bool initialized = false;

    if (!initialized && BuildPathFromBase(path, My(), _T("UI\\Recorder")))
        initialized = true;

    return path;
}

const TCHAR* Folders::EditorUI()
{
    static TCHAR path[MAX_PATH] = { 0 };
    static bool initialized = false;

    if (!initialized && BuildPathFromBase(path, My(), _T("UI\\Editor")))
        initialized = true;

    return path;
}

const TCHAR* Folders::FromMine(TCHAR *path, const TCHAR *subPath)
{
    BuildPathFromBase(path, My(), subPath);

    return path;
}

const TCHAR* Folders::FromHome(TCHAR *path, const TCHAR *subPath)
{
    BuildPathFromBase(path, Home(), subPath);

    return path;
}

const TCHAR* Folders::FromRecorderUI(TCHAR *path, const TCHAR *subPath)
{
    BuildPathFromBase(path, RecorderUI(), subPath);

    return path;
}

const TCHAR* Folders::FromEditorUI(TCHAR *path, const TCHAR *subPath)
{
    BuildPathFromBase(path, EditorUI(), subPath);

    return path;
}

bool Folders::EnsurePathExists(const TCHAR *path)
{
    return SUCCEEDED(SHCreateDirectoryEx(0, path, 0));
}

bool Folders::EnsureHomeFolderExists()
{
	if (EnsurePathExists(Home()))
	{
		//	Mark the folder as special.
		SetFileAttributes(Home(), FILE_ATTRIBUTE_SYSTEM);

		//	Create desktop.ini with path to application icon.
		TCHAR desktopIni[MAX_PATH] = { 0 };
		if (0 != PathCombine(desktopIni, Home(), _T("desktop.ini")))
		{
			TCHAR icon[MAX_PATH] = { 0 };
			if (0 != PathCombine(icon, My(), _T("app.ico")))
			{
				const TCHAR *Section = _T(".ShellClassInfo");

				WritePrivateProfileString(Section, _T("IconFile"), icon, desktopIni);
				WritePrivateProfileString(Section, _T("IconIndex"), _T("0"), desktopIni);
			}

			SetFileAttributes(desktopIni, FILE_ATTRIBUTE_HIDDEN);
		}
	}

	return false;
}
