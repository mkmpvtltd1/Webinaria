#pragma once
#include "combase.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		// Namspace DXShowFilters contains classes for processing capturing data with DirectShow Filters
		namespace DXShowFilters
		{
			// Class that implements IAMCopyCaptureFileProgress
			class CProgress : public CUnknown, public IAMCopyCaptureFileProgress
			{
			// Public methods
			public:
				// Constructor
				CProgress(wchar_t *pName, LPUNKNOWN pUnk, HRESULT *phr);

				// Virtual destructor
				virtual ~CProgress();

				// Stub
				STDMETHODIMP_(ULONG) AddRef();

				// Stub
				STDMETHODIMP_(ULONG) Release();

				// Query for interface
				STDMETHODIMP QueryInterface(REFIID iid, void **p);

				// Set progress for specified place
				STDMETHODIMP Progress(int i/*, CProgressWindow prWnd*/);
		};

		CComModule _Module;
		}
	}
}
