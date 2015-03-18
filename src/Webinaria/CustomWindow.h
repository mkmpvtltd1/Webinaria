#pragma once

// Include GNU library HTMLayout, позвол€ющую создавать удобные 
// графические интерфейсы на основе HTML pages
#include "htmlayout.h"
#include "TrayIcon.h"
#include "Recorder.h"
#include "DeviceEnumerator.h"
#include "Microphone.h"
#include "Authorization.h"
#include "shlwapi.h"
#include <fstream>

namespace WebinariaApplication
{
	namespace WebinariaUserInterface
	{
		class CCustomWindow:	public htmlayout::event_handler,
								public htmlayout::notification_handler<CCustomWindow>
		{
		// Public methods
		public:

			CCustomWindow(	const int & X, 
							const int & Y, 
							const int & Width, 
							const int & Height,
							/*const */CDeviceEnumerator /*const*/ * Enum,
							const wchar_t* CaptionText = 0); 

			virtual ~CCustomWindow(void);

			static ATOM RegisterWindowClass(HINSTANCE hInstance);

			void SetWindowCaption();
			
			// Set media event handler
			static void SetME(IMediaEventEx * NewME);

			static CCustomWindow* self(HWND hWnd) 
			{ 
				return (CCustomWindow*)::GetWindowLongPtr(hWnd,GWLP_USERDATA); 
			}

			// Return window handle
			HWND GetHWND() const
			{
				return HWnd;
			}

			// Reset screen capture parameters
			void ResetScreenParameters();

			// Flash tray
			void FlashTray(const bool & NormalState);

			void Show();
			void Hide();
			bool ShouldHideWindowWhenRecording() const;

		// Protected methods
		protected:
			// Default constructor
			CCustomWindow(void): event_handler(HANDLE_BEHAVIOR_EVENT) 
			{
			}
			
			int HitTest(const int & x,
						const int & y ) const;

			// Get root DOM element of the HTMLayout document
			HELEMENT Root() const;

			// Get true if window is minimized
			// otherwise - false
			bool IsMinimized(HWND hwnd) const;

			virtual BOOL on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason );
			
			static void self(HWND hWnd, CCustomWindow* Inst) 
			{ 
				::SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(Inst)); 
			}
			
			static LRESULT CALLBACK CustomWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

			// For transparent coners
			static lpfnSetLayeredWindowAttributes SetLayeredWindowAttributes;

			// Run default html-browser with Webinaria Help Forum
			void HelpForumExecute(void);

			// Run Webinaria Update Service
			static DWORD WINAPI UpdateExecute(LPVOID lpParam);

			// Load capturing devices to WebComboBox
			void LoadDevicesToList();

			void UpdateControlsDisabledState(wchar_t *state);
			void UpdateRecordControls(bool canRecord, bool canPause, bool canStop);

		// Protected fields
		protected:
			HWND HWnd;										// Window handle
			static wchar_t * Caption;						// Caption of main window
			CTrayIcon * TrayIcon;							// Pointer to tray button

			bool paused;
			RECT captureRegion;
			bool skipWMActivate;

			ATOM keyRecord, keyPause, keyStop;

			void SendRecord();
			void SendPause();
			void SendStop();

			void ResetCaptureRegion();
			bool GetLastActiveWindowHandleAndRect(HWND *pActiveWindow, RECT *pRect);

		  //// Common UI elements 
			htmlayout::dom::element  Body;					// Body of HTMLayout document
			htmlayout::dom::element  WindowCaption;			// Document caption
			htmlayout::dom::element  MinimizeButton;		// Minimize window button
			htmlayout::dom::element  CloseButton;			// Close window button
			htmlayout::dom::element  IconButton;			// System menu button
			//htmlayout::dom::element  Corner;				// Window size change corner
			htmlayout::dom::element  StatusBar;				//	Status bar.
		  //\\ Common UI elements 

		  //// Record tab UI elements 
			htmlayout::dom::element  RecordButton;			// Start capturing with specified parameters
			htmlayout::dom::element  PauseButton;	// Pause/resume current capturing
			htmlayout::dom::element  StopButton;			// Stop capturing
			htmlayout::dom::element  HideWindowWhenRecording;		//	Indicates whether the UI should be hidden when recording is in action.
		  //\\ Record tab UI elements 

		  //// Hardware tab UI elements
			// Web camera group UI elements
			htmlayout::dom::element  WebCameraSelectBox;	// Installed web cameras selector
			htmlayout::dom::element  FormatButton;			// Configure format for selected web camera
			htmlayout::dom::element  SourceButton;			// Configure source for selected web camera

			// Sound group UI elements
			htmlayout::dom::element  SoundDeviceSelectBox;	// Installed sound device selector
			htmlayout::dom::element  SoundTestButton;		// Test selected sound device
		  //\\ Hardware tab UI elements

		  //// Options tab UI elements
			// Record source group UI elements
			htmlayout::dom::element  AudioNarrationCheckBox;// Check audio narration
			htmlayout::dom::element  WebcamVideoCheckBox;	// Webcamera video check
			//htmlayout::dom::element  TransparentBackground;	// Delete webcamera background check

			// Capture group UI elements
			htmlayout::dom::element  ScreenRadioButton;		// Capturing whole screen area
			htmlayout::dom::element  ActiveWindowRadioButton;// Capturing active window content
			htmlayout::dom::element  CustomAreaRadioButton;	// Capturing custom area of screen
			htmlayout::dom::element  CustomAreaInfo;		//	Custom are information label.
			htmlayout::dom::element	 SelectRegionButton;

			// File options
			htmlayout::dom::editbox	FrameRate;		// Set video frame rate

			// Tools group UI elements
			htmlayout::dom::element  TextWritingCheckBox;	// Check adding text to specified frames
			htmlayout::dom::element  ArrowPointerCheckBox;	// Check adding arrow pointers to specified frames
			htmlayout::dom::element  CompressQualitySlider;	// Slider for video codec compress quality
			htmlayout::dom::element  CompressQualityEdit;	// Text field for video codec compress quality

		  //\\ Options tab UI elements

		  //// Help tab UI elements 
			htmlayout::dom::element  HelpForumButton;		// Going to http://www.webinaria.com/forum/ 
			htmlayout::dom::element  UpdateButton;			// Going to http://webinaria.com/Setup.exe
			htmlayout::dom::element  UninstallButton;		// Uninstalling Webinaria recorder
			htmlayout::dom::element  UnlockButton;			// Unlock the software.
		  //\\ Help tab UI elements 

			// Additional runtime elements
			htmlayout::dom::element WebComboBox;
			htmlayout::dom::element * WebElement;
			unsigned short WebElementCount;
			wchar_t * curWeb;

			htmlayout::dom::element AudComboBox;
			htmlayout::dom::element * AudElement;
			unsigned short AudElementCount;
			wchar_t * curAud;

			htmlayout::dom::element HardwareTab;
			
			static HINSTANCE HInstance;			// Window instance handle
			static HMODULE hUser32;				// user32.dll module

			CDeviceEnumerator	* DevEnum;		// Extern device enumerator

			static IMediaEventEx     *pME;		// Use for handling media events
			CMicrophone *		microphone;		// Microphone popup window
			static CAuthorization *	authdialog;	// Authorization popup window
		};
	}

	namespace WUI = WebinariaUserInterface;
}
