#include "CustomWindow.h"

#pragma once

namespace EditorApplication
{
	using namespace EditorLogical;
	using namespace EditorUserInterface;

	// Class for creat, delete main application and handle message loop of main application
	class CApplication
	{
	// Public methods
	public:
		// Constructor
		CApplication(HINSTANCE hInstance, int nCmdShow, char * InFile);

		// Virtual destructor
		virtual ~CApplication(void);

		// Main application message loop
		WPARAM MessageLoop(void);

		bool m_Running;

	// Protected fields
	protected:
		HINSTANCE			HInstance;		// Instance of window
		CCustomWindow		* CustomWindow;	// Main user window
		MSG					msg;			// Global message queue message
		HACCEL				hAccelTable;	// Handle for acceleration table
		CViewer				* Viewer;		// Pointer to viewer class
		wchar_t				* File;			// Input file
	};
}
