#pragma once

#include "intervals.h"
#include "TimelineFilter.h"

extern const AMOVIESETUP_FILTER sudTransNotes;

class CNotesAdder   : public CTransInPlaceFilter
                    , public INotesAdder
                    , public CTimelineFilter
{
protected:
	void ClearGdiObjects(void);
public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    
    HRESULT CheckInputType(const CMediaType *mtIn);
    HRESULT SetMediaType( PIN_DIRECTION pindir, const CMediaType *pMediaType);

	STDMETHODIMP SetIntervals(	void * TextIntervals,
								void * ArrowIntervals);
	DECLARE_IUNKNOWN;
	
    STDMETHODIMP Run(REFERENCE_TIME tStart);
	//STDMETHODIMP Stop();
	STDMETHODIMP Pause();
	HRESULT Transform(IMediaSample *pSample);

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

private:
	REFERENCE_TIME MediaStart;
	STextInterval TextInterval;
	SArrowInterval ArrowInterval;

    // Constructor
    CNotesAdder(LPUNKNOWN punk, HRESULT *phr);
	virtual ~CNotesAdder();

    int         m_nWidth;
    int         m_nHeight;
    HBITMAP     m_DibSection;
    HDC         m_DC;
    void *      m_pDibBits;
    HGDIOBJ     m_OldObject;
	LONGLONG	m_llFrameNo;
};