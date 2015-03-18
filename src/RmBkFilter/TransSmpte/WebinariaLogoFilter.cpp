#include <atlbase.h>
#include <streams.h>
#include "..\iRGBFilters.h"
#include "WebinariaLogoFilter.h"
#include "..\resource.h"
#include <ks.h>
#include <ksmedia.h>
#include <dvdmedia.h>

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

const AMOVIESETUP_FILTER sudWebinLogo =
{
    &CLSID_CWebinariaLogoFilter,// Filter CLSID
    L"WebinariaLogo",			// String name
    MERIT_DO_NOT_USE,			// Filter merit
    2,							// Number pins
    psudPins						// Pin details
};

CUnknown * WINAPI CWebinariaLogoFilter::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CWebinariaLogoFilter(lpunk, phr);
    if (punk == NULL) 
    {
        *phr = E_OUTOFMEMORY;
    }
    return punk;

}

CWebinariaLogoFilter::CWebinariaLogoFilter(LPUNKNOWN punk, HRESULT *phr) 
:CTransInPlaceFilter(TEXT("WebinariaLogo"), punk, CLSID_CWebinariaLogoFilter, phr)
,m_nWidth(0)
,m_nHeight(0)
,m_hDibSectionLogo(NULL)
,m_hBmpLogo(NULL)
,m_DC(NULL)
,m_nLogoWidth(0)
,m_nLogoHeight(0)
,m_nLogoX(8)
,m_nLogoY(58)
,m_smallLogoPath(0)
,m_largeLogoPath(0)
{
}
//------------------------------------------------------------------------------
CWebinariaLogoFilter::~CWebinariaLogoFilter( )
{
	FreeGdiObjects();
	DeleteLogoPaths();
}
//------------------------------------------------------------------------------
// FreeGdiObjects
//
void CWebinariaLogoFilter::FreeGdiObjects(void)
{
    if( m_DC )
    {
        DeleteDC( m_DC );
        m_DC = NULL;
    }
	if(m_hDibSectionLogo)
    {
        DeleteObject( m_hDibSectionLogo );
        m_hDibSectionLogo = NULL;
    }
	if(m_hBmpLogo)
	{
		DeleteObject(m_hBmpLogo);
		m_hBmpLogo = NULL;
	}
} // FreeGdiObjects
										   
HRESULT CWebinariaLogoFilter::SetMediaType( PIN_DIRECTION pindir, const CMediaType *pMediaType)
{
    CheckPointer(pMediaType,E_POINTER);

    VIDEOINFO* pVI = (VIDEOINFO*) pMediaType->Format();
    CheckPointer(pVI,E_UNEXPECTED);

    m_nWidth  = pVI->bmiHeader.biWidth;
    m_nHeight = pVI->bmiHeader.biHeight;

    return CTransInPlaceFilter::SetMediaType( pindir, pMediaType );
}


HRESULT CWebinariaLogoFilter::CheckInputType(const CMediaType *mtIn)
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
STDMETHODIMP CWebinariaLogoFilter::Pause()
{
	if(State_Stopped==m_State)
	{
		FreeGdiObjects();

		VIDEOINFO* pVI = (VIDEOINFO*) m_pInput->CurrentMediaType( ).Format( );
		CheckPointer(pVI, E_UNEXPECTED);
	
		// load a logo from resource
		if(m_nWidth<=320)
		{
			m_hBmpLogo = (!m_smallLogoPath) ? LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LOGO8BIT112X48)) : 
				(HBITMAP)LoadImage(0, m_smallLogoPath, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
		}
		else
		{
			m_hBmpLogo = (!m_smallLogoPath) ? LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LOGO8BIT176X78)) : 
				(HBITMAP)LoadImage(0, m_smallLogoPath, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE);
		}
		if(!m_hBmpLogo)
			return E_UNEXPECTED;

		HDC hdc = GetDC( NULL );
		m_DC = CreateCompatibleDC( hdc );

		if( !m_DC )
			return E_FAIL;

		ReleaseDC( NULL, hdc );

		BITMAPINFO bmi;
		ZeroMemory(&bmi, sizeof(bmi));
		bmi.bmiHeader.biSize= sizeof(bmi.bmiHeader);

		int iLines = GetDIBits(m_DC, m_hBmpLogo, 0, 0, NULL, &bmi, DIB_RGB_COLORS);

		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biPlanes   = 1;
		bmi.bmiHeader.biBitCount = 24;
		bmi.bmiHeader.biClrUsed  = 0;	
		bmi.bmiHeader.biSizeImage = m_lBytesLogoSize = GetBitmapSize(&bmi.bmiHeader);

		m_hDibSectionLogo = CreateDIBSection(m_DC, &bmi, DIB_RGB_COLORS, (void**)&m_pBytesLogo, NULL, 0);

		if( !m_hDibSectionLogo )
		{
			return E_FAIL;
		}
		m_nLogoWidth			= bmi.bmiHeader.biWidth;
		m_nLogoHeight			= bmi.bmiHeader.biHeight;
		m_nLogoX				= (m_nWidth - m_nLogoWidth - 24) % 0xfff8;
		m_nFrameLineLength		= pVI->bmiHeader.biWidth * (pVI->bmiHeader.biBitCount / 8);
		m_nFrameLogoRawOffset	= m_nFrameLineLength * m_nLogoY + m_nLogoX * (pVI->bmiHeader.biBitCount / 8);

		iLines = GetDIBits(	m_DC, m_hBmpLogo, 0, m_nLogoHeight, m_pBytesLogo, &bmi, DIB_RGB_COLORS);
	}
	return CTransInPlaceFilter::Pause();
}
STDMETHODIMP CWebinariaLogoFilter::Run(REFERENCE_TIME tStart)
{
    return CTransInPlaceFilter::Run(tStart);
}

//#define ADD_LUMA(sp, Add) ((DWORD)(sp) + Add) 
#define ADD_LUMA(sp, Add) ((LONG)(sp) - Add) 

inline void ApplyLuma(SRGB* pRgb, DWORD Luma)
{
	if(0==Luma) return;

	LONG AddVal = ( 63*Luma ) / 255;

	LONG Val;

	Val = ADD_LUMA(pRgb->r, AddVal);
	if(Val<15) Val=31;
	pRgb->r = (BYTE)Val;

	Val = ADD_LUMA(pRgb->g, AddVal);
	if(Val<15) Val=31;
	pRgb->g = (BYTE)Val;

	Val = ADD_LUMA(pRgb->b, AddVal);
	if(Val<15) Val=31;
	pRgb->b = (BYTE)Val;
};

HRESULT CWebinariaLogoFilter::Transform(IMediaSample *pSample)
{//it is work only for RGB24
    CheckPointer(pSample, E_POINTER);
   
	LPBYTE pFrame0 = NULL;
	LPBYTE pFrame = NULL;
	LPBYTE pLogo = m_pBytesLogo;
    pSample->GetPointer( &pFrame0 );
		
	for(int LogoLine=0; LogoLine<m_nLogoHeight; LogoLine++)
	{
		pFrame = pFrame0 + m_nFrameLogoRawOffset+LogoLine*m_nFrameLineLength;
		for(int LogoX=0; LogoX<m_nLogoWidth; LogoX++, pLogo+=3, pFrame+=3)
		{
			ApplyLuma((SRGB*)pFrame, *pLogo);
		}
	}
    return NOERROR;
}

void CWebinariaLogoFilter::DeleteLogoPaths()
{
	if (m_smallLogoPath)
	{
		delete[] m_smallLogoPath;
		m_smallLogoPath = 0;
	}
	if (m_largeLogoPath)
	{
		delete[] m_largeLogoPath;
		m_largeLogoPath = 0;
	}
}

STDMETHODIMP CWebinariaLogoFilter::SetLogoPath(BSTR smallLogoPath, BSTR largeLogoPath)
{
	COLE2T smallPath(smallLogoPath), largePath(largeLogoPath);
	UINT smallPathLen = _tcslen(smallPath.m_psz), largePathLen = _tcslen(largePath.m_psz);

	DeleteLogoPaths();

	m_smallLogoPath = new TCHAR[smallPathLen + 1];
	bool ok = !_tcscpy_s(m_smallLogoPath, smallPathLen + 1, smallPath.m_psz);

	m_largeLogoPath = new TCHAR[largePathLen + 1];
	ok = ok && !_tcscpy_s(m_largeLogoPath, largePathLen + 1, largePath.m_psz);

	return (ok) ? S_OK : E_OUTOFMEMORY;
}

STDMETHODIMP CWebinariaLogoFilter::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
    CheckPointer(ppv,E_POINTER);
    {
		if(IID_ILogoFilter == riid)
		{
			return GetInterface((ILogoFilter *) this, ppv);
		}

		return CTransInPlaceFilter::NonDelegatingQueryInterface(riid, ppv);
    }
}
