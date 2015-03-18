#pragma once

#include "Error.h"
#include "Parameters.h"
#include "..\RmBkFilter\iRGBFilters.h"

namespace EditorApplication
{
	namespace EditorLogical
	{
		// Structure for human-readable format - not using now
		struct SVideoTime
		{
			int		ms;
			short	s;
			short	m;
			short	h;
			SVideoTime():ms(0), s(0), m(0), h(0)
			{
			}

			SVideoTime(	const short & hours,
						const short & minutes,
						const short & seconds,
						const int & milliseconds):	ms(milliseconds),
													s(seconds),
													m(minutes),
													h(hours)
			{
			}

			SVideoTime(long long Time)
			{
				unsigned long nMilliseconds = (unsigned long) Time / 10000; // 100ns -> ms
				int nSeconds = nMilliseconds / 1000;
				int nMinutes = nSeconds / 60;
				h = nMinutes / 60;
				m = nMinutes % 60;
				s = nSeconds % 60;
				ms = nMilliseconds % 1000;
			}
		};

		// Class for provide viewing of input avi-clip
		class CViewer
		{
		// Public methods
		public:
			CViewer(HWND hWnd, HWND hClip);
			virtual ~CViewer(void);

			// Open Webinaria Recorder non compression avi
			bool Open(const wchar_t * NewFileToOpen = NULL);

			HRESULT Update(void);

			// Start playback
			HRESULT Play();

			// Pause playback
			HRESULT Pause();
			HRESULT PauseToggle();

			// Stop playback
			HRESULT Stop();

			// Set playback to start frame
			long long GoToStart();

			// Set playback to specified in frame position
			long long SetPFrame(const long long & TrackPosition);

			// Get frames count
			long long GetFramesCount() const;

			// Check audio on/off
			void SetAudio(const bool & flag);

			// Check web video on/off
			void SetWebcamVideo(const bool & flag);

			// Move render window
			void MoveVideoWindow(bool NonConstRatio = false);

			// Move web stream
			void MoveWebStream(const short & hPerShift, const short & vPerShift);

			// Repaint render window
			HRESULT RepaintVideo(HDC hdc);

			// Get media event handler
			//IMediaEventEx * GetME();

			wchar_t * GetFileName() const;

			// All done with the filters and the graph builder
			void FreeFilters();

			bool Loaded() const;
			bool WebcamPresent(void) const;
			bool AudioPresent(void) const;
            long GetDuration(void) const;

			void UpdateNotes(const STextInterval * Text, const SArrowInterval * Arrow);

			unsigned long GetMainStreamLeftShift();

			unsigned long GetMainStreamTopShift();

			/*unsigned long GetMainStreamWidth();

			unsigned long GetMainStreamHeight();*/

			float GetReduction();

			void SetWindows(const HWND & hMain, const HWND & hClip);

			void SetPreviewMode(const bool & Mode);
            void OverlayClear(void);
            void OverlayCommit(void);

		// Protected methods
		protected:
            HRESULT OverlayCreate(void);
            void OverlayDestroy(void);
			// VMR9 verifier
			bool VerifyVMR9(void);

			// Initialize windowless video media renderer
			HRESULT InitializeWindowlessVMR(IBaseFilter **ppVmr9);

			// Initialize graph and display video streams
			HRESULT InitializeVideoAudio(void);


			// Configure all video streams from source file
			HRESULT ConfigureMultiStreamVMR9();
			bool	IsRunning(void);				// Clip is running

		// Protected fields
		protected:
			HWND					hMainWnd;			// Handle to current main window
			HWND					hClipWnd;			// Handle to current clipping window
			HWND					hEditMainWnd;		// Handle to Webinaria Editor main window
			HWND					hEditClipWnd;		// Handle to Webinaria Editor clipping window
			wchar_t					FileName[MAX_PATH];	// Current Webinaria avi file
			IGraphBuilder			*pGB;				// Builder of graph
			IMediaSeeking			*pMS;				// Seeking video frame interface
			IMediaControl			*pMC;				// Play handling interface
			//IMediaEventEx			*pME;				// Media event interface
			IVMRWindowlessControl9	*pWC;				// Control for windowless
			IVMRMixerControl9		*pMix;				// Main mixer control
            IVMRMixerBitmap9*       m_pVmrMixerBitmap9; // Mixer overlay bitmap control
			INotesAdder				*pNotes;			// Filter interface for set text and arrows in stream
			bool					IsWebStream;		// True if web stream exists
			bool					IsAudioStream;		// True if audio stream exists
			LONG					MainStreamWidth;
			LONG					MainStreamHeight;
			LONG					WebStreamWidth;
			LONG					WebStreamHeight;
			LONG					MainStreamVisibleHeight;
			LONG					MainStreamVisibleWidth;
			float					MainStreamReduction;
			float					HorCoeff;			// Coefficient for reduction of shift of web-stream
			float					VerCoeff;
			float					m_CameraTopDelta, m_CameraLeftDelta;
			DWORD					g_dwGraphRegister;	// Registered graph ID			
			bool					IsLoaded;			// Graph is loaded
			long long				StartFrame,			// Start position of playback
									EndFrame;			// End position of playback
//			GUID					guidOriginalFormat;	// Time format
			bool					PreviewMode;
            long                    m_lDuration;        // Clip duration in seconds

            HDC                     m_hdcOveraly;
            HBITMAP                 m_hbmOveraly;
            COLORREF                m_rgbTranspColor;
		public:
			inline LONG GetMainWidth(void) {return MainStreamWidth;}
			inline LONG GetMainHeight(void) {return MainStreamHeight;}
			inline LONG GetWebWidth(void) {return WebStreamWidth;}
			inline LONG GetWebHeight(void) {return WebStreamHeight;}
            inline HDC  GetOverlayHdc(void) {return m_hdcOveraly;}
            inline COLORREF GetOverlayTranspColor(void) {m_rgbTranspColor;}
            inline void SetOverlayTranspColor(COLORREF color) {m_rgbTranspColor = color;}
		};

	}
}