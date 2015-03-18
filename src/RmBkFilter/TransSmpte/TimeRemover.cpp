
#include <streams.h>          // quartz, includes windows
#include "..\iRGBFilters.h"

#include "TimeRemover.h"

// ----------------------------------------------------------------------------
// Implementation of pins and filter
// ----------------------------------------------------------------------------
// DbgFunc
//
// Put out the name of the function and instance on the debugger.
// Call this at the start of interesting functions to help debug
//
#define DbgFunc(a) DbgLog(( LOG_TRACE                        \
                          , 2                                \
                          , TEXT("CTimeRemover(Instance %d)::%s") \
                          , m_nThisInstance                  \
                          , TEXT(a)                          \
                         ));

//------------------------------------------------------------------------------
// setup data
//------------------------------------------------------------------------------
static const AMOVIESETUP_MEDIATYPE
sudInputPinTypes =   { &MEDIATYPE_NULL              // clsMajorType
                     , &MEDIASUBTYPE_NULL }  ;       // clsMinorType

static  const AMOVIESETUP_MEDIATYPE
sudOutputPinTypes =   { &MEDIATYPE_NULL              // clsMajorType
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

const AMOVIESETUP_FILTER sudTimeRemover = 
{ 
            &CLSID_CTimeRemover      // clsID
            , L"TimeRemover"         // strName
            , MERIT_DO_NOT_USE       // dwMerit
            , 2                      // nPins
            , psudPins               // lpPin
};                     



//
// initialise the static instance count.
//
//int CTimeRemover::m_nInstanceCount = 0;

// ----------------------------------------------------------------------------
// Filter implementation:
//------------------------------------------------------------------------------
// CTimeRemover::Constructor
//
CTimeRemover::CTimeRemover(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
:CTransInPlaceFilter (tszName, punk, CLSID_CTimeRemover, phr)
{
} // (CTimeRemover constructor)

//------------------------------------------------------------------------------
//
//
HRESULT CTimeRemover::CheckConnect(PIN_DIRECTION dir,IPin *pPin)
{
HRESULT hr=S_OK;
    if(dir==PINDIR_INPUT)
    {
    //TODO: Insert custom checks here
    }
    else if(dir==PINDIR_OUTPUT)
    {
        //TODO: Insert custom checks here
    }
    else
    {
        hr=E_INVALIDARG;
    }
    return hr;
} // CheckConnect
//------------------------------------------------------------------------------
//
// CreateInstance
//
// Override CClassFactory method.
// Provide the way for COM to create a CTimeRemover object
//
CUnknown * WINAPI CTimeRemover::CreateInstance(LPUNKNOWN punk, HRESULT *phr) 
{
    CTimeRemover *pNewObject = new CTimeRemover(NAME("TimeRemover Filter"), punk, phr );
    if (pNewObject == NULL) 
    {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
} // CreateInstance
//------------------------------------------------------------------------------
// NonDelegatingQueryInterface
//
STDMETHODIMP CTimeRemover::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);

    {
        return CTransInPlaceFilter::NonDelegatingQueryInterface(riid, ppv);
    }

} // NonDelegatingQueryInterface
//------------------------------------------------------------------------------
// Transform
//
HRESULT CTimeRemover::Transform(IMediaSample *pSample)
{
REFERENCE_TIME TimeStart, TimeEnd;
HRESULT hr = S_OK;
	TimeStart = TimeEnd = 0;
	hr = pSample->SetTime(&TimeStart,&TimeEnd);
    return hr;
} // Transform
//------------------------------------------------------------------------------

// Microsoft C Compiler will give hundreds of warnings about
// unused inline functions in header files.  Try to disable them.
#pragma warning( disable:4514)
