// Webinaria.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Application.h"

#include "Registrar.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if (FindWindowExW(NULL,NULL,L"WUI::CustomWindow",NULL))
		return 0;

	//	Check for command line parameters.
	int argc;
	LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (0 != args)
	{
		bool exit = true;

		if (argc > 1)
		{
			if (!wcsicmp(args[1], L"/register"))
				Registrar::RegisterFilters();
			else if (!wcsicmp(args[1], L"/unregister"))
				Registrar::UnregisterFilters();
			else
				exit = false;
		}
		else
			exit = false;

		LocalFree(args);

		if (exit)
			return 0;
	}

	WebinariaApplication::CApplication MainApp(hInstance,nCmdShow);
	if (!MainApp.m_Running)
		return 0;

 	return MainApp.MessageLoop();
}