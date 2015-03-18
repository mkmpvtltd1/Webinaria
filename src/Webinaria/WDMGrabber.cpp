#include "StdAfx.h"
#include "WDMGrabber.h"

#include "FileSelector.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		//////////////////////////////////////////////////////////////////////////
		//								Public methods							//
		//////////////////////////////////////////////////////////////////////////
		// Default constructor
		CWDMGrabber::CWDMGrabber(HWND hWnd):CGrabber(hWnd),
											pVSC(NULL),
											pVC(NULL),
											pVCap(NULL)
		{
		}

		// Virtual destructor
		CWDMGrabber::~CWDMGrabber(void)
		{
			SAFE_RELEASE(pVSC);
			SAFE_RELEASE(pVC);
			SAFE_RELEASE(pVCap);
		}

		//////////////////////////////////////////////////////////////////////////
		//							Protected methods							//
		//////////////////////////////////////////////////////////////////////////
		// Put up the open file dialog
		bool CWDMGrabber::OpenFileDialog()
		{
            FileSelector selector;

            if (selector.Select(hMainWnd, SelectToSave))
			{
				if (File != NULL)
					delete[] File;

				// We have a capture file name
                File = new TCHAR[_tcslen(selector.FileName()) + sizeof(TCHAR)];
                _tcscpy(File, selector.FileName());

				// Delete file if it is exist
				WIN32_FIND_DATAW fd;
				HANDLE hr = FindFirstFileW(File,&fd); 
				if ( hr != INVALID_HANDLE_VALUE )
					DeleteFileW(File);
				FindClose(hr);

				return true;
			}
			return false;
		}
	}
}