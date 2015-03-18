//

#include <streams.h>

#include "..\iRGBFilters.h"
#include "AudioSkipper.h"


static const AMOVIESETUP_MEDIATYPE
sudInputPinTypes =   { &MEDIATYPE_Audio              // clsMajorType
                     , &MEDIASUBTYPE_NULL }  ;       // clsMinorType

static const AMOVIESETUP_MEDIATYPE
sudOutputPinTypes =   { &MEDIATYPE_Audio              // clsMajorType
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

const AMOVIESETUP_FILTER sudAudioSkipper =
{
    &CLSID_CAudioSkipper,	// Filter CLSID
    L"AudioSkipper",		// String name
    MERIT_DO_NOT_USE,		// Filter merit
    2,						// Number pins
    psudPins				// Pin details
};
//------------------------------------------------------------------------------
CAudioSkipper::CAudioSkipper(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
:CTransInPlaceFilter(tszName, punk, CLSID_CAudioSkipper, phr)
,AudioDeleteInterval()
,AllDeleteInterval()
,m_llAudioSamplesCount(0)
,m_llVideoSamplesCount(0)

{
	
}
//------------------------------------------------------------------------------
CAudioSkipper::~CAudioSkipper()
{
	
}
//------------------------------------------------------------------------------
STDMETHODIMP CAudioSkipper::Pause()
{
	if(m_State==State_Stopped)
	{
		m_llDeliveredSampleNo = m_llSampleNo = 0;
		WAVEFORMATEX* pWfx = (WAVEFORMATEX*) m_pInput->CurrentMediaType( ).Format( );
		CheckPointer(pWfx, E_UNEXPECTED);
		m_lChannelsCount = pWfx->nChannels;
		m_lBytesPerSample = pWfx->wBitsPerSample/8;
		if(0==m_lBytesPerSample) m_lBytesPerSample = 1;
	}
	return CTransInPlaceFilter::Pause();
}
//------------------------------------------------------------------------------
HRESULT CAudioSkipper::Transform(IMediaSample *pOut)
{
	if(!pOut)	
		return S_FALSE;

	LONGLONG llRelatedFrame = GetRelatedFrame(m_llSampleNo);
	LONG lLength =pOut->GetActualDataLength();
	m_llSampleNo+=lLength/(m_lBytesPerSample*m_lChannelsCount);

	if(AllDeleteInterval.In(llRelatedFrame))
	{
		//just do not deliver a sample at all
		return S_FALSE;
	}

	if(AudioDeleteInterval.In(llRelatedFrame))
	{
			BYTE* pBuf = NULL;
			pOut->GetPointer(&pBuf);
			if(pBuf)
			{	//Make a silence:
				LONG lDataLength = pOut->GetActualDataLength();
				ZeroMemory(pBuf, lDataLength);
			}
	}

	//force mediatimes
	REFERENCE_TIME rtStart(0);
	REFERENCE_TIME rtStop(0);

	LONGLONG llTimeEnd = m_llDeliveredSampleNo+1;
	pOut->SetMediaTime(&m_llDeliveredSampleNo, &llTimeEnd);

	pOut->GetTime(&rtStart, &rtStop);
	REFERENCE_TIME rtDelta = rtStop-rtStart;
	rtStart = m_llDeliveredSampleNo*rtDelta;
	rtStop  = rtStart+rtDelta;
	pOut->SetTime(&rtStart, &rtStop);

	m_llDeliveredSampleNo = llTimeEnd;

	return S_OK;
}
//------------------------------------------------------------------------------
LONGLONG CAudioSkipper::GetRelatedFrame(const LONGLONG& llSampleNo)
{
LONGLONG lResult(-1);
	if(m_llAudioSamplesCount && m_llVideoSamplesCount)
	{
		lResult = (llSampleNo * m_llVideoSamplesCount) / m_llAudioSamplesCount;
	}
	return lResult;
}
//------------------------------------------------------------------------------
HRESULT CAudioSkipper::SetIntervals(void * MainIntervals, void * AudioIntervals)
{
	if (MainIntervals)
		AllDeleteInterval = *((SInterval *) MainIntervals);
	else
		AllDeleteInterval.Reset();

	if (AudioIntervals)
		AudioDeleteInterval = *((SInterval *) AudioIntervals);
	else
		AudioDeleteInterval.Reset();
	return S_OK;	
}
//------------------------------------------------------------------------------
HRESULT CAudioSkipper::SetSamplesCount(LONGLONG llFramesCount, LONGLONG llSamplesCount)
{
	m_llAudioSamplesCount = llSamplesCount;
	m_llVideoSamplesCount = llFramesCount;
	return S_OK;
}
//------------------------------------------------------------------------------
// this goes in the factory template table to create new instances
CUnknown * WINAPI CAudioSkipper::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
	ASSERT(phr);
    
    // assuming we don't want to modify the data
	CAudioSkipper *pNewObject = new CAudioSkipper(TEXT("AudioSkipper"), punk, phr);

    if(pNewObject == NULL)
	{
        if (phr)
            *phr = E_OUTOFMEMORY;
    }
    return pNewObject;   
}
//------------------------------------------------------------------------------
// NonDelegatingQueryInterface
//
STDMETHODIMP CAudioSkipper::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv, E_POINTER);

    if(IID_IAudioSkip==riid)
        return GetInterface((IAudioSkip *) this, ppv);
    else if (riid == IID_ITimelineFilter)
        return GetInterface((ITimelineFilter *) this, ppv);

    return CTransInPlaceFilter::NonDelegatingQueryInterface(riid, ppv);
} // NonDelegatingQueryInterface
//------------------------------------------------------------------------------
