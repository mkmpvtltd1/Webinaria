//
#pragma once

#include "intervals.h"
#include "TimelineFilter.h"

extern const AMOVIESETUP_FILTER sudAudioSkipper;


class CAudioSkipper : public CTransInPlaceFilter
					, public IAudioSkip
                    , public CTimelineFilter
{
private:
	
protected:
	CAudioSkipper(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
	SInterval		AudioDeleteInterval;
	SInterval		AllDeleteInterval;
	LONGLONG		m_llSampleNo;
	LONGLONG		m_llDeliveredSampleNo;
	LONGLONG		m_llAudioSamplesCount;
	LONGLONG		m_llVideoSamplesCount;
	LONG			m_lChannelsCount;
	LONG			m_lBytesPerSample;
	LONGLONG		GetRelatedFrame(const LONGLONG& llSampleNo);
public:
	virtual ~CAudioSkipper();
		DECLARE_IUNKNOWN;
        //
        // --- CTransInPlaceFilter Overrides --
        //
//        HRESULT CheckConnect(PIN_DIRECTION dir, IPin *pPin);
        HRESULT CheckInputType(const CMediaType* mtIn)
            { UNREFERENCED_PARAMETER(mtIn);  return S_OK; }

		HRESULT Transform(IMediaSample *pSample);
		STDMETHODIMP Pause();
        STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
		STDMETHODIMP SetIntervals(void * MainIntervals, void * AudioIntervals);	
		STDMETHODIMP SetSamplesCount(LONGLONG llFramesCount, LONGLONG llSamplesCount);
    // this goes in the factory template table to create new instances
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

};
