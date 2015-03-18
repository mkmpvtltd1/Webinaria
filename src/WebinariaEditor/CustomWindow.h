#pragma once

#include <string>

#include "Viewer.h"
#include "Previewer.h"
#include "WebcamControl.h"
#include "Timeline.h"
#include "TimelineControl.h"
#include "OverlayControl.h"
#include "OverlayElementEditorManager.h"

namespace EditorApplication
{
	namespace EditorUserInterface
	{
		typedef std::basic_string<TCHAR> string_t;

		// Class CCustomWindow
		[event_receiver(native)]
		class CCustomWindow:	public htmlayout::event_handler,
								public htmlayout::notification_handler<CCustomWindow>
		{
		// Public methods
		public:
			// Construct window in specified point with specified dimensions and caption
			CCustomWindow(	const int & X, 
							const int & Y, 
							const int & Width, 
							const int & Height,
							const wchar_t* CaptionText = 0); 

			virtual ~CCustomWindow(void);

			// Set viewer handler
			void SetViewer(EditorLogical::CViewer * NewViewer);

			// Set media event handler
			static void SetME(IMediaEventEx * NewME);

			// Register customer window class
			static ATOM RegisterWindowClass(HINSTANCE hInstance);

			// Set window caption
			void SetWindowCaption();

			// Return pointer to data associated with window
			static CCustomWindow* self(HWND hWnd) 
			{ 
				return (CCustomWindow*)::GetWindowLongPtr(hWnd,GWLP_USERDATA); 
			}

			// Return window handle
			HWND GetHWND(const ApplicationWindow & AW) const;

			// Reset screen capture parameters
			void ResetScreenParameters();

			void OpenFile(TCHAR *file);

		// Protected methods
		protected:
			// Default constructor
			CCustomWindow(void): event_handler(HANDLE_BEHAVIOR_EVENT) 
			{
			}
			
			// Hit mouse pointer
			int HitTest(const int & x, const int & y ) const;

			// Get root DOM element of the HTMLayout document
			HELEMENT Root() const;

			// Get true if window is minimized
			// otherwise - false
			bool IsMin() const;

			virtual BOOL on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason );
			virtual LRESULT on_create_control(LPNMHL_CREATE_CONTROL pnmcc);
			
			static void self(HWND hWnd, CCustomWindow* Inst) 
			{ 
				::SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(Inst)); 
			}

			// Handle window's events
			static LRESULT CALLBACK CustomWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
			// Handle Preview holder window's events
			static LRESULT CALLBACK PreviewWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


		// Protected fields
		protected:
			static HWND HWnd;								// Window handle
			HWND HPictureWnd;								// Picture box handle
			HWND HMainSliderWnd;							// Main slider box handle
			static wchar_t * Caption;						// Caption of main window

		  //// Common UI elements 
			htmlayout::dom::element  Body;					// Body of HTMLayout document
			htmlayout::dom::element  WindowCaption;			// Document caption
			htmlayout::dom::element  MinimizeButton;		// Minimize window button
			htmlayout::dom::element  MaximizeButton;		// Minimize window button
			htmlayout::dom::element  CloseButton;			// Close window button
			htmlayout::dom::element  IconButton;			// System menu button
			//htmlayout::dom::element  Corner;				// Window size change corner
		  //\\ Common UI elements

		 //// Common elements
			htmlayout::dom::element  OpenButton;			// Open file
			htmlayout::dom::element  SaveButton;			// Save file
			htmlayout::dom::element  PreviewButton;			// Webinaria previewer
			htmlayout::dom::element  MakeFlashVideoButton;	// Convertation to FLV
			htmlayout::dom::element  SoundControlButton;	// Flip audio on/off
			htmlayout::dom::element  WebcamControlButton;	// Flip video on/off
			htmlayout::dom::element  VideoPreview;			// Video preview control
			htmlayout::dom::element  AddTextButton;			// Add new text element
		 //\\ Common elements
			
		 //// Stream control elements
			htmlayout::dom::element  AddIntervalButton;		// Mark selection start
			htmlayout::dom::element  DeleteOverlayButton;		// Mark selection end
			htmlayout::dom::element  DeleteContentButton;			// Delete selection
			htmlayout::dom::element  NextFrameButton;		// Go to next frame
			htmlayout::dom::element  PrevFrameButton;		// Go to previous frame
		 //\\ Stream control elements

		 //// Additional elements
			htmlayout::dom::element  PublishButton;			// Publish on-line your flv
			htmlayout::dom::element  StatusReporter;
			htmlayout::dom::element  ConversionMessage;		// Text line with run-time information
		 //\\ Additional elements
			
			WebcamControl *m_WebcamControl;

			int m_SelectionStart, m_SelectionEnd;

			bool convertingFile;

			void ChangeConversionProcessState(bool started);
			void UpdateAllControls(bool enable, bool includingOpenButton);
			void UpdateSelectionControls();

			static HINSTANCE HInstance;						// Window instance handle
			static HMODULE hUser32;							// user32.dll module

			static IMediaEventEx				*pME;		// Use for handling media events
			static EditorLogical::CViewer		*Viewer;	// Main external viewer
			static unsigned short				iter;		// Repaint iterator
			static WNDPROC						OldProc;	// The elder trackbar window procedure 
			static WNDPROC						OldPreviewWndProc;// The elder preview window procedure 

			static EditorLogical::CParameters	params;		// Frames changing parameters
			static bool VideoDown;
			static int DownX;
			static int DownY;
			static int Left;
			static int Top;
			static bool NewArrow;
			static bool NewArrowFirstPoint;

			CPreviewerWindow * Previewer;

		private:
			bool hasVideo, hasSound;

		private:
			Timeline timeline;
			TimelineControl timelineControl;
			static OverlayControl overlayControl;
			OverlayElementEditorManager overlayElementEditorManager;
			static bool freezePreview;

			string_t baseFileName;

			void ParseFileName(const TCHAR *fileName);

			void Save(bool saveAs);

			void TimelineChanged();
			void TimelineCurrentFrameChanged();
			void TimelineActiveOverlayChanged();
			void OverlayRightMouseButtonClicked(OverlayElement *element, const POINT *pt);
			void OverlayActiveElementChanged();
			void OverlayElementChanged();
            static void DrawOverlay();

		};
	}
}
