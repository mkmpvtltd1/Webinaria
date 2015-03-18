
#include <streams.h>          // quartz, includes windows
#include "..\iRGBFilters.h"

#include "Synchronizer.h"

//------------------------------------------------------------------------------
CSynchronizer*	CSynchronizer::s_pInstances[MAX_SYNC_INSTANCES] = {NULL,NULL,NULL,NULL};
CSynchronizer*	CSynchronizer::s_pMaster = NULL;
CCritSec		CSynchronizer::s_ccVideoGate;
bool			CSynchronizer::s_fGateOpen = false;
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
                          , TEXT("CSynchronizer(Instance %d)::%s") \
                          , m_nThisInstance                  \
                          , TEXT(a)                          \
                         ));

//------------------------------------------------------------------------------
// setup data
//------------------------------------------------------------------------------
static const AMOVIESETUP_MEDIATYPE
sudInputPinTypes =   { &MEDIATYPE_NULL              // clsMajorType
                     , &MEDIASUBTYPE_NULL }  ;       // clsMinorType

static const AMOVIESETUP_MEDIATYPE
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

const AMOVIESETUP_FILTER sudSynchronizer = 
{ 
            &CLSID_CSynchronizer      // clsID
            , L"Synchronizer"         // strName
            , MERIT_DO_NOT_USE       // dwMerit
            , 2                      // nPins
            , psudPins               // lpPin
};                     



//
// initialise the static instance count.
//
//int CSynchronizer::m_nInstanceCount = 0;

// ----------------------------------------------------------------------------
// Filter implementation:
//------------------------------------------------------------------------------
// CSynchronizer::Constructor
//
CSynchronizer::CSynchronizer(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
:CTransInPlaceFilter (tszName, punk, CLSID_CSynchronizer, phr)
,m_InstanceId(-1)
{
	for(int i=0; i<MAX_SYNC_INSTANCES; i++)
	{
		if(NULL==s_pInstances[i])
		{
			s_pInstances[i] = this;
			m_InstanceId = i;
			break;
		}
	}
	if(m_InstanceId<0)
	{
		*phr=E_OUTOFMEMORY;
		return;
	}
} // (CSynchronizer constructor)
//------------------------------------------------------------------------------
CSynchronizer::~CSynchronizer()
{
	if(m_InstanceId>=0)
	{
		s_pInstances[m_InstanceId] = NULL;
	}
}
//------------------------------------------------------------------------------
//
//
HRESULT CSynchronizer::CheckConnect(PIN_DIRECTION dir,IPin *pPin)
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
// Provide the way for COM to create a CSynchronizer object
//
CUnknown * WINAPI CSynchronizer::CreateInstance(LPUNKNOWN punk, HRESULT *phr) 
{
    CSynchronizer *pNewObject = new CSynchronizer(NAME("Synchronizer Filter"), punk, phr );
    if (pNewObject == NULL) 
    {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
} // CreateInstance
//------------------------------------------------------------------------------
// NonDelegatingQueryInterface
//
STDMETHODIMP CSynchronizer::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);
	if (riid == IID_IParams)
	{
        return GetInterface((IParams *) this, ppv);
	}
	else
    {
        return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
    }

} // NonDelegatingQueryInterface
//------------------------------------------------------------------------------
// Pause
//
STDMETHODIMP CSynchronizer::Pause()
{
HRESULT hr;
	if(m_State == State_Stopped)
	{
		m_fFirstSampleArrived = false;
		m_iGateType = fgtUnknown;
		m_llTotal = m_llMediaTime = 0;
		if(0 == wcscmp(m_pName, L"SC Synchronizer"))
		{
			m_iGateType = fgtMaster;
			s_fGateOpen = false;
			s_pMaster = this;
		}
		else if(0 == wcscmp(m_pName, L"Webcam Synchronizer"))
		{
			m_iGateType = fgtSlave;
		}
	}
	hr = CTransInPlaceFilter::Pause();
	return hr;
}
//------------------------------------------------------------------------------
// Stop
//
STDMETHODIMP CSynchronizer::Stop()
{
	HRESULT hr = CTransInPlaceFilter::Stop();
	if(s_pMaster == this)
		s_pMaster = NULL;
	DbgLog(( LOG_TRACE, 2, TEXT("Instance:  %ws"), m_pName));
	DbgLog(( LOG_TRACE, 2, TEXT("Samples  Total:  %016I64d"), m_llTotal));
	DbgLog(( LOG_TRACE, 2, TEXT("Samples Passed:  %016I64d"), m_llMediaTime));
	return hr;
}
//------------------------------------------------------------------------------
// Transform
//
HRESULT CSynchronizer::Transform(IMediaSample *pSample)
{
//REFERENCE_TIME TimeStart, TimeEnd;
HRESULT hr = S_OK;
	m_llTotal++;
	m_fFirstSampleArrived = true;
	CSynchronizer* pInst;
	for(int i=0; i<MAX_SYNC_INSTANCES; i++)
	{
		pInst = s_pInstances[i];
		if(pInst && pInst!=this && !pInst->m_fFirstSampleArrived)
		{//skip sample if any other filter does not accept first sample
			return S_FALSE;
		}
	}

	if(fgtSlave==m_iGateType)
	{
		CAutoLock Lock(&s_ccVideoGate);
		if(!s_fGateOpen && s_pMaster && m_llMediaTime>=s_pMaster->m_llMediaTime)
		{
			return S_FALSE;
		}
		s_fGateOpen = false;
	}
	else if(fgtMaster==m_iGateType)
	{
		CAutoLock Lock(&s_ccVideoGate);
		s_fGateOpen = true;
	}
	LONGLONG llStartTime = m_llMediaTime;
	m_llMediaTime++;
	pSample->SetMediaTime(&llStartTime, &m_llMediaTime);
	if(fgtSlave==m_iGateType && s_pMaster)
	{//duplicate frame
		while(m_llMediaTime<s_pMaster->m_llMediaTime)
		{
			LONGLONG llStartTime = m_llMediaTime;
			m_llMediaTime++;
			pSample->SetMediaTime(&llStartTime, &m_llMediaTime);
			hr = m_pOutput->Deliver(pSample);
		}
	}
    return hr;
} // Transform
//------------------------------------------------------------------------------
HRESULT CSynchronizer::SetLong( LONG lParamId, LONGLONG llParamValue)
{
	return E_INVALIDARG;
}
//------------------------------------------------------------------------------
HRESULT CSynchronizer::GetLong( LONG lParamId, LONGLONG* pllParamValue)
{
	CheckPointer(pllParamValue, E_POINTER);
	switch(lParamId)
	{
		case roParamSamplesTotal:
			*pllParamValue = m_llTotal;
			return S_OK;
		case roParamSamplesPassed:
			*pllParamValue = m_llMediaTime;
			return S_OK;
	}
	return E_INVALIDARG;
}
//------------------------------------------------------------------------------

// Microsoft C Compiler will give hundreds of warnings about
// unused inline functions in header files.  Try to disable them.
#pragma warning( disable:4514)
