#include <atlbase.h>
#include <streams.h>
#include "..\iRGBFilters.h"
#include "Adder.h"
#include "transfrm.h"
#include <ks.h>
#include <ksmedia.h>
#include <dvdmedia.h>

#include <TCHAR.H>

#include "utils.h"

static const AMOVIESETUP_MEDIATYPE
sudInputPinTypes =   { &MEDIATYPE_Video              // clsMajorType
                     , &MEDIASUBTYPE_NULL }  ;       // clsMinorType

const AMOVIESETUP_PIN psudAdderPins[] =
{ 
  { L"In1"              // strName
  , FALSE               // bRendered
  , FALSE               // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudInputPinTypes                // lpTypes
  }, 

  { L"Out"           // strName
  , FALSE               // bRendered
  , TRUE                // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudInputPinTypes                // lpTypes
  },
  { L"In2"              // strName
  , FALSE               // bRendered
  , FALSE               // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudInputPinTypes                // lpTypes
  } 

};

const AMOVIESETUP_FILTER sudAdder =
{
    &CLSID_CAdder,				// Filter CLSID
    L"Adder",					// String name
    MERIT_DO_NOT_USE,			// Filter merit
    3,							// Number pins
    psudAdderPins				// Pin details
};

LONGLONG GetFramesCount(IPin* pInPin)
{
LONGLONG llResult=0;
	if(pInPin)
	{
		CComPtr<IMediaSeeking> cpMediaSeeking;
		CComPtr<IPin> cpOutPin;

		HRESULT hr = pInPin->ConnectedTo(&cpOutPin);
		if(SUCCEEDED(hr))
		{
			hr = cpOutPin->QueryInterface(IID_IMediaSeeking, (void**) &cpMediaSeeking);
			if(SUCCEEDED(hr))
			{
				GUID guidFormat(GUID_NULL);
				hr = cpMediaSeeking->GetTimeFormat(&guidFormat);
				if(SUCCEEDED(hr))
				{			
					if(guidFormat!=TIME_FORMAT_FRAME)
					{
						hr = cpMediaSeeking->SetTimeFormat(&TIME_FORMAT_FRAME);
					}
					if(SUCCEEDED(hr))
					{			
						hr = cpMediaSeeking->GetDuration(&llResult);
					}
					cpMediaSeeking->SetTimeFormat(&guidFormat);
				}
			}
		}
	}
	return llResult;
}

CUnknown * WINAPI CAdder::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
	ASSERT(phr);
    
    // assuming we don't want to modify the data
	CAdder *pNewObject = new CAdder(TEXT("Adder"), punk, phr);

    if(pNewObject == NULL)
	{
        if (phr)
            *phr = E_OUTOFMEMORY;
    }

    return pNewObject;   
}


CAdder::CAdder(TCHAR *pName, LPUNKNOWN pUnk,  HRESULT* phr)
:CTransInPlaceFilter(pName, pUnk, CLSID_CAdder, phr)
,m_pAInput2(NULL)
,m_hEventArrived(NULL)
,m_hEventArrived2(NULL)
,m_bEOSDelivered(FALSE)
,m_bQualityChanged(FALSE)
,m_bSampleSkipped(FALSE)
,m_nHeight(0)
,m_nWidth(0)
,m_nFrameBuf1Size(0)
,m_nHeight2(0)
,m_nWidth2(0)
,pbBufIn2(NULL)
,SecondExists(false)
,m_rtTimePerFrame(0)
,m_llArrivedSampleNo(0)
,m_llDeliveredSampleNo(0)
,DeletedWeb(false)
,m_pDibBits(NULL)
,m_DibSection(NULL)
,m_DC(NULL)
,m_OldObject(NULL)
{
	ZeroMemory( &AllDeleteInterval, sizeof(AllDeleteInterval));
	ZeroMemory( &WebDeleteInterval, sizeof(WebDeleteInterval));
	ZeroMemory( &WebPosInterval, sizeof(WebPosInterval));
	ZeroMemory( &TextInterval, sizeof(TextInterval));
	ZeroMemory( &ArrowInterval, sizeof(ArrowInterval));

    m_tcThumbnailPath[0] = _T('\0');
}

// destructor

CAdder::~CAdder()
{
    // Delete the pins
	delete m_pAInput2;
	delete[] pbBufIn2;
	ClearGDIObjects();
}
void CAdder::ClearGDIObjects(void)
{
	if( m_OldObject )
    {
        SelectObject( m_DC, m_OldObject );
        m_OldObject = NULL;
    }
    if( m_DC )
    {
        DeleteDC( m_DC );
        m_DC = NULL;
    }
    if( m_DibSection )
    {
        DeleteObject( m_DibSection );
        m_DibSection = NULL;
    }
}

STDMETHODIMP CAdder::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
    CheckPointer(ppv, E_POINTER);

	if (riid == IID_IAdder)
        return GetInterface((IAdder *) this, ppv);
    else if (riid == IID_ITimelineFilter)
        return GetInterface((ITimelineFilter *) this, ppv);

    return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}


STDMETHODIMP CAdder::SetIntervals(	void * MainIntervals,
									void * WebIntervals,
									void * WebPosIntervals,
									void * ArrowIntervals,
									void * TextIntervals)
{
	if (MainIntervals)
		AllDeleteInterval = *((SInterval *)MainIntervals);
	if (WebIntervals)
		WebDeleteInterval = *((SInterval *)WebIntervals);
	if (WebPosIntervals)
		WebPosInterval = *((SPosInterval *)WebPosIntervals);
	if (TextIntervals)
		TextInterval = *((STextInterval *)TextIntervals);
	if (ArrowIntervals)
		ArrowInterval = *((SArrowInterval *)ArrowIntervals);
	return S_OK;
}

STDMETHODIMP CAdder::SetThumbnailPath(BSTR path)
{
    COLE2T sPath(path);

    return _tcscpy_s(m_tcThumbnailPath, _countof(m_tcThumbnailPath), sPath);
}


HRESULT CAdder::ReceiveOptional(IMediaSample *pSample)
{
HRESULT hr = S_OK;
//REFERENCE_TIME rtStream1;
//REFERENCE_TIME rtStream2;
DWORD dwWaitResult;

//	{
//		CAutoLock Lock(&m_csReceive);
//		
///		rtStream1 = m_llArrivedSampleNo*m_rtTimePerFrame;
//		rtStream2 = m_llArrivedSampleNo2*m_rtTimePerFrame2;
//		m_llArrivedSampleNo2++;
//	}

//	if(rtStream2<rtStream1)
//	{
//		return S_OK;
//	}
//	else
	{
		while(true)
		{
			dwWaitResult = WaitForSingleObject( m_hEventArrived, 1000);
			if(WAIT_TIMEOUT!=dwWaitResult)
				break;
		}
	}

	if (AllDeleteInterval.In(m_llArrivedSampleNo))
	{
		return S_OK;
	}

	DeletedWeb = WebDeleteInterval.In(m_llArrivedSampleNo);
	if (DeletedWeb)
	{
		SetEvent(m_hEventArrived2);
		Sleep(0);
		return S_OK;
	}
	char * pBuf = NULL;
	pSample->GetPointer( (LPBYTE*) &pBuf );
	if(pBuf && pbBufIn2)
	{
		long lDatalength = pSample->GetActualDataLength();
		memcpy( pbBufIn2, pBuf, lDatalength );
	}
	SetEvent(m_hEventArrived2);
	Sleep(0);
    return hr;
}


HRESULT CAdder::Transform(IMediaSample * pOut)
{
bool fNeedCopyBuf = false;
	CheckPointer(pOut, E_POINTER);

	{
		CAutoLock Lock(&m_csReceive);
		m_llArrivedSampleNo++;
	
		if(m_hEventArrived)
		{
			SetEvent(m_hEventArrived);
			Sleep(0);
		}
	}

	
#ifndef DEBUG
	if(0==(m_llArrivedSampleNo % 25))
#endif
	{//we notify host application every 25 frames:		
		NotifyEvent(EC_PROGRESS,  (LONG)m_llArrivedSampleNo, 0);
	}


	if (AllDeleteInterval.In(m_llArrivedSampleNo))
	{   //todo: correct timestamps???
		return S_FALSE;
	}

	CAutoLock cObjectLock(m_pLock);

	BYTE* pBufOut = NULL;
	pOut->GetPointer( &pBufOut );
	if(!pBufOut)
		return S_FALSE;

    OverlayDraw(m_llArrivedSampleNo, pOut);

	/* Arrow notes */ 
	SArrowInterval * CurrentArrow = ArrowInterval.Next;

	while (NULL != CurrentArrow)
	{
		if ((CurrentArrow->Start <= m_llArrivedSampleNo) && (m_llArrivedSampleNo <= CurrentArrow->End))
		{
			if(!fNeedCopyBuf)
			{
				memcpy( m_pDibBits, pBufOut, pOut->GetActualDataLength() );
				fNeedCopyBuf = true;
			}
			HPEN hPen = CreatePen(PS_SOLID, CurrentArrow->Width, /*0xFF000000|*/CurrentArrow->Color);
			HPEN hOldPen = (HPEN)SelectObject(m_DC, hPen);

			int OldBkMode = SetBkMode(m_DC, OPAQUE);

			int ArrBX = 4 * CurrentArrow->x2/5 + CurrentArrow->x1/5;
			int ArrBY = 4 * CurrentArrow->y2/5 + CurrentArrow->y1/5;

			MoveToEx(m_DC, CurrentArrow->x1, CurrentArrow->y1, NULL);
			LineTo(m_DC, ArrBX, ArrBY);

			MoveToEx(m_DC, (-CurrentArrow->y2 + CurrentArrow->y1)/15 + ArrBX,
						   (CurrentArrow->x2 - CurrentArrow->x1)/15 + ArrBY, NULL);
			LineTo(m_DC, (CurrentArrow->y2 - CurrentArrow->y1)/15 + ArrBX,
						 (-CurrentArrow->x2 + CurrentArrow->x1)/15 + ArrBY);

			MoveToEx(m_DC, CurrentArrow->x2, CurrentArrow->y2, NULL);
			LineTo(m_DC, (-CurrentArrow->y2 + CurrentArrow->y1)/15 + ArrBX,
						 (CurrentArrow->x2 - CurrentArrow->x1)/15 + ArrBY);
			MoveToEx(m_DC, CurrentArrow->x2, CurrentArrow->y2, NULL);
			LineTo(m_DC, (CurrentArrow->y2 - CurrentArrow->y1)/15 + ArrBX,
						 (-CurrentArrow->x2 + CurrentArrow->x1)/15 + ArrBY);
	
			SetBkMode(m_DC, OldBkMode);

			SelectObject(m_DC, hOldPen);
			DeleteObject(hPen);
		}
		CurrentArrow = CurrentArrow->Next;
	}
	/* Arrow notes end */

	/* Text notes */
	STextInterval * CurrentText = TextInterval.Next;

	while (NULL != CurrentText)
	{
		if ((CurrentText->Start <= m_llArrivedSampleNo) && (m_llArrivedSampleNo <= CurrentText->End))
		{
			if(!fNeedCopyBuf)
			{
				memcpy( m_pDibBits, pBufOut, pOut->GetActualDataLength() );
				fNeedCopyBuf = true;
			}

			HFONT hFont = CreateFontIndirect(&CurrentText->lf);
			HFONT hOldFont = (HFONT)SelectObject(m_DC, hFont);
			COLORREF OldColor = SetTextColor(m_DC, /*0xFF000000|*/CurrentText->Color);

			int OldBkMode = SetBkMode(m_DC, TRANSPARENT);

			// Out the lettering
			TextOutW( m_DC, CurrentText->Left, CurrentText->Top, CurrentText->String, wcslen(CurrentText->String));

			SelectObject(m_DC, hOldFont);
			DeleteObject(hFont);

			SetBkMode(m_DC, OldBkMode);

			SetTextColor(m_DC, OldColor);
		}
		CurrentText = CurrentText->Next;
	}
	/* Text notes end*/

	if(fNeedCopyBuf)
	{
		memcpy( pBufOut, m_pDibBits, m_nFrameBuf1Size );
	}
	if(m_hEventArrived2)
	{
		DWORD dwWaitResult;
//		while(true)
		{
			dwWaitResult = WaitForSingleObject( m_hEventArrived2, 2000);
//			if(WAIT_TIMEOUT!=dwWaitResult)
//				break;
		}
	}
	
	if (SecondExists && !DeletedWeb)
	{	
		int dh = 0;
		int dw = 0;
		SPosInterval * Current = WebPosInterval.IntervalByTime(m_llArrivedSampleNo);
		if (NULL != Current)
		{
			dh = (int)((float)(100 - Current->VertShift) * (float)m_nHeight / 100.0f);
			dw = (int)((float)Current->HorShift * (float)m_nWidth / 100.0f);
		}

		int offset  = 0;
		int offset2 = 0;
		float alpha = 0;
		BYTE* pBufTemp = pBufOut;//(BYTE*)m_pDibBits;
		for (int i = 1; i < m_nHeight2; i++)
		{
			for (int j = 1; j < m_nWidth2 - 1 ; j++)
			{
				offset = (m_nHeight - i - 1 - dh) * (m_nWidth << 2) + ((j + dw) << 2);
				offset2 = (m_nHeight2 - i -1) * (m_nWidth2 << 2)  + (j << 2);
//				alpha = ((float)(BYTE)pBufIn2[offset2 + 3]) / 255.0f;

				if ((j + dw < m_nWidth)&&( m_nHeight > i + dh))
				{
					pBufTemp[offset]     = pbBufIn2[offset2];
					pBufTemp[offset + 1] = pbBufIn2[offset2 + 1];
					pBufTemp[offset + 2] = pbBufIn2[offset2 + 2];

//					pBufTemp[offset + 3] = 0xFF;
//					pBufTemp[offset]     = (BYTE)(((float)pBufIn2[offset2]) * alpha + ((float)pBufTemp[offset]) * (1.0f - alpha));
//					pBufTemp[offset + 1] = (BYTE)(((float)pBufIn2[offset2 + 1]) * alpha + ((float)pBufTemp[offset + 1]) * (1.0f - alpha));
//					pBufTemp[offset + 2] = (BYTE)(((float)pBufIn2[offset2 + 2]) * alpha + ((float)pBufTemp[offset + 2]) * (1.0f - alpha));
				}
			}
		}
	}


	if (10 == m_llDeliveredSampleNo /*|| 150 == m_lDeliveredSampleNo*/)
	{
		VIDEOINFO* pVI = (VIDEOINFO*) m_pInput->CurrentMediaType( ).Format( );
		CheckPointer(pVI, E_UNEXPECTED);
		memcpy( m_pDibBits, pBufOut, pOut->GetActualDataLength() );
		CreateThumbnail(m_DC, m_DibSection, TEXT("thumbnail"), (PBITMAPINFO)&pVI->bmiHeader);
	}
	LONGLONG llStart = m_llDeliveredSampleNo;

	REFERENCE_TIME rtSampleStart = m_llDeliveredSampleNo*m_rtTimePerFrame;
	REFERENCE_TIME rtSampleEnd   = rtSampleStart+m_rtTimePerFrame;


	m_llDeliveredSampleNo++;
	pOut->SetMediaTime(&llStart, &m_llDeliveredSampleNo);
	pOut->SetTime(&rtSampleStart, &rtSampleEnd);
	return NO_ERROR;
}

void CAdder::CreateThumbnail(const HDC hDC, const HBITMAP hBMP, PTCHAR ptcFile, PBITMAPINFO pbi)
{
    if (_T('\0') == m_tcThumbnailPath[0])
        return;

	HANDLE hf;					// file handle 
    BITMAPFILEHEADER hdr;       // bitmap file-header 
    BITMAPINFO bi;				// bitmap info-header 
    LPBYTE lpBits;              // memory pointer 
    DWORD dwTotal;              // total count of bytes 
    DWORD cb;                   // incremental count of bytes 
    BYTE *hp;                   // byte pointer 
    DWORD dwTmp;

	bi = *pbi;
	
	HDC hdcScaled = CreateCompatibleDC(hDC); 
 
    HBITMAP hbmScaled = CreateCompatibleBitmap(hDC, 95, 70); 
 
    if (hbmScaled == 0) 
        return;
 
    // Select the bitmaps into the compatible DC. 
 
    if (!SelectObject(hdcScaled, hbmScaled)) 
        return;

	SetStretchBltMode( hdcScaled, HALFTONE );

	int HeightShift = 0;
	int WidthShift = 0;

	if (bi.bmiHeader.biWidth * 70 > bi.bmiHeader.biHeight * 95)
		HeightShift = (int)(35.0f - 47.5f * (float) bi.bmiHeader.biHeight/(float)bi.bmiHeader.biWidth);
	else
		WidthShift = (int)(47.5f - 35.0f * (float) bi.bmiHeader.biWidth/(float)bi.bmiHeader.biHeight);
	StretchBlt(	hdcScaled, 
				WidthShift, HeightShift, 
				95 - (WidthShift<<1), 70 - (HeightShift<<1), 
				hDC, 
				0, 0, 
				bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, 
				SRCCOPY); 

	bi.bmiHeader.biHeight = 70;
	bi.bmiHeader.biWidth = 95;
	bi.bmiHeader.biSizeImage = 26600;

    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, bi.bmiHeader.biSizeImage);

    if (!lpBits) 
         return; 

    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB. 
    if (!GetDIBits(hdcScaled, hbmScaled, 0, (WORD) bi.bmiHeader.biHeight, lpBits, &bi, DIB_RGB_COLORS)) 
		return;

    // Create .BMP file. 
    hf = CreateFile(m_tcThumbnailPath, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) 
        return;
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    hdr.bfSize = (DWORD) (	sizeof(BITMAPFILEHEADER) + 
							bi.bmiHeader.biSize + 
							bi.bmiHeader.biClrUsed * sizeof(RGBQUAD) + 
							bi.bmiHeader.biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices. 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    bi.bmiHeader.biSize + 
					bi.bmiHeader.biClrUsed * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file. 
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL)) 
		return;

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if (!WriteFile(hf, (LPVOID)&(bi.bmiHeader), sizeof(BITMAPINFOHEADER) + bi.bmiHeader.biClrUsed * sizeof (RGBQUAD), 
                  (LPDWORD)&dwTmp, NULL)) 
        return;

    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = bi.bmiHeader.biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
		return;

    // Close the .BMP file. 
    if (!CloseHandle(hf)) 
        return;

    if( hbmScaled )
    {
        DeleteObject( hbmScaled );
        hbmScaled = NULL;
    }
	if( hdcScaled )
    {
        DeleteDC( hdcScaled );
        hdcScaled = NULL;
    }

    // Free memory. 
    GlobalFree((HGLOBAL)lpBits);

	typedef void (__stdcall * LPFUNC)(const wchar_t * );

	HMODULE hDLL;				// Handle to DLL
	LPFUNC SaveAsJPEG;			// Function pointer

	hDLL = LoadLibrary(TEXT("JPEG.dll"));
	if (hDLL != NULL)
	{
		SaveAsJPEG = (LPFUNC)GetProcAddress(hDLL, "_SaveAsJPEG@4");
		if (!SaveAsJPEG)
		{
			FreeLibrary(hDLL);       
			return;
		}
		else
		{
			SaveAsJPEG(m_tcThumbnailPath);
			FreeLibrary(hDLL);
		}
	}

	DeleteFileW(m_tcThumbnailPath);
}

// check if you can support mtIn
HRESULT CAdder::CheckInputType(const CMediaType* mtIn)
{
	CheckPointer(mtIn,E_POINTER);

    if (*mtIn->FormatType() != FORMAT_VideoInfo)
    {
        return E_INVALIDARG;
    }

    if( *mtIn->Type( ) != MEDIATYPE_Video )
    {
        return E_INVALIDARG;
    }

	if( *mtIn->Subtype( ) != MEDIASUBTYPE_ARGB32)
    {
        return E_INVALIDARG;
    }

    VIDEOINFO *pVI = (VIDEOINFO *) mtIn->Format();
    CheckPointer(pVI,E_UNEXPECTED);

    if( pVI->bmiHeader.biBitCount != 32 )
    {
        return E_INVALIDARG;
    }

    // Reject negative height bitmaps to prevent drawing upside-down text
    if( pVI->bmiHeader.biHeight < 0 )
    {
        return E_INVALIDARG;
    }

    return NOERROR;
}


// return the number of pins we provide
int CAdder::GetPinCount()
{
    return 3;
}


// return a non-addrefed CBasePin * for the user to addref if he holds onto it
// for longer than his pointer to us. We create the pins dynamically when they
// are asked for rather than in the constructor. This is because we want to
// give the derived class an oppportunity to return different pin objects

// We return the objects as and when they are needed. If either of these fails
// then we return NULL, the assumption being that the caller will realise the
// whole deal is off and destroy us - which in turn will delete everything.

CBasePin * CAdder::GetPin(int n)
{
    HRESULT hr = S_OK;

    // Create an input pin if necessary

	if(NULL==m_pAInput2)
	{
		m_pAInput2 = new CAdderInputPin(NAME("Adder input pin"),
                                          this,              // Owner filter
                                          &hr,               // Result code
                                          TEXT("In 2"),			 // Pin name
										  2);				 // Pin number
		if(FAILED(hr))
		{
			delete m_pAInput2;
			m_pAInput2 = NULL;
			return NULL;
		}
	}
	if(0==n || 1==n)
	{
		return CTransInPlaceFilter::GetPin(n);
	}
	else if(2==n)
	{
		return m_pAInput2;
	}
	return NULL;
}


//
// FindPin
//
// If Id is In or Out then return the IPin* for that pin
// creating the pin if need be.  Otherwise return NULL with an error.

STDMETHODIMP CAdder::FindPin(LPCWSTR Id, IPin **ppPin)
{
    CheckPointer(ppPin,E_POINTER);
    ValidateReadWritePtr(ppPin,sizeof(IPin *));

    if (0==lstrcmpW(Id,L"In1"))
	{
        *ppPin = GetPin(0);
    } 
	else if (0==lstrcmpW(Id,L"In2"))
	{
        *ppPin = GetPin(2);
    } 
	else if (0==lstrcmpW(Id,L"Out"))
	{
        *ppPin = GetPin(1);
    } 
	else
	{
        *ppPin = NULL;
        return VFW_E_NOT_FOUND;
    }

    HRESULT hr = NOERROR;
    //  AddRef() returned pointer - but GetPin could fail if memory is low.
    if (*ppPin) 
	{
        (*ppPin)->AddRef();
    }
	else
	{
        hr = VFW_E_NOT_FOUND;  
    }
    return hr;
}



// EndOfStream received. Default behaviour is to deliver straight
// downstream, since we have no queued data. If you overrode Receive
// and have queue data, then you need to handle this and deliver EOS after
// all queued data is sent
/*
HRESULT CAdder::EndOfStream(void)
{
    HRESULT hr = NOERROR;
    if (m_pAOutput != NULL && m_bEOSDelivered == FALSE) 
	{
		if(m_pAInput2->IsConnected())
		{
			if(m_pAInput->m_fEosRecived && m_pAInput2->m_fEosRecived)
			{
				hr = m_pAOutput->DeliverEndOfStream();
				m_bEOSDelivered = TRUE;
			}
		}
		else
		{
			hr = m_pAOutput->DeliverEndOfStream();
			m_bEOSDelivered = TRUE;
		}
    }

    return hr;
}
*/

// override these so that the derived filter can catch them

STDMETHODIMP CAdder::Stop()
{
HRESULT hr;
    if ((m_State != State_Stopped) )
    {
        // decommit the input pin before locking or we can deadlock
        m_pAInput2->Inactive();

	}
	if(pbBufIn2)
	{
		delete[] pbBufIn2;
		pbBufIn2 = NULL;
	}
	ClearGDIObjects();
	if(m_hEventArrived)
	{
		SetEvent(m_hEventArrived);
		CloseHandle(m_hEventArrived);
		m_hEventArrived = NULL;
	}
	if(m_hEventArrived2)
	{
		CloseHandle(m_hEventArrived2);
		m_hEventArrived2 = NULL;
	}
	hr = CTransInPlaceFilter::Stop();
    OverlaysDestroy();

    return hr;
}
STDMETHODIMP CAdder::Run(REFERENCE_TIME tStart)
{
	return CTransInPlaceFilter::Run(tStart);
}

STDMETHODIMP CAdder::Pause()
{
    CAutoLock lck(&m_csFilter);
    HRESULT hr = NOERROR;

	if(m_State == State_Stopped)
	{
		SecondExists = false;
		m_llDeliveredSampleNo = m_llArrivedSampleNo = m_llArrivedSampleNo2 = 0;
		m_rtTimePerFrame = m_rtTimePerFrame2 = 0;

		AM_MEDIA_TYPE mt;
		NotifyEvent(EC_PROGRESS, 0, 0);

		ClearGDIObjects();

		m_nWidth = m_nHeight = 0;

		hr = m_pInput->ConnectionMediaType( &mt);

		if(SUCCEEDED(hr))
		{
			if (mt.formattype == FORMAT_VideoInfo)
		    {
	            VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
				m_rtTimePerFrame  = pVih->AvgTimePerFrame;
				m_nWidth  = pVih->bmiHeader.biWidth;
				m_nHeight = pVih->bmiHeader.biHeight;
			}
		    else if (mt.formattype == FORMAT_VideoInfo2)
	        {
				VIDEOINFOHEADER2 *pVih2 = reinterpret_cast<VIDEOINFOHEADER2*>(mt.pbFormat);
				m_rtTimePerFrame  = pVih2->AvgTimePerFrame;
				m_nWidth  = pVih2->bmiHeader.biWidth;
				m_nHeight = pVih2->bmiHeader.biHeight;
		    }
	    }
		m_nFrameBuf1Size = m_nWidth * m_nHeight * 4;

		SecondExists = m_pAInput2->IsConnected();
		if(SecondExists)
		{
			hr = m_pAInput2->ConnectionMediaType( &mt);
			VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
			m_nWidth2  = pVih->bmiHeader.biWidth;
			m_nHeight2 = pVih->bmiHeader.biHeight;
			m_rtTimePerFrame2  = pVih->AvgTimePerFrame;
			pbBufIn2 = new BYTE[m_nWidth2 * m_nHeight2 * 4];

			m_hEventArrived  = CreateEvent(NULL, FALSE, FALSE, NULL);
			m_hEventArrived2 = CreateEvent(NULL, FALSE, FALSE, NULL);
			if(!m_hEventArrived || !m_hEventArrived2)
			{
				return E_UNEXPECTED;
			}
			m_llFramesCount = GetFramesCount(m_pInput);
			m_llFramesCount2= GetFramesCount(m_pAInput2);


		}

		VIDEOINFO* pVI = (VIDEOINFO*) m_pInput->CurrentMediaType( ).Format( );
		CheckPointer(pVI,E_UNEXPECTED);

		m_DibSection = CreateDIBSection(NULL,
										(BITMAPINFO*) &pVI->bmiHeader,
										DIB_RGB_COLORS,
										&m_pDibBits,
										NULL,
										0);
		if( !m_DibSection )
		{
			return E_FAIL;
		}

		HDC hdc = GetDC( NULL );
		m_DC = CreateCompatibleDC( hdc );
		ReleaseDC( NULL, hdc );
		if( !m_DC )
			return E_FAIL;


		m_OldObject = SelectObject( m_DC, m_DibSection );
		if( !m_OldObject )
			return E_FAIL;

        OverlaysInit(pVI);
	}
    

	hr = CTransInPlaceFilter::Pause();

    return hr;
}



// =================================================================
// Implements the CAdderInputPin class
// =================================================================


// constructor

CAdderInputPin::CAdderInputPin(	TCHAR *pObjectName, CAdder *pAdder, HRESULT * phr, LPCWSTR pName, const int & num)
:CBaseInputPin(	pObjectName, pAdder, &pAdder->m_csFilter, phr, pName), Num(num)
{
    DbgLog((LOG_TRACE,2,TEXT("CAdderInputPin::CAdderInputPin")));
    m_pAdder = pAdder;
}

// provides derived filter a chance to grab extra interfaces

HRESULT CAdderInputPin::CheckConnect(IPin *pPin)
{
HRESULT hr;
    hr = m_pAdder->CheckConnect(PINDIR_INPUT, pPin);
    if (FAILED(hr))
	{
    	return hr;
    }
	hr = CBaseInputPin::CheckConnect(pPin);
    return hr;
}


// provides derived filter a chance to release it's extra interfaces

HRESULT CAdderInputPin::BreakConnect()
{
    //  Can't disconnect unless stopped
    ASSERT(IsStopped());
    m_pAdder->BreakConnect(PINDIR_INPUT);
    return CBaseInputPin::BreakConnect();
}


// Let derived class know when the input pin is connected

HRESULT CAdderInputPin::CompleteConnect(IPin *pReceivePin)
{
    HRESULT hr = m_pAdder->CompleteConnect(PINDIR_INPUT,pReceivePin);
    if (FAILED(hr))
	{
        return hr;
    }
    return CBaseInputPin::CompleteConnect(pReceivePin);
}


// check that we can support a given media type

HRESULT CAdderInputPin::CheckMediaType(const CMediaType* pmt)
{
    // Check the input type

    HRESULT hr = m_pAdder->CheckInputType(pmt);
    if (S_OK != hr)
	{
        return hr;
    }

    // if the output pin is still connected, then we have
    // to check the transform not just the input format

    if ((m_pAdder->m_pOutput != NULL) &&
        (m_pAdder->m_pOutput->IsConnected()))
	{
            return m_pAdder->CheckTransform(pmt, &m_pAdder->m_pOutput->CurrentMediaType());
    }
	else
	{
        return hr;
    }
}



// =================================================================
// Implements IMemInputPin interface
// =================================================================


// provide EndOfStream that passes straight downstream
// (there is no queued data)

STDMETHODIMP CAdderInputPin::EndOfStream(void)
{
	m_fEosRecived = TRUE;
	if(m_pAdder->m_hEventArrived2)
	{
		CloseHandle(m_pAdder->m_hEventArrived2);
		m_pAdder->m_hEventArrived2 = NULL;
	}
	HRESULT hr = CBaseInputPin::EndOfStream();
    return hr;
}



// here's the next block of data from the stream.
// AddRef it yourself if you need to hold it beyond the end
// of this call.
HRESULT CAdderInputPin::Receive(IMediaSample * pSample)
{
    HRESULT hr;
    //CAutoLock lck(&m_pAdder->m_csReceive);
    ASSERT(pSample);

    // check all is well with the base class
    hr = CBaseInputPin::Receive(pSample);
    if (S_OK == hr) 
	{
        hr = m_pAdder->ReceiveOptional (pSample);
    }
    return hr;
}

// the following removes a very large number of level 4 warnings from the microsoft
// compiler output, which are not useful at all in this case.
#pragma warning(disable:4514)
