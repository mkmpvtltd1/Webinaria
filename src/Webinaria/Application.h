#include "CustomWindow.h"
#include "Recorder.h"
#include "TrayIcon.h"
#include "DeviceEnumerator.h"
#include "AudioVideoGrabber.h"
#include "Recorder.h"

#pragma once

namespace WebinariaApplication
{
	using namespace WebinariaLogical;
	using namespace WebinariaUserInterface;

	// Class for creat, delete main application and handle message loop of main application
	class CApplication
	{
	// Public methods
	public:
		// Constructor
		CApplication(HINSTANCE hInstance, int nCmdShow);

		// Virtual destructor
		virtual ~CApplication(void);

		// Main application message loop
		WPARAM MessageLoop(void);

		static HINSTANCE Instance();

		bool m_Running;

	// Protected fields
	protected:
		HINSTANCE								HInstance;		// Instance of window
		static HINSTANCE m_Instance;
		WUI::CCustomWindow						* CustomWindow;	// Main user window
		MSG										msg;			// Global message queue message
		HACCEL									hAccelTable;	// Handle for acceleration table
		CDeviceEnumerator						* DevEnum;		// Enumerator for all capture devices
		WL::CAudioVideoGrabber					* MainGrabber;	// Grabber for webcamera and audio
		int										FlashIterator;	// Iterate abstract time of tray flash
		bool									TrayIconState;	// True if state normal; false if was toggle to 
																// "gray flash"
	};
}
