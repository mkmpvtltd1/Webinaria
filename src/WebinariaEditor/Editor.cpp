// Webinaria.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Application.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if (FindWindowExW(NULL,NULL,L"EUI::CustomWindow",NULL))
		return 0;

	EditorApplication::CApplication MainApp(hInstance,nCmdShow,lpCmdLine);
	if (!MainApp.m_Running)
		return 0;

	return MainApp.MessageLoop();
}