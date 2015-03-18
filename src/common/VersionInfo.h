#pragma once

#include <string>

namespace VersionInfo
{

typedef struct tagMODULEVERSION
{
    DWORD Major, Minor;
    DWORD Release, Build;
    std::basic_string<TCHAR> ModulePath;
    std::basic_string<TCHAR> ModuleFileName;
    std::basic_string<TCHAR> ModuleFullPath;
} MODULEVERSION;

OSVERSIONINFO& WindowsVersion();
MODULEVERSION& ModuleVersion(HINSTANCE instance = 0);

BOOL IsNTVersion(DWORD Major, DWORD Minor, bool Strict = false);
BOOL Is9xVersion(DWORD Major, DWORD Minor, bool Strict = false);

}
