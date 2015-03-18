
#include <streams.h>
#include "..\iRGBFilters.h"
#include "NotesAdder.h"


static const AMOVIESETUP_MEDIATYPE
sudInputPinTypes =   { &MEDIATYPE_Video              // clsMajorType
                     , &MEDIASUBTYPE_NULL }  ;       // clsMinorType

static const AMOVIESETUP_MEDIATYPE
sudOutputPinTypes =   { &MEDIATYPE_Video              // clsMajorType
                      , &MEDIASUBTYPE_NULL }  ;       // clsMinorType

static const AMOVIESETUP_PIN
psudPins[] = 
{
            { L"Input"              // strName
              , FALSE               // bRendered
              , FALSE               // bOutput
              , FALSE               // bZero
              , FALSE               // bMany
              , &CLSID_NULL         // clsConnectsToFilter
              , L"Output"           // strConnectsToPin
              , 1                   // nTypes
              , &sudInputPinTypes   // lpTypes
            }       
           ,
            {
              L"Output"             // strName
              , FALSE               // bRendered
              , TRUE                // bOutput
              , FALSE               // bZero
              , FALSE               // bMany
              , &CLSID_NULL         // clsConnectsToFilter
              , L"Input"            // strConnectsToPin
              , 1                   // nTypes
              , &sudOutputPinTypes  // lpTypes
            }
};


const AMOVIESETUP_FILTER sudTransNotes =
{
    &CLSID_CNotesAdder,			// Filter CLSID
    L"NotesAdder",				// String name
    MERIT_DO_NOT_USE,			// Filter merit
    2,							// Number pins
    psudPins					// Pin details
};

CUnknown * WINAPI CNotesAdder::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
   
    CUnknown *punk = new CNotesAdder(lpunk, phr);
    if (punk == NULL) 
    {
        *phr = E_OUTOFMEMORY;
    }
    return punk;

}

CNotesAdder::CNotesAdder(LPUNKNOWN punk,HRESULT *phr) 
:CTransInPlaceFilter(TEXT("NotesAdder"), punk, CLSID_CNotesAdder, phr)
,m_nWidth(0)
,m_nHeight(0)
,m_pDibBits(NULL)
,m_DibSection(NULL)
,m_DC(NULL)
,m_OldObject(NULL)
,MediaStart(-1)
,TextInterval()
,ArrowInterval()
,m_llFrameNo(0)
{
}

CNotesAdder::~CNotesAdder( )
{
	ClearGdiObjects();
}
void CNotesAdder::ClearGdiObjects(void)
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

STDMETHODIMP CNotesAdder::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
    CheckPointer(ppv, E_POINTER);

	if (riid == IID_INotesAdder)
        return GetInterface((INotesAdder *) this, ppv);
    else if (riid == IID_ITimelineFilter)
        return GetInterface((ITimelineFilter *) this, ppv);

    return CTransInPlaceFilter::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP CNotesAdder::SetIntervals(	void * TextIntervals,
										void * ArrowIntervals)
{
	if(TextIntervals)
		TextInterval = *((STextInterval *)TextIntervals);
	if(ArrowIntervals)
		ArrowInterval = *((SArrowInterval *)ArrowIntervals);
	return S_OK;
}


HRESULT CNotesAdder::SetMediaType( PIN_DIRECTION pindir, const CMediaType *pMediaType)
{
    CheckPointer(pMediaType,E_POINTER);

    VIDEOINFO* pVI = (VIDEOINFO*) pMediaType->Format();
    CheckPointer(pVI,E_UNEXPECTED);

    m_nWidth  = pVI->bmiHeader.biWidth;
    m_nHeight = pVI->bmiHeader.biHeight;

    return CTransInPlaceFilter::SetMediaType( pindir, pMediaType );
}


HRESULT CNotesAdder::CheckInputType(const CMediaType *mtIn)
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

	if( *mtIn->Subtype( ) != MEDIASUBTYPE_RGB24)
    {
        return E_INVALIDARG;
    }

    VIDEOINFO *pVI = (VIDEOINFO *) mtIn->Format();
    CheckPointer(pVI,E_UNEXPECTED);

    if( pVI->bmiHeader.biBitCount != 24 )
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
STDMETHODIMP CNotesAdder::Pause()
{
	if(m_State==State_Stopped)
	{
		m_llFrameNo = 0;
		ClearGdiObjects();
		VIDEOINFO* pVI = (VIDEOINFO*) m_pInput->CurrentMediaType( ).Format( );
		CheckPointer(pVI,E_UNEXPECTED);

        OverlaysDestroy();
        OverlaysInit(pVI);


		HDC hdc = GetDC( NULL );
		m_DC = CreateCompatibleDC( hdc );

		if( !m_DC )
			return E_FAIL;

		ReleaseDC( NULL, hdc );

		m_DibSection = CreateDIBSection(m_DC,
										(BITMAPINFO*) &pVI->bmiHeader,
										DIB_RGB_COLORS,
										&m_pDibBits,
										NULL,
										0);
		if( !m_DibSection )
		{
			return E_FAIL;
		}


		m_OldObject = SelectObject( m_DC, m_DibSection );
		if( !m_OldObject )
			return E_FAIL;
	}
	return CTransInPlaceFilter::Pause();
}
STDMETHODIMP CNotesAdder::Run(REFERENCE_TIME tStart)
{


	return CTransInPlaceFilter::Run(tStart);
}
HRESULT CNotesAdder::Transform(IMediaSample *pSample)
{
LONGLONG st(-1), en(-1);
    CheckPointer(pSample,E_POINTER);
	
	st = m_llFrameNo;
	m_llFrameNo++;
	en = m_llFrameNo;
	pSample->SetMediaTime(&st, &en);
/*
	
	HRESULT hr = pSample->GetMediaTime(&st, &en);
	if (FAILED(hr))
	{
		pSample->GetTime(&st, &en);
		st = st/(en - st);
	}
	//MediaStart = m_tStart.Millisecs();
*/	

//not used yet    OverlayDraw(m_llFrameNo, pSample);

	char * pBuffer = 0;
	pSample->GetPointer( (LPBYTE*) &pBuffer );
	long lDataLength = pSample->GetActualDataLength();
	// Copy the frame into our DIB section, onto which we will draw
	// the frame time
	memcpy( m_pDibBits, pBuffer, lDataLength );

	// Add arrow notes

	SArrowInterval * CurrentArrow = ArrowInterval.Next;

	while (NULL != CurrentArrow)
	{
		if ((CurrentArrow->Start <= st)&&(st <= CurrentArrow->End))
		{
			HPEN hPen = CreatePen(PS_SOLID, CurrentArrow->Width, CurrentArrow->Color);
			HPEN hOldPen = (HPEN)SelectObject(m_DC, hPen);

			int OldBkMode = SetBkMode(m_DC, OPAQUE);
			int ArrBX = 4*CurrentArrow->x2/5 + CurrentArrow->x1/5;
			int ArrBY = 4*CurrentArrow->y2/5 + CurrentArrow->y1/5;

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
	
			if (CurrentArrow->IsSelected)
			{
				RECT rc;
				if (CurrentArrow->x1<CurrentArrow->x2)
				{
					rc.left = CurrentArrow->x1;
					rc.right = CurrentArrow->x2;
				}
				else
				{
					rc.left = CurrentArrow->x2;
					rc.right = CurrentArrow->x1;
				}

				if (CurrentArrow->y1<CurrentArrow->y2)
				{
					rc.top = CurrentArrow->y1;
					rc.bottom = CurrentArrow->y2;
				}
				else
				{
					rc.top = CurrentArrow->y2;
					rc.bottom = CurrentArrow->y1;
				}
				DrawFocusRect( m_DC, &rc);
			}

			SetBkMode(m_DC, OldBkMode);

			SelectObject(m_DC, hOldPen);
			DeleteObject(hPen);
		}
		CurrentArrow = CurrentArrow->Next;
	}

	// Add text notes

	STextInterval * CurrentText = TextInterval.Next;

	while (NULL != CurrentText)
	{
		if ((CurrentText->Start <= st)&&(st <= CurrentText->End))
		{
   			HFONT hFont = CreateFontIndirect(&CurrentText->lf);
			HFONT hOldFont = (HFONT)SelectObject(m_DC, hFont);
			COLORREF OldColor = SetTextColor(m_DC, CurrentText->Color);

			int OldBkMode = SetBkMode(m_DC, TRANSPARENT);

			// Out the lettering
			TextOutW( m_DC, CurrentText->Left, CurrentText->Top, CurrentText->String, wcslen(CurrentText->String));

			SIZE sz;
			GetTextExtentPoint32W( m_DC, CurrentText->String, wcslen(CurrentText->String), &sz);

			if (CurrentText->IsSelected)
			{
				RECT rc;
				rc.left = CurrentText->Left;
				rc.top = CurrentText->Top;
				rc.right = rc.left + sz.cx;
				rc.bottom = rc.top + sz.cy;
				DrawFocusRect( m_DC, &rc);
			}

			SelectObject(m_DC, hOldFont);
			DeleteObject(hFont);

			SetBkMode(m_DC, OldBkMode);

			SetTextColor(m_DC, OldColor);
		}
		CurrentText = CurrentText->Next;
	}

	// Copy our modified bitmap into the original sample buffer
	memcpy( pBuffer, m_pDibBits, lDataLength);

    return NOERROR;
}