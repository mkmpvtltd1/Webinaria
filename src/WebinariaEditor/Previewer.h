// Include GNU library HTMLayout
#include <shlwapi.h>
#include <commctrl.h>

#include "htmlayout.h"
#include "FLVConverter.h"
#include "Timeline.h"

#pragma once

// Namespace WebinariaUserInterface
namespace EditorApplication
{
	namespace EditorUserInterface
	{
		// Application windows enumeration
		enum ApplicationWindow {AW_MAIN = 0, AW_CLIP = 1};

		// Class CPreviewerWindow отвечает за создание главного окна приложения
		class CPreviewerWindow:	public htmlayout::event_handler,
								public htmlayout::notification_handler<CPreviewerWindow>
		{
		// Public methods
		public:
			// Construct window in specified point with specified dimensions and caption
			CPreviewerWindow(	const int & X, 
								const int & Y, 
								const int & Width, 
								const int & Height,
								const wchar_t* CaptionText = 0); 

			// Virtual destructor
			virtual ~CPreviewerWindow(void);

			// Register customer window class
			static ATOM RegisterWindowClass(HINSTANCE hInstance);

			// Set window caption
			void SetWindowCaption();

			// Set notes/deletes frames/source file name/etc. parameters
			void SetParameters(	const EditorLogical::CParameters & NewParams,
								const wchar_t * Source, Timeline *value);

			void SetTimeline(Timeline *value);

			// Return pointer to data associated with window
			static CPreviewerWindow* self(HWND hWnd) 
			{ 
				return (CPreviewerWindow*)::GetWindowLongPtr(hWnd,GWLP_USERDATA); 
			}

			// Return window handle
			HWND GetHWND(const ApplicationWindow & AW) const;

		// Protected methods
		protected:
			// Default constructor
			CPreviewerWindow(void): event_handler(HANDLE_BEHAVIOR_EVENT) 
			{
			}
			
			// Hit mouse pointer
			int HitTest(const int & x,
						const int & y ) const;

			// Get root DOM element of the HTMLayout document
			HELEMENT Root() const;

			// Get true if window is minimized
			// otherwise - false
			bool IsMin() const;

			// Get true if window is maximized
			// otherwise - false
			bool IsMax() const;

			RECT oldWindowSize;
			bool maximized, paused;

			void UpdatePlaybackControls(bool canPlay, bool canPause, bool canStop);

			// Handle events of HTMLayout document
			virtual BOOL on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason );
			virtual LRESULT on_create_control(LPNMHL_CREATE_CONTROL pnmcc);
			
			// Set data associated with window
			static void self(HWND hWnd, CPreviewerWindow* Inst) 
			{ 
				::SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(Inst)); 
			}

			// Handle window's events
			static LRESULT CALLBACK CustomWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
			static LRESULT CALLBACK PreviewWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
			bool CreateGraph();

			void FreeWholeGraph();

			void MoveVideoWindow();

		// Protected fields
		protected:
			static HWND				HWnd;					// Window handle
			HWND					HPictureWnd;			// Picture box handle
			static WNDPROC			OldPreviewWndProc;// The elder preview window procedure 
			static wchar_t *		Caption;						// Caption of main window

		  //// Common UI elements 
			htmlayout::dom::element  Body;					// Body of HTMLayout document
			htmlayout::dom::element  WindowCaption;			// Document caption
			htmlayout::dom::element  MinimizeButton;		// Minimize window button
			htmlayout::dom::element  MaximizeButton;		// Minimize window button
			htmlayout::dom::element  CloseButton;			// Close window button
			htmlayout::dom::element  IconButton;			// System menu button
			htmlayout::dom::element  Corner;				// Window size change corner
		  //\\ Common UI elements
			
		 //// Common elements
			htmlayout::dom::element  OpenButton;			// Open file
			htmlayout::dom::element  SaveButton;			// Convertation to FLV
			htmlayout::dom::element  PlayButton;			// Play/resume playback
			htmlayout::dom::element  PauseButton;			// Pause playback
			htmlayout::dom::element  StopButton;			// Stop playback
		 //\\ Common elements
			
			static HINSTANCE HInstance;						// Window instance handle
			static HMODULE hUser32;							// user32.dll module

			static EditorLogical::CParameters*	params;		// Frames changing parameters
			static wchar_t * SourceFileName;				// Source file name

//			static IMoniker * pMoniker;
			static IGraphBuilder * pGB;
			static IMediaControl * pMC; // Play handling interface
			static IMediaSeeking * pMS;
			static IMediaEventEx * pME;
			static IPin * pOutputPin;
			static IPin * pInputPin;
			static IBaseFilter * pSource;
			static IBaseFilter * pAVISplitter;
			static IBaseFilter * pAdder;
			static IAdder * pAdderSettings;
			static IBaseFilter * pVR;
			static IVMRWindowlessControl9 * pWC;
//			static IVMRMixerControl9 * pMix;
			static IBaseFilter * pAR;

			LONG VideoStreamWidth;
			LONG VideoStreamHeight;
			LONG ArVideoStreamWidth;
			LONG ArVideoStreamHeight;
			LONGLONG m_llAudioSamplesCount;
			LONGLONG m_llVideoFramesCount;
			static DWORD dwRotReg;

		private:
			Timeline *timeline;

		};
	}
}
