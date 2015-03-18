#include "StdAfx.h"
#include "TrayIcon.h"

#include "resource.h"
#include "Branding.h"

namespace WebinariaApplication
{
	namespace WebinariaUserInterface
	{
		HMENU CTrayIcon::TrayContextMenu = 0;
		HINSTANCE CTrayIcon::HInstance = 0;										// Set default instance

		//////////////////////////////////////////////////////////////////////////
		//							Public methods								//
		//////////////////////////////////////////////////////////////////////////

		// Default constructor
		CTrayIcon::CTrayIcon(void)
		{
			IconData.cbSize				= sizeof(NOTIFYICONDATA);				
			IconData.uID				= 1;
			IconData.uCallbackMessage	= WM_TRAY_ICON_NOTIFY_MESSAGE;			// Create new message
			IconData.hIcon				= 0;
			IconData.szTip[0]			= 0;	
			IconData.uFlags				= NIF_MESSAGE;							// Icon data contains message
			IconData.hWnd				= 0;
		}
		
		// Constructor with associate window initialize
		CTrayIcon::CTrayIcon(HWND HWnd, HINSTANCE HI)
			: TrayIconVisible(false)
			, MinimizeToTray(true)
		{
			HInstance = HI;

			trayIcon = 
				(HICON)LoadImage(0, Branding::Instance()->TrayIconPath(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR);
			recordingIcon = LoadIcon(HInstance, MAKEINTRESOURCE(IDI_TRAY_FLASH));

			TrayContextMenu = LoadMenu(HInstance, MAKEINTRESOURCE(108));

			IconData.cbSize				= sizeof(NOTIFYICONDATA);	
			IconData.hWnd				= HWnd;									// Association main window
			IconData.uID				= 1;
			IconData.uFlags				= NIF_MESSAGE|NIF_ICON|NIF_TIP;			// Contain messges, icons 
																				// and tips
			IconData.uCallbackMessage	= WM_TRAY_ICON_NOTIFY_MESSAGE;			// Create new message
			IconData.hIcon				= trayIcon;
			wcscpy(IconData.szTip, Branding::Instance()->RecorderWindowTitle());					// Set tray tool tip
			Show();																// Display tray button
			Update();															// Update tray
		}

		// Virtual destructor
		CTrayIcon::~CTrayIcon(void)
		{
			Hide();																// Hide tray button before
																				// closing application
			if((IconData.hWnd)&&(IconData.uID>0)&&(TrayIconVisible))			
			{
				Shell_NotifyIcon(NIM_DELETE,&IconData);							// Delete our tray button
			}

			DestroyIcon(trayIcon);
			DestroyIcon(recordingIcon);
		}

		// Show tray button
		bool CTrayIcon::Show()
		{
			BOOL bSuccess = FALSE;
			if(!TrayIconVisible)												
			{	
				bSuccess = Shell_NotifyIcon(NIM_ADD,&IconData);					// Adding our button to tray
				TrayIconVisible = (bSuccess == TRUE);
			}
			return (bSuccess == TRUE);
		}

		// Hide tray button
		bool CTrayIcon::Hide()
		{
			BOOL bSuccess = FALSE;
			if(TrayIconVisible)
			{
				bSuccess = Shell_NotifyIcon(NIM_DELETE,&IconData);				// Delete our tray button
				TrayIconVisible = (bSuccess == FALSE);
			}
			return (bSuccess == TRUE);
		}

		// Flash tray button
		bool CTrayIcon::Flash(const bool & NormalState)
		{
			BOOL bSuccess = FALSE;
			if(TrayIconVisible)
			{
				if (NormalState)
					IconData.hIcon	= trayIcon;
				else
					IconData.hIcon	= recordingIcon;

				bSuccess = Shell_NotifyIcon(NIM_MODIFY,&IconData);				// Update tray with our data
			}
			return (bSuccess == TRUE);
		}

		// Update tray button
		bool CTrayIcon::Update()
		{
			BOOL bSuccess = FALSE;
			if(TrayIconVisible)
			{
				bSuccess = Shell_NotifyIcon(NIM_MODIFY,&IconData);				// Update tray with our data
			}
			return (bSuccess == TRUE);
		}

		// Notify messages pocedure
		UserActions CTrayIcon::NotifyMsgProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
		{
			UINT uID; 
			UINT uMsg; 

			uID = (UINT) wParam;							// Extract element ID
			uMsg = (UINT) lParam;							// Extract message

			if (uID != 1)
				return UA_INVALID_ACTION;
	
			POINT pt;										// Mouse position point

			switch (uMsg ) 
			{ 
			/*case WM_LBUTTONDOWN:							// Mouse left button down event handler
				return UA_RECORD_STOP;
				break;*/
			case WM_LBUTTONDBLCLK:							// Mouse left button double-click event handler
				WINDOWPLACEMENT wp;
				/*GetWindowPlacement(hWnd,&wp);
				if (wp.showCmd >= 1)							// If window is active
				{
					::SetForegroundWindow(hWnd);			// Set main window to foreground
					ShowWindow(hWnd,SW_HIDE);				// Hide main window
				}
				else*/
				{
					::SetForegroundWindow(hWnd);			// Set main window to foreground
					ShowWindow(hWnd,SW_RESTORE);			// Restore main window
				}
				break;
			case WM_RBUTTONDOWN:							// Mouse right button down event handler
			case WM_CONTEXTMENU:							// and context menu event handler
				GetCursorPos(&pt);							// Get mouse position
				::SetForegroundWindow(hWnd);				// Set main window to foreground
				TrackPopupMenu(	GetSubMenu(TrayContextMenu,0),
								TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,
								pt.x,
								pt.y,
								0,
								hWnd,
								NULL);						// Show popup menu at current mouse position
				PostMessage(hWnd ,WM_NULL, 0, 0);			// Post null message to main window
				break;
			} 
			return UA_NONE_ACTION; 
		}

		//////////////////////////////////////////////////////////////////////////
		//							Protected methods							//
		//////////////////////////////////////////////////////////////////////////
		// Set icon to tray button from resource
		void CTrayIcon::SetIcon(UINT ResourceIconName)
		{
			IconData.hIcon	= LoadIcon(HInstance, (LPCTSTR)ResourceIconName);// Load new icon to our button
		}

		// Set tool tip
		void CTrayIcon::SetToolTip(LPCWSTR lpwszToolTip)
		{
			wcscpy(IconData.szTip, lpwszToolTip);							 //	Set new tool tip
		}
	}
}