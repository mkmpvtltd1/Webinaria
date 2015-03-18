#include "StdAfx.h"
#include "Application.h"

#include "Folders.h"
#include "Registrar.h"
#include "Branding.h"

namespace WebinariaApplication
{
	HINSTANCE CApplication::m_Instance = 0;

	HINSTANCE CApplication::Instance()
	{
		return m_Instance;
	}

	// Constructor
	CApplication::CApplication(HINSTANCE hInstance, int nCmdShow):	HInstance(HInstance),
																	CustomWindow(NULL),
																	DevEnum(NULL),
																	MainGrabber(NULL),
																	FlashIterator(0),
																	TrayIconState(true),
																	m_Running(false)
	{
		// Initialize COM
#ifdef DEBUG
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#else
		CoInitialize(NULL);
#endif

		m_Instance = hInstance;

		//	Ensure all needed filters are registered for current user.
		Registrar::RegisterFilters();

        //  Make sure folder for user data exists.
        Folders::EnsureHomeFolderExists();

		// Initialize global window class
		CCustomWindow::RegisterWindowClass(hInstance);

		// Create enumerator for capturing devices
		DevEnum = new CDeviceEnumerator(NULL);

		// Creating window
		CustomWindow = new CCustomWindow(300, 300, 371, 521, DevEnum, Branding::Instance()->RecorderWindowTitle());

		HWND hMainWnd = CustomWindow->GetHWND();

		DevEnum->SetMainWindow(hMainWnd);

		// If custom window is available
		if (hMainWnd)
		{
			// Show custom window
			ShowWindow(hMainWnd, nCmdShow);
			// Update custom window
			UpdateWindow(hMainWnd);
			// Hide control panel button, leave only tray button
			/*ShowWindow(hMainWnd, SW_HIDE); 
			UINT style= GetWindowLong(hMainWnd, GWL_EXSTYLE); 
			style |= WS_EX_TOOLWINDOW; 
			SetWindowLong(hMainWnd, GWL_EXSTYLE, style);
			ShowWindow(hMainWnd, SW_SHOWNORMAL);*/
		}

		// Load acceleration table
		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_WEBINARIA_ACC);		

		//	NOTE: grabber is always being created with logo filter disabled.
		//	Logo will be rendered on a final FLV file.
		MainGrabber = new CAudioVideoGrabber(hMainWnd, DevEnum, false);

		m_Running = true;
	}

	// Virtual destructor
	CApplication::~CApplication(void)
	{
		if (DevEnum != NULL)
			delete DevEnum;

		if (MainGrabber != NULL)
			delete MainGrabber;

		if (CustomWindow != NULL)
			delete CustomWindow;

		CoUninitialize();
	}

	// Main application message loop
	WPARAM CApplication::MessageLoop(void)
	{
		RecorderCommand RC;
		while (GetMessageW(&msg, NULL, 0, 0)) 
		{
			if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			RC = CRecorder::GetCurrentRecorderCommand();
			if (CRecorder::GetAdditionalFlag(AF_CHANGE_SIZE))
			{
				CustomWindow->ResetScreenParameters();
				CRecorder::SetAdditionalFlag(WL::AF_CHANGE_SIZE, false);
			}
			if (CRecorder::GetCurrentRecorderState() == RS_RECORDING)
			{
				if (FlashIterator == 10)
				{
					FlashIterator = 0;
					TrayIconState = !TrayIconState;
					CustomWindow->FlashTray(TrayIconState);
				}
				else
					FlashIterator++;
			}
			switch (RC)
			{
			case RC_RECORD:
				{
					//	Finding windows by title is dead wrong.
					HWND hWE = FindWindow(NULL, Branding::Instance()->EditorWindowTitle());
					if (NULL != hWE)
						SendMessage(hWE, WM_DESTROY, 0, 0);
					HWND hWP = FindWindow(NULL, L"Preview");
					if (NULL != hWP)
						SendMessage(hWP, WM_DESTROY, 0, 0);
					while ((NULL != FindWindow(NULL, Branding::Instance()->EditorWindowTitle())) || (NULL != FindWindow(NULL, L"Preview")))
					{}

					if (CRecorder::GetCurrentRecorderState() == RS_STOP)
					{
						MainGrabber->SetAudioCapture(CRecorder::GetAdditionalFlag(AF_CAP_AUDIO));
						MainGrabber->SetWebcamCapture(CRecorder::GetAdditionalFlag(AF_CAP_CAMERA));

						MainGrabber->SetFrameRate(CRecorder::GetFrameRate());
						//MainGrabber->SetRemoveBackground(CRecorder::GetAdditionalFlag(AF_REMOVE_BACK));

						MainGrabber->FreeCaptureFilters();

						bool goOnWithRecording = false;

						if (MainGrabber->InitCaptureFilters())
						{
							MainGrabber->SetScreenFrameRate(CRecorder::GetFrameRate());
							MainGrabber->SetScreenArea(CRecorder::GetLeft(), CRecorder::GetTop(), CRecorder::GetRight(), CRecorder::GetBottom());

							if (MainGrabber->BuildCaptureGraph())
							{
								if (CustomWindow->ShouldHideWindowWhenRecording())
									CustomWindow->Hide();

								if (MainGrabber->Start())
								{
									if (CRecorder::GetAdditionalFlag(AF_REMOVE_MOUSE))
									{
										RECT rc;
										GetWindowRect(GetDesktopWindow(), &rc);
										SetCursorPos(rc.right,rc.bottom);
										rc.top = rc.bottom;
										rc.left = rc.right;
										ClipCursor(&rc);
									}

									CRecorder::ProvideCommand();
									goOnWithRecording = true;
								}
							}
						}

						//  TODO: Need proper error message for the failure on the line below.
						if (!goOnWithRecording)
							SendMessage(msg.hwnd, WM_COMMAND, ID_STOP, 0);
					}
				}
				break;
			case RC_STOP:
				{
					ClipCursor(NULL);
					MainGrabber->Stop();
					MainGrabber->TearDownGraph();
					CRecorder::ProvideCommand();
					FlashIterator = 0;
					TrayIconState = true;
					CustomWindow->FlashTray(TrayIconState);
				}
				break;
			case RC_PAUSE:
				{
					MainGrabber->Pause();
					CRecorder::ProvideCommand();
				}
				break;
			case RC_RESUME:
				{
					MainGrabber->Resume();
					CRecorder::ProvideCommand();
				}
				break;
			case RC_CHANGE_DEVICE:
				{
					MainGrabber->FreeCaptureFilters();
					MainGrabber->InitCaptureFilters();
					CRecorder::ProvideCommand();
				}
				break;
			}
		}
		if (CRecorder::GetCurrentRecorderState() != RS_STOP)
		{
			MainGrabber->Stop();
			MainGrabber->TearDownGraph();
		}
		return msg.wParam;
	}
}