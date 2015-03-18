#include "StdAfx.h"
#include "AudioGrabber.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		//////////////////////////////////////////////////////////////////////////
		//								Public methods							//
		//////////////////////////////////////////////////////////////////////////
		// Default constructor
		CAudioGrabber::CAudioGrabber(HWND hWnd):CGrabber(hWnd),
												pASC(NULL),
												pACap(NULL)
		{
		}

		// Virtual destructor
		CAudioGrabber::~CAudioGrabber(void)
		{
			SAFE_RELEASE(pASC);
			SAFE_RELEASE(pACap);
		}

		//////////////////////////////////////////////////////////////////////////
		//							Protected methods							//
		//////////////////////////////////////////////////////////////////////////
		// Put up the open file dialog
		/*bool CAudioGrabber::OpenFileDialog(HWND hMainWnd, LPTSTR FileName, int cb)
		{
			OPENFILENAME ofn;
			LPTSTR p;
			TCHAR szFileName[_MAX_PATH];
			TCHAR szBuffer[_MAX_PATH];

			if(FileName == NULL || cb <= 0)
				return FALSE;

			// Start with capture file as current file name
			szFileName[0] = 0;
			wcstombs(szFileName, File, NUMELMS(szFileName));
			// Get just the path info
			// Terminate the full path at the last backslash
			lstrcpyn(szBuffer, szFileName, NUMELMS(szBuffer));
			for(p = szBuffer + lstrlen(szBuffer); p > szBuffer; p--)
			{
				if(*p == '\\')
				{
					*(p+1) = '\0';
					break;
				}
			}
			szBuffer[_MAX_PATH-1] = 0;  // Null-terminate

			ZeroMemory(&ofn, sizeof(OPENFILENAME)) ;
			ofn.lStructSize   = sizeof(OPENFILENAME) ;
			ofn.hwndOwner     = hMainWnd ;
			ofn.lpstrFilter   = TEXT("*.wav\0*.*\0\0");
			ofn.nFilterIndex  = 0 ;
			ofn.lpstrFile     = szFileName;
			ofn.nMaxFile      = sizeof(szFileName) ;
			ofn.lpstrFileTitle = NULL;
			ofn.lpstrTitle    = TEXT("Set Capture File");
			ofn.nMaxFileTitle = 0 ;
			ofn.lpstrInitialDir = szBuffer;
			ofn.Flags = OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST ;

			if(GetOpenFileName(&ofn))
			{
				// We have a capture file name
				lstrcpyn(FileName, szFileName, cb);
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}*/

		// Choose an audio capture format using ACM
		void CAudioGrabber::ChooseAudioFormat()
		{
			DWORD dwSize;
			LPWAVEFORMATEX lpwfx;
			AM_MEDIA_TYPE *pmt;

			// There's no point if we can't set a new format
			if(pASC == NULL)
				return;

			// What's the largest format size around?
			acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &dwSize);
			HRESULT hr = pASC->GetFormat(&pmt);
			if(hr != NOERROR)
				return;

			lpwfx = (LPWAVEFORMATEX)pmt->pbFormat;
			dwSize = (DWORD) (max(dwSize, lpwfx->cbSize + sizeof(WAVEFORMATEX)));

			// !!! This doesn't really map to the supported formats of the filter.
			// We should be using a property page based on IAMStreamConfig

			lpwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, dwSize);
			if(lpwfx)
			{
				CopyMemory(lpwfx, pmt->pbFormat, pmt->cbFormat);

				lpwfx->nChannels = 1;
				lpwfx->nSamplesPerSec = 44100;
				lpwfx->wBitsPerSample = 16;
				lpwfx->nBlockAlign = lpwfx->nChannels * lpwfx->wBitsPerSample / 8;
				lpwfx->nAvgBytesPerSec = lpwfx->nChannels * lpwfx->nSamplesPerSec * lpwfx->wBitsPerSample / 8;
				//lpwfx->wFormatTag = 0x0161;
				

				((CMediaType *)pmt)->SetFormat((LPBYTE)lpwfx, lpwfx->cbSize + sizeof(WAVEFORMATEX));

				pASC->SetFormat(pmt);  // Filter will reconnect

				GlobalFreePtr(lpwfx) ;
			}
			DeleteMediaType(pmt);
		}
	}
}