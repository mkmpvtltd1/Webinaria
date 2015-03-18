//
#include <streams.h>
#include "..\iRGBFilters.h"
#include "TransSmpte.h"

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


const AMOVIESETUP_FILTER sudTransSmpte =
{
    &CLSID_CRemoveBackground,   // Filter CLSID
    L"RemoveBackGround",		// String name
    MERIT_DO_NOT_USE,			// Filter merit
    2,							// Number pins
    psudPins					// Pin details
};

CUnknown * WINAPI CRemoveBackground::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
   
    CUnknown *punk = new CRemoveBackground(lpunk, phr);
    if (punk == NULL) 
    {
        *phr = E_OUTOFMEMORY;
    }
    return punk;

}

CRemoveBackground::CRemoveBackground(LPUNKNOWN punk,HRESULT *phr) 
    : CTransInPlaceFilter(TEXT("RemoveBackGround"), punk, CLSID_CRemoveBackground, phr)
    , m_nWidth(0)
    , m_nHeight(0)
{
}

CRemoveBackground::~CRemoveBackground( )
{
}


HRESULT CRemoveBackground::SetMediaType( PIN_DIRECTION pindir, const CMediaType *pMediaType)
{
    CheckPointer(pMediaType,E_POINTER);

    VIDEOINFO* pVI = (VIDEOINFO*) pMediaType->Format();
    CheckPointer(pVI,E_UNEXPECTED);

    m_nWidth  = pVI->bmiHeader.biWidth;
    m_nHeight = pVI->bmiHeader.biHeight;

    return CTransInPlaceFilter::SetMediaType( pindir, pMediaType );
}


HRESULT CRemoveBackground::CheckInputType(const CMediaType *mtIn)
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


HRESULT CRemoveBackground::Transform(IMediaSample *pSample)
{
    CheckPointer(pSample,E_POINTER);

    char *pBuffer = 0;
    pSample->GetPointer( (LPBYTE*) &pBuffer );

	CRemover::ClearBack(pBuffer, m_nWidth, m_nHeight);

   return NOERROR;
}