#include <windows.h>
#include <atlbase.h>
#include <strmif.h>
#include <tchar.h>
#include <mtype.h>
#include <uuids.h>
#include <amvideo.h>
#include <vfwmsgs.h>
#include <strsafe.h>
#include <assert.h>
#include <olectl.h>
#include "utils.h"

size_t TruncatePath(TCHAR* tcPath, size_t Len)
{
	while(Len)
	{
		if('\\'==tcPath[Len])
		{
			tcPath[Len+1] = 0;
			break;
		}
		Len--;
	}
	return Len;
}
size_t GetAppFolder(TCHAR* tcPath, size_t Len)
{
	if(Len)
	{
		Len = GetModuleFileName(NULL, tcPath, Len);
		Len = TruncatePath(tcPath, Len );
	}
	return Len;
}
bool __fastcall FileExists(const TCHAR* tcName)
{
	assert(tcName);
	if(!tcName)
		return false;
	WIN32_FIND_DATAW fd;
	HANDLE fh = FindFirstFile(tcName, &fd);
	FindClose(fh);
	return INVALID_HANDLE_VALUE==fh ? false : true;
}

HRESULT __fastcall GetUnconnectedPin(	IBaseFilter *pFilter,   // Pointer to the filter.
										PIN_DIRECTION PinDir,   // Direction of the pin to find.
										IPin **ppPin)           // Receives a pointer to the pin.
{
IEnumPins *pEnum = NULL;
IPin *pPin = NULL;

	if (!ppPin)
		return E_POINTER;
	*ppPin = NULL;

	// Get a pin enumerator
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
		return hr;

	// Look for the first unconnected pin
	PIN_DIRECTION ThisPinDir;
	while (pEnum->Next(1, &pPin, NULL) == S_OK)
	{
		pPin->QueryDirection(&ThisPinDir);
		if (ThisPinDir == PinDir)
		{
			IPin *pTmp = NULL;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))  // Already connected, not the pin we want.
			{
				pTmp->Release();
			}
			else  // Unconnected, this is the pin we want.
			{
				pEnum->Release();
				*ppPin = pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}
	// Release the enumerator
	pEnum->Release();

	// Did not find a matching pin
	return E_FAIL;
}

HRESULT __fastcall JoinFilterToChain(IGraphBuilder *pFilterGraph, IBaseFilter* Filter, IPin **LastPin)
{
HRESULT hr = S_OK;
	if(!pFilterGraph || !Filter || !LastPin)
		return E_POINTER;
	IPin* pFilterPinOut=NULL;
	IPin* pFilterPinIn=NULL;
	hr = GetUnconnectedPin(Filter, PINDIR_INPUT, &pFilterPinIn);
	if(SUCCEEDED(hr))
	{
		hr=pFilterGraph->Connect(*LastPin, pFilterPinIn);
		pFilterPinIn->Release();
		(*LastPin)->Release();
		if(SUCCEEDED(hr))
		{
			HRESULT hr2 = GetUnconnectedPin(Filter, PINDIR_OUTPUT, &pFilterPinOut);
			if(SUCCEEDED(hr2))
			{
				(*LastPin)=pFilterPinOut;
			}
			else
			{
				(*LastPin)=NULL;
			}
		}
		else
		{
			pFilterGraph->RemoveFilter(Filter);
		}
	}
	
	return hr;
}
HRESULT __fastcall JoinFilterToChain(IGraphBuilder *pFilterGraph, IBaseFilter* Filter, LPCWSTR FilterName, IPin **LastPin)
{
HRESULT hr = S_OK;
	if(!pFilterGraph || !Filter || !LastPin)
		return E_POINTER;
	hr = pFilterGraph->AddFilter(Filter, FilterName);
	if(SUCCEEDED(hr))
	{
		hr = JoinFilterToChain(pFilterGraph, Filter, LastPin);
	}
	return hr;
}

// Tear down everything downstream of a given filter
void __fastcall NukeDownstream(IBaseFilter * pf, IGraphBuilder * pGB)
{
			IPin *pP = 0;
			IPin *pTo = 0;
			ULONG u;
			IEnumPins *pins = NULL;
			PIN_INFO pininfo;

			if (!pf)
				return;

			// Enumerate all filter pins
			HRESULT hr = pf->EnumPins(&pins);
			// Go to beginning of enumeration
			pins->Reset();

			while(hr == NOERROR)
			{
				hr = pins->Next(1, &pP, &u);
				if(hr == S_OK && pP)
				{
					pP->ConnectedTo(&pTo);
					if(pTo)
					{
						hr = pTo->QueryPinInfo(&pininfo);
						if(hr == NOERROR)
						{
							if(pininfo.dir == PINDIR_INPUT)
							{
								NukeDownstream(pininfo.pFilter, pGB);
								pGB->Disconnect(pTo);
								pGB->Disconnect(pP);
								pGB->RemoveFilter(pininfo.pFilter);
							}
							pininfo.pFilter->Release();
							pininfo.pFilter = NULL;
						}
						pTo->Release();
						pTo = NULL;
					}
					pP->Release();
					pP = NULL;
				}
			}
			if(pins)
			{
				pins->Release();
				pins = NULL;
			}
}


PinType __fastcall GetPinType(IPin* pPin)
{
PinType ptResult = ptUnknown;
CComPtr<IEnumMediaTypes> cpEnum;
AM_MEDIA_TYPE *pmt    = NULL;
ULONG ulFetched = 0;

	if(!pPin)
	{
		return ptUnknown;
	}
	HRESULT hr = pPin->EnumMediaTypes(&cpEnum);
	if(FAILED(hr))
	{
		return ptUnknown;
	}
	cpEnum->Reset();
	hr = cpEnum->Next(1, &pmt, &ulFetched);
	if(S_OK!=hr)
	{
		return ptUnknown;
	}
	if(MEDIATYPE_Video==pmt->majortype)
	{
		ptResult = ptVideo;
	}
	else if(MEDIATYPE_Audio==pmt->majortype)
	{
		ptResult = ptAudio;
	}
	else
	{
		ptResult = ptUnknown;
	}
	DeleteMediaType(pmt);

	return ptResult;
}
HRESULT __fastcall RemoveAndDeleteFilter(IGraphBuilder *pFilterGraph, IBaseFilter **ppFilter)
{
HRESULT hr=S_OK;

    if(*ppFilter)
    {
        if(pFilterGraph)
        {
            hr = pFilterGraph->RemoveFilter(*ppFilter);
        }
		(*ppFilter)->Release();
		*ppFilter = NULL;
    }
    return hr;
}
HRESULT __fastcall DemolishGraphFilters(IGraphBuilder *pFilterGraph)
{
HRESULT hr = S_OK;
IEnumFilters* pEnum = NULL;

    if(!pFilterGraph)
        return E_POINTER;

    hr = pFilterGraph->EnumFilters(&pEnum);
    if(SUCCEEDED(hr))
    {
		IBaseFilter *pFilter=NULL;
        while(S_OK == pEnum->Next(1, &pFilter, NULL))
        {
            RemoveAndDeleteFilter(pFilterGraph, &pFilter);
            pEnum->Reset(); //it is need because enumerator go confused 
                            //after any "RemoveAndDeleteFilter" call
        }
        pEnum->Release();
    }
    return hr;
}
HRESULT __fastcall CreateAndInsertFilter(	IGraphBuilder *pFilterGraph,
											REFGUID clsidFilter, 
                                            IBaseFilter** ppBaseFilter,
                                            LPCWSTR lpcwstrName)
{
HRESULT hr=S_OK;

    if(!pFilterGraph || !ppBaseFilter )
    {
        return E_POINTER;
    }

    *ppBaseFilter = NULL;

	hr = CreateFilter(clsidFilter, ppBaseFilter);

    if(SUCCEEDED(hr))
    {
        hr = pFilterGraph->AddFilter(*ppBaseFilter, lpcwstrName);
        if(FAILED(hr))
        {   // all or nothing:
			(*ppBaseFilter)->Release();
			*ppBaseFilter = NULL;
        }
    }
    return hr;
}

HRESULT __fastcall GetFrameSize(IPin* pPin, LONG* plWidth, LONG* plHeight)
{
HRESULT hr;
	if(!pPin || !plWidth || !plHeight)
		return E_POINTER;
	*plWidth = *plHeight = 0;
	AM_MEDIA_TYPE mt;
	hr = pPin->ConnectionMediaType(&mt);
	if(SUCCEEDED(hr))
	{
		if(FORMAT_VideoInfo==mt.formattype)
        {
			VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
			if(pVih)
			{
				*plWidth  = pVih->bmiHeader.biWidth;
				*plHeight = pVih->bmiHeader.biHeight;
			}
		}
		else
		{
			hr = VFW_E_UNSUPPORTED_STREAM;
		}
		FreeMediaType(mt);
	}

	return hr;
}

#ifdef DEBUG
void __fastcall AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
IMoniker * pMoniker;
IRunningObjectTable *pROT = NULL;
WCHAR wsz[128];
HRESULT hr;

	if (!pUnkGraph || !pdwRegister)
		return;

	if(FAILED(GetRunningObjectTable(0, &pROT)))
		return;
	hr = StringCbPrintf(wsz, sizeof(wsz), L"FilterGraph %08x pid %08x\0", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
	if(FAILED(hr))
		return;

	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if(SUCCEEDED(hr))
	{
		// Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
		// to the object.  Using this flag will cause the object to remain
		// registered until it is explicitly revoked with the Revoke() method.
		//
		// Not using this flag means that if GraphEdit remotely connects
		// to this graph and then GraphEdit exits, this object registration 
		// will be deleted, causing future attempts by GraphEdit to fail until
		// this application is restarted or until the graph is registered again.
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, pMoniker, pdwRegister);
		pMoniker->Release();
	}
	pROT->Release();
	return;
}

// Removes a filter graph from the Running Object Table
void __fastcall RemoveGraphFromRot(DWORD dwRegister)
{
	IRunningObjectTable *pROT;

	if(SUCCEEDED(GetRunningObjectTable(0, &pROT)))
	{
		pROT->Revoke(dwRegister);
		pROT->Release();
	}
}
#endif

void GetCursorPosition(HWND hwnd, POINT *pt)
{
	GetCursorPos(pt);
	ScreenToClient(hwnd, pt);
}

RECT NewRect(int x, int y, int width, int height)
{
	RECT rc;
	rc.left = x;
	rc.top = y;
	rc.right = rc.left + width;
	rc.bottom = rc.top + height;

	return rc;
}

void DrawRect(HDC dc, const RECT *rc)
{
	Rectangle(dc, rc->left, rc->top, rc->right, rc->bottom);
}

int RectWidth(const RECT &rect)
{
	return rect.right - rect.left;
}

int RectHeight(const RECT &rect)
{
	return rect.bottom - rect.top;
}

void CenterWindow(HWND wnd)
{
	RECT parent, my;
	GetWindowRect(GetParent(wnd), &parent);
	GetWindowRect(wnd, &my);
	MoveWindow(wnd, parent.left + (RectWidth(parent) - RectWidth(my)) / 2, parent.top + (RectHeight(parent) - RectHeight(my)) / 2, 
		RectWidth(my), RectHeight(my), TRUE);
}

TCHAR* _tcstrim(TCHAR *p)
{
	TCHAR *x = p, *y = 0;

	while (*x)
	{
		if (_istspace(*x))
		{
			if (!y)
				y = x;
		}
		else
			y = 0;

		x++;
	}
	if (y)
		*y = _T('\0');

	return p;
}
