#include "stdafx.h"

#include <malloc.h>

#include "VersionInfo.h"
#pragma comment(lib, "version.lib")

namespace VersionInfo
{

OSVERSIONINFO& WindowsVersion()
{
    static bool Initialized = false;
    static OSVERSIONINFO g_WindowsVersion = { 0 };

    if (!Initialized)
    {
        //  Initialize Windows version information
        g_WindowsVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		Initialized = GetVersionEx(&g_WindowsVersion) ? true : false;
    }

    return g_WindowsVersion;
}

MODULEVERSION& ModuleVersion(HINSTANCE instance/* = 0*/)
{
    static bool Initialized = false;
    static MODULEVERSION g_ModuleVersion = { 0 };

    if (!Initialized)
    {
        //  Initialize Module version information
        TCHAR szFileName[MAX_PATH];
        DWORD dwLen;
        BOOL bGotModuleVersion = FALSE;
        if ((dwLen = GetModuleFileName(static_cast<HMODULE>(instance), szFileName, sizeof(szFileName))))
        {
            szFileName[dwLen] = '\0';
            DWORD dwHandle, dwInfoLen;
            if ((dwInfoLen = GetFileVersionInfoSize(szFileName, &dwHandle)))
            {
                char *szVersionBuffer = (char *)malloc(dwInfoLen);
                if (GetFileVersionInfo(szFileName, dwHandle, dwInfoLen, szVersionBuffer))
                {
                    VS_FIXEDFILEINFO *fi;
                    UINT uBlockLen;
                    if (VerQueryValue(szVersionBuffer, _T("\\"), 
                        reinterpret_cast<LPVOID*>(&fi), &uBlockLen))
                    {
                        Initialized = true;
                        g_ModuleVersion.Major = fi->dwProductVersionMS >> 16;
                        g_ModuleVersion.Minor = fi->dwProductVersionMS & 0xff;
                        g_ModuleVersion.Release = fi->dwProductVersionLS >> 16;
                        g_ModuleVersion.Build = fi->dwProductVersionLS & 0xff;
                        //  Get module path
                        g_ModuleVersion.ModuleFullPath = szFileName;
                        TCHAR *eop = _tcsrchr(szFileName, _T('\\'));            //  end of path
                        g_ModuleVersion.ModuleFileName = CharNext(eop);         //  beginning of the file name
                        *CharNext(eop) = _T('\0');
                        g_ModuleVersion.ModulePath = szFileName;
                    }
                }
                free(szVersionBuffer);
            }
        }
    }

    return g_ModuleVersion;
}

BOOL IsVersion(DWORD Major, DWORD Minor, DWORD dwPlatformId, bool Strict)
{
    if (Strict)
        return (WindowsVersion().dwMajorVersion == Major && WindowsVersion().dwMinorVersion == Minor && 
            WindowsVersion().dwPlatformId == dwPlatformId);
    else
        return (WindowsVersion().dwPlatformId == dwPlatformId && 
			(WindowsVersion().dwMajorVersion > Major || (WindowsVersion().dwMajorVersion == Major && WindowsVersion().dwMinorVersion >= Minor)));
}

BOOL IsNTVersion(DWORD Major, DWORD Minor, bool Strict)
{
    return IsVersion(Major, Minor, VER_PLATFORM_WIN32_NT, Strict);
}

BOOL Is9xVersion(DWORD Major, DWORD Minor, bool Strict)
{
    return IsVersion(Major, Minor, VER_PLATFORM_WIN32_WINDOWS, Strict);
}

}
