#pragma once

#include "htmlayout.h"
#include "mmsystem.h"
#include "commctrl.h"
#include "Error.h"

namespace WebinariaApplication
{
	namespace WebinariaUserInterface
	{
		// Class for get, vizualization and set default microphone parameters
		class CMicrophone:	public htmlayout::event_handler,
							public htmlayout::notification_handler<CMicrophone>
		{
		// Public methods
		public:
			// Constructor
			CMicrophone(HWND hWnd, HINSTANCE hInstance);
		
			// Default destructor
			virtual ~CMicrophone(void);

			bool ShowPopupDialog();

			// Return associated data
			static CMicrophone* self(HWND hWnd) 
			{ 
				return (CMicrophone*)::GetWindowLongPtr(hWnd,GWLP_USERDATA); 
			}

			// Register microphone window class
			ATOM RegisterWindowClass(HINSTANCE hInstance);

		// Protected methods
		protected:
			// Get microphone meter
			static bool GetMeter(HWND hWnd);

			// Get microphone volume
			bool GetVolume();

			// Set microphone volume
			bool SetVolume();

			// Set microphone volume mute
			bool GetMute();

			// Set microphone volume mute
			bool SetMute();

			// Set window caption
			void SetWindowCaption();

			// Test HTMLayount controls under mouse pouinter
			int HitTest( const int & x, const int & y ) const;

			// Return associated data
			static void self(HWND hWnd, CMicrophone* Inst) 
			{ 
				::SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(Inst)); 
			}

			// Get root DOM element of the HTMLayout document
			HELEMENT Root() const;

			// Handle HTMLayout messages
			BOOL on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason);

			// Handle dialog box messages
			static LRESULT CALLBACK SoundDlgWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
		// Protected fields
		protected:
			static HMODULE							hUser32;					// user32.dll module
			// For transparent coners
			static lpfnSetLayeredWindowAttributes	SetLayeredWindowAttributes;
			static HINSTANCE						HInstance;			// main instance
			int										Volume;
			static int								MinVolume, MaxVolume;
			bool									Mute;
			static int								MeterVolume;
			wchar_t *								Caption;
			HWND									HWnd;
			HWND									hMainWnd;
			static HWND								HProgress;
			static bool								Show;
			static int								i;
			static HMIXER							hMixer;          // Mixer handle used in mixer API calls.

			static HWAVEIN							waveInHandle;
			static WAVEHDR							waveHeader[2];    
			static WAVEFORMATEX						waveFormat;

		  //// Common UI elements 
			htmlayout::dom::element  Body;					// Body of HTMLayout document
			htmlayout::dom::element  WindowCaption;			// Document caption
			htmlayout::dom::element  CloseButton;			// Close window button
		  //\\ Common UI elements 

			htmlayout::dom::element  OKButton;
			htmlayout::dom::element  VolumeSlider;
			htmlayout::dom::element  MuteCheckBox;
		};
	}
}