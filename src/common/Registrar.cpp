#include "StdAfx.h"
#include "Registrar.h"

#include <winsvc.h>
#include <shlwapi.h>
#include "Folders.h"

typedef HRESULT (__stdcall *DllServerRegistrationFunc)();

bool Registrar::UserHasAdminRights()
{
    SC_HANDLE hSCMan = OpenSCManager(0, 0, GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE);

	bool result = (NULL != hSCMan);
    if (result)
        CloseServiceHandle(hSCMan);

    return result;
}

void Registrar::UpdateServerRegistration(const TCHAR *serverPath, bool registerServer/* = true*/)
{
	bool userIsAdmin = UserHasAdminRights();

	//
	//	Registry key override happens only in case if the user is not admin.
	//
	HKEY hClasses;
	if (userIsAdmin || ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, L"Software\\Classes", &hClasses))
	{
		bool restoreClasses = (!userIsAdmin && ERROR_SUCCESS == RegOverridePredefKey(HKEY_CLASSES_ROOT, hClasses));

		HMODULE filterLib = LoadLibrary(serverPath);
		if (0 != filterLib)
		{
			char *registrationFuncName = (registerServer) ? "DllRegisterServer" : "DllUnregisterServer";
			DllServerRegistrationFunc registrationFunc = 
				(DllServerRegistrationFunc)GetProcAddress(filterLib, registrationFuncName);

			if (0 != registrationFunc)
				registrationFunc();

			FreeLibrary(filterLib);
		}

		if (restoreClasses)
			RegOverridePredefKey(HKEY_CLASSES_ROOT, 0);

		if (!userIsAdmin)
			RegCloseKey(hClasses);
	}
}

void Registrar::UpdateFiltersRegistration(bool registerFilters/* = true*/)
{
	CoInitialize(0);

	TCHAR path[MAX_PATH];
	TCHAR* filters[] = { _T("RmBkFilter.dll"), _T("UScreenCapture.ax") };

	for (int i = 0; i < _countof(filters); i++)
	{
		PathCombine(path, Folders::My(), filters[i]);
		UpdateServerRegistration(path, registerFilters);
	}

	CoUninitialize();
}

void Registrar::RegisterFilters()
{
	UpdateFiltersRegistration();
}

void Registrar::UnregisterFilters()
{
	UpdateFiltersRegistration(false);
}
