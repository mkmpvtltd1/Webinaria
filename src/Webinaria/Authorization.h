#pragma once

#pragma once

#include "htmlayout.h"
#include "Error.h"
#include "resource.h"

namespace WebinariaApplication
{
	namespace WebinariaUserInterface
	{
		// Class for get, vizualization and set default microphone parameters
		class CAuthorization:	public htmlayout::event_handler,
								public htmlayout::notification_handler<CAuthorization>
		{
		// Public methods
		public:
			// Constructor
			CAuthorization(HWND hWnd, HINSTANCE hInstance, const wchar_t * Caption);
		
			// Default destructor
			virtual ~CAuthorization(void);

			// Return associated data
			static CAuthorization* self(HWND hWnd) 
			{ 
				return (CAuthorization*)::GetWindowLongPtr(hWnd,GWLP_USERDATA); 
			}
			
			// Register authorization window class
			ATOM RegisterWindowClass(HINSTANCE hInstance);
			
			// Show dialog window in popup state
			void ShowPopupDialog();
			
			// Get login and password
			bool GetLoginPassword(wchar_t * login, wchar_t * password);

		// Protected methods
		protected:
			// Set window caption
			void SetWindowCaption();

			// Test HTMLayount controls under mouse pouinter
			int HitTest( const int & x, const int & y ) const;

			// Return associated data
			static void self(HWND hWnd, CAuthorization * Inst) 
			{ 
				::SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(Inst)); 
			}

			// Get root DOM element of the HTMLayout document
			HELEMENT Root() const;

			// Handle HTMLayout messages
			BOOL on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason);

			// Handle dialog box messages
			static LRESULT CALLBACK AuthDlgWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
		// Public fields
		public:
			static HANDLE							EnterEvent;
	
		// Protected fields
		protected:
			static HMODULE							hUser32;					// user32.dll module
			// For transparent coners
			static lpfnSetLayeredWindowAttributes	SetLayeredWindowAttributes;
			static HINSTANCE						HInstance;			// main instance
			
			wchar_t *								Caption;
			HWND									HWnd;
			HWND									hMainWnd;
			static bool								Show;
			
		  //// Common UI elements 
			htmlayout::dom::element  Body;					// Body of HTMLayout document
			htmlayout::dom::element  WindowCaption;			// Document caption
			htmlayout::dom::element  CloseButton;			// Close window button
		  //\\ Common UI elements 

			htmlayout::dom::element  OKButton;
			htmlayout::dom::element  PasswordEdit;
			htmlayout::dom::element  LoginEdit;
		};
	}
}