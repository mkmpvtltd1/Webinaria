#include "StdAfx.h"
#include "Progress.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		namespace DXShowFilters
		{
			// Constructor
			CProgress::CProgress(wchar_t *pName, LPUNKNOWN pUnk, HRESULT *phr) :CUnknown(pName, pUnk, phr)
			{

			}

			// Virtual destructor
			CProgress::~CProgress(void)
			{

			}

			// Stub
			STDMETHODIMP_(ULONG) CProgress::AddRef()
			{
				return 1;
			}

			// Stub
			STDMETHODIMP_(ULONG) CProgress::Release()
			{
				return 0;
			}

			// Query for interface
			STDMETHODIMP CProgress::QueryInterface(REFIID iid, void **p)
			{
				CheckPointer(p, E_POINTER);
				if(iid == IID_IAMCopyCaptureFileProgress)
				{
					return GetInterface((IAMCopyCaptureFileProgress *)this, p);
				}
				else
				{
					return E_NOINTERFACE;
				}
			}

			// Set current progress for specified place
			STDMETHODIMP CProgress::Progress(int i/*, CProgressWindow prWnd*/)
			{
				/*Set progress window to current progress position*/
				return S_OK;
			}
		}
	}
}