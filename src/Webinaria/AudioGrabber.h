#pragma once
#include "Grabber.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		// Class provide interface for recording audio from any audio devices (e.g. microphone)
		class CAudioGrabber: virtual public CGrabber
		{
		// Public methods
		public:
			// Default constructor
			CAudioGrabber(HWND hWnd);
		
			// Virtual destructor
			virtual ~CAudioGrabber(void);

		// Protected methods
		protected:
			// Choose an audio capture format using ACM
			void ChooseAudioFormat();
			// Choose an output file name
			//virtual bool OpenFileDialog(HWND hMainWnd, LPTSTR FileName, int cb);

		// Protected fields
		protected:
			IAMStreamConfig	*pASC;	// For audio capabilities
			IBaseFilter		*pACap;	// Audio capture filter
		};
	}
}