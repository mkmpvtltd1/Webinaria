#include "StdAfx.h"
#include "Application.h"

#include "Folders.h"
#include "Registrar.h"
#include "Branding.h"

namespace EditorApplication
{
	//////////////////////////////////////////////////////////////////////////
	//							Public methods								//
	//////////////////////////////////////////////////////////////////////////

	// Constructor
	CApplication::CApplication(HINSTANCE hInstance, int nCmdShow, char * InFile):	HInstance(HInstance),
																					CustomWindow(NULL),
																					Viewer(NULL),
																					File(NULL),
																					m_Running(false)
	{
		// Initialize COM
#ifdef DEBUG
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#else
		CoInitialize(NULL);
#endif

		InitCommonControls();

		//	Ensure all needed filters are registered for current user.
		Registrar::RegisterFilters();

        //  Make sure folder for user data exists.
        Folders::EnsureHomeFolderExists();

		// Initialize global window class
		CCustomWindow::RegisterWindowClass(hInstance);

		// Creating window
		CustomWindow = new CCustomWindow(300, 300, 750, 532, Branding::Instance()->EditorWindowTitle());

		HWND hMainWnd = CustomWindow->GetHWND(AW_MAIN);

		HWND hClipWnd = CustomWindow->GetHWND(AW_CLIP);

		// If custom window is available
		if (hMainWnd)
		{
			// Show custom window
			ShowWindow(hMainWnd, nCmdShow);
			// Update custom window
			UpdateWindow(hMainWnd);
		}

		Viewer = new CViewer(hMainWnd, hClipWnd);

		CustomWindow->SetViewer(Viewer);

		// Load acceleration table
		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_WEBINARIA_ACC);		

		File = new wchar_t[strlen(InFile)+1];
		mbstowcs(File,InFile,strlen(InFile)+1);
		if (wcscmp(File,L"") == 0)
		{
			delete File;
			File = NULL;
		}

		m_Running = true;

		CustomWindow->OpenFile(File);
	}

	// Virtual destructor
	CApplication::~CApplication(void)
	{
		if (File != NULL)
			delete[] File;
		if (Viewer != NULL)
			delete Viewer;
		CoUninitialize();
	}

	// Main application message loop
	WPARAM CApplication::MessageLoop(void)
	{
		while (GetMessageW(&msg, NULL, 0, 0)) 
		{
			if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		return msg.wParam;
	}
}