#pragma once
#include "Grabber.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		// Class provide interface for capturing and then saving video from any devices (e.g. web-cameras)
		class CWDMGrabber: virtual public CGrabber
		{
		// Public methods
		public:
			// Default constructor
			CWDMGrabber(HWND hWnd);

			// Virtual destructor
			virtual ~CWDMGrabber(void);

		// Protected methods
		protected:
			// Put up the open file dialog
			virtual bool OpenFileDialog();

		// Protected fields:
		protected:
//			double				FrameRate;		// Frame rate of video
			IAMStreamConfig		*pVSC;			// For video capabilities
			IAMVideoCompression	*pVC;			// Use for set compression options of video stream
			IBaseFilter			*pVCap;			// Video capture filter
		};
	}
}