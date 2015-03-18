#pragma once
#include "WDMGrabber.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		// Class provide interface for capturing and then saving video from user's monitor screen
		class CScreenGrabber: virtual public CWDMGrabber
		{
		// Public methods:
		public:
			// Default constructor
			CScreenGrabber(HWND hWnd);

			// Virtual destructor
			virtual ~CScreenGrabber(void);

			// Set capture area of screen
			bool SetScreenArea(	const int & Left,
								const int & Top,
								const int & Right,
								const int & Bottom);

			// Set frame rate of screen capture
			bool SetScreenFrameRate(const int & Rate);
		// Protected metods
		protected:
			IBaseFilter			*pSCap;			// Video capture filter
			IAMStreamConfig		*pSSC;			// For screen filter capabilities

		};
	}
}