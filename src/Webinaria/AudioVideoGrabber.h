#pragma once
#include "AudioGrabber.h"
#include "WDMGrabber.h"
#include "ScreenGrabber.h"
#include "DeviceEnumerator.h"
#include "shellapi.h"
#include "Recorder.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		// Class for simultaneously record audio and video
		class CAudioVideoGrabber : virtual public CAudioGrabber, virtual public CWDMGrabber, virtual public CScreenGrabber
		{
		// Public methods
		public:
			// Default constructor
			CAudioVideoGrabber(HWND hWnd, CDeviceEnumerator * ExistEnum, const bool & FreeVersion);

			// Virtual destructor
			virtual ~CAudioVideoGrabber(void);

			// Tear down everything downstream of the capture filters, so we can build
			// a different capture graph.  Notice that we never destroy the capture filters
			// and WDM filters upstream of them, because then all the capture settings
			// we've set would be lost.
			virtual void TearDownGraph();

			// Ñreate the capture filters of the graph.  We need to keep them loaded from
			// the beginning, so we can set parameters on them and have them remembered
			bool InitCaptureFilters();

			// All done with the capture filters and the graph builder
			void FreeCaptureFilters();

			// Build the capture graph
			bool BuildCaptureGraph();

			// Function for start grabbing
			virtual bool Start();

			// Fuction for stop grabbing
			virtual bool Stop();

			// Fuction for pause grabbing
			virtual bool Pause();

			// Fuction for resume grabbing
			virtual bool Resume();

			// Set capture possibility of audio narration
			void SetAudioCapture(const bool & Capture);

			// Set capture possibility of webcam video
			void SetWebcamCapture(const bool & Capture);

			void SetFrameRate(double value){FrameRate = value;}
			double GteFraneRate(void){return FrameRate;}

			// Set on/off background removing
			//void SetRemoveBackground(const bool & Back);

		// Protected methods
		protected:
			// Make a graph object we can use for capture graph building
			bool MakeGraph();

			// Set all options
			void MakeOptionsAndDialogs();

		// Protected fields
		protected:
			ICaptureGraphBuilder2				* pBuilder;		// Builder for capturing graph
			IGraphBuilder						* pFg;			// For build filter graph
			IAMCrossbar							* pCrossbar;	// Crossbar for video filters
			//IBaseFilter							* pRemoveBackFilter;// Filter for remove background from 
			IBaseFilter							* pFirstCompressor;	// Filter for compress first videostream
			IBaseFilter							* pSecondCompressor;	// Filter for compress second videostream			
			//IBaseFilter							* pAudioCompressor;	// Filter for compress audiostream			
			IBaseFilter							* pLogoFilter;	// Filter for add logo to video
			bool								CapWebcam;		// If webcamera video need to capture - true,
																// false - otherwise
			bool								CapAudio;		// If audio need to capture - true,
																// false - otherwise
			bool								CapAudioIsRelevant;// If audio relevant - true
			unsigned short						MasterStreamNum;// Number of master stream - use for synhronouzly
			bool								NeedPreview;	// If need preview - set true
			bool								UseFrameRate;	// If true - allow to use custom frame rate
			bool								CaptureGraphBuilt;// True if graph has been built
			bool								PreviewFaked;	// If doesn't construct preview
			bool								CCAvail;		// Assume no closed captioning support
			//bool								TBack;			// If need transparent background
			bool								Logo;			// If free version of Webinaria
			CDeviceEnumerator					* DevEnum;		// All devices enumerator
//			long								NumberOfVideoInputs;// Count of possible video inputs
			long								DroppedBase;
			long								NotBase;
			unsigned long long					CapStartTime;	// The beginning time of capture
			unsigned long long					CapStopTime;	// The ending time of capture
			double								FrameRate;
		};
	}
}
