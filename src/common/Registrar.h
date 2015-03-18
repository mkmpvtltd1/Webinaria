#pragma once

//
//	All filters are now being registered in a user mode.
//	Despite that, however, the program won't work under LUA properly, as
//	it needs r/w access to HKLM\SOFTWARE\Unreal\Live registry key.
//	W/o this kind of access, only desktop in whole could be grabbed.
//	This problem is addressed in setup script which trys to give everyone
//	full permissions on this key, but in any case, at least development under LUA
//	is possible w/o hassles.
//
class Registrar
{

private:
	Registrar() { }

	static bool UserHasAdminRights();

	static void UpdateServerRegistration(const TCHAR *serverPath, bool registerServer = true);
	static void UpdateFiltersRegistration(bool registerFilters = true);

public:
	static void RegisterFilters();							//	Registers filters which are required by Webinaria.
	static void UnregisterFilters();						//	Unregisters filters upon uninstallation.

};
