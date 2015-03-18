#include "StdAfx.h"
#include "ScreenGrabber.h"

#include "iscreencapture.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		//////////////////////////////////////////////////////////////////////////
		//								Public methods							//
		//////////////////////////////////////////////////////////////////////////
		// Default constructor
		CScreenGrabber::CScreenGrabber(HWND hWnd):	CGrabber(hWnd),
													CWDMGrabber(hWnd),
													pSCap(NULL),
													pSSC(NULL)
		{

		}	

		// Virtual destructor
		CScreenGrabber::~CScreenGrabber(void)
		{
			SAFE_RELEASE(pSCap);
			SAFE_RELEASE(pSSC);
		}

		// Set capture area of screen
		bool CScreenGrabber::SetScreenArea(const int &Left, const int &Top, const int &Right, const int &Bottom)
		{
			CComQIPtr<IUScreenCaptureSettings> settings(pSCap);

			if (settings.p)
			{
				settings->put_Region(Left, Top, Right, Bottom);
				return true;
			}

			return false;
		}

		// Set frame rate of screen capture
		bool CScreenGrabber::SetScreenFrameRate(const int &Rate)
		{
			CComQIPtr<IUScreenCaptureSettings> settings(pSCap);

			if (settings.p)
			{
				settings->put_FramesPerSec(Rate);
				return true;
			}

			return false;
		}
	}
}