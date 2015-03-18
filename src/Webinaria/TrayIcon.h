#pragma once

#include <shellapi.h>

namespace WebinariaApplication
{
	namespace WebinariaUserInterface
	{
		// Enum with all based users actions
		enum UserActions{	UA_INVALID_ACTION = -2,
							UA_NONE_ACTION = -1,
							UA_RECORD = 0, 
							UA_PAUSE_RESUME = 1,
							UA_STOP = 2,
							UA_RECORD_STOP = 3,
							UA_HELP = 4,
							UA_UPDATE = 5,
							UA_EXIT = 6};

		// Our tray button message
		#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 50)

		// Class for application tray icon actions handling
		class CTrayIcon
		{
		private:
			HICON trayIcon, recordingIcon;

		// Public methods
		public:
			// Default constructor
			CTrayIcon(void);

			// Constructor with associate window initialize
			CTrayIcon(HWND HWnd,		// Handle of main window
					  HINSTANCE HI);	// Instance of main window

			// Virtual destructor
			virtual ~CTrayIcon(void);

			// Show tray button
			bool Show();

			// Hide tray button
			bool Hide();

			// Flash tray button
			bool Flash(const bool & NormalState);

			// Update tray button
			bool Update();

			// Notify messages pocedure
			static UserActions NotifyMsgProc(HWND hWnd, WPARAM wParam, LPARAM lParam);

			HMENU Menu() const { return CTrayIcon::TrayContextMenu; }
		
		// Protected methods
		protected:
			// Set icon to tray button from resource
			void SetIcon(UINT ResourceIconName);

			// Set tool tip
			void SetToolTip(LPCWSTR lpwszToolTip);

		// Protected fields
		protected:
			NOTIFYICONDATA IconData;
			static HMENU TrayContextMenu;
			static HINSTANCE HInstance;
			bool TrayIconVisible;
			bool MinimizeToTray;
		};
	}
}
	
