//
#pragma once
#include "intervals.h"
#include "NotesAdder.h"
#include "TimelineFilter.h"

//extern const AMOVIESETUP_PIN psudAdderPins[];

extern const AMOVIESETUP_FILTER sudAdder;

// ======================================================================
// This is the com object that represents a simple transform filter. It
// supports IBaseFilter, IMediaFilter and two pins through nested interfaces
// ======================================================================

class CAdder;

// ==================================================
// Implements the input pin
// ==================================================

class CAdderInputPin : public CBaseInputPin
{
    friend class CAdder;

private:

    CAdder *m_pAdder;
	int Num;
    CAdderInputPin(
        TCHAR *pObjectName,
        CAdder *pAdder,
        HRESULT * phr,
        LPCWSTR pName,
		const int & num);

public:
	BOOL	m_fEosRecived;
	STDMETHODIMP QueryId(LPWSTR * Id)
    {
        return AMGetWideString(L"In", Id);
    }

    // Grab and release extra interfaces if required

    HRESULT CheckConnect(IPin *pPin);
    HRESULT BreakConnect();
    HRESULT CompleteConnect(IPin *pReceivePin);

    // check that we can support this output type
    HRESULT CheckMediaType(const CMediaType* mtIn);

    // set the connection media type

    // --- IMemInputPin -----

    // here's the next block of data from the stream.
    // AddRef it yourself if you need to hold it beyond the end
    // of this call.
    STDMETHODIMP Receive(IMediaSample * pSample);

    // provide EndOfStream that passes straight downstream
    // (there is no queued data)
    STDMETHODIMP EndOfStream(void);
/*
    // passes it to CTransformFilter::BeginFlush
    STDMETHODIMP BeginFlush(void);

    // passes it to CTransformFilter::EndFlush
    STDMETHODIMP EndFlush(void);
*/
    // Check if it's OK to process samples
//    virtual HRESULT CheckStreaming();

    // Media type
public:
    CMediaType& CurrentMediaType() { return m_mt; };

};

void SaveAsJPEG(const wchar_t * File);

class CAdder    : public CTransInPlaceFilter
                , public CTimelineFilter
                , public IAdder

{

public:

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    // map getpin/getpincount for base enum of pins to owner
    // override this to return more specialised pin objects

    virtual int GetPinCount();
    virtual CBasePin * GetPin(int n);
    STDMETHODIMP FindPin(LPCWSTR Id, IPin **ppPin);

    // override state changes to allow derived transform filter
    // to control streaming start/stop
    STDMETHODIMP Stop();
    STDMETHODIMP Pause();
	STDMETHODIMP Run(REFERENCE_TIME tStart);

	STDMETHODIMP SetIntervals(	void * MainIntervals,
								void * WebIntervals,
								void * WebPosIntervals,
								void * ArrowIntervals,
								void * TextIntervals);
    STDMETHODIMP SetThumbnailPath(BSTR path);

//    STDMETHODIMP AssignTimeline( ITimeline* pTimeline);
    //
	DECLARE_IUNKNOWN;
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

private:
	
	TCHAR		m_tcThumbnailPath[MAX_PATH];

	SInterval AllDeleteInterval;
	SInterval WebDeleteInterval;
	SPosInterval WebPosInterval;
	STextInterval TextInterval;
	SArrowInterval ArrowInterval;

	bool DeletedWeb;

	REFERENCE_TIME	m_rtTimePerFrame;
	REFERENCE_TIME	m_rtTimePerFrame2;

	LONGLONG		m_llArrivedSampleNo;	//Serial number for sample arrived to input #0
	LONGLONG		m_llArrivedSampleNo2;	//Serial number for sample arrived to optional input 
	LONGLONG		m_llDeliveredSampleNo;  //Serial number for sample passed to downstream

	LONGLONG		m_llFramesCount;
	LONGLONG		m_llFramesCount2;

	HANDLE			m_hEventArrived;		//Sample at input #1 arrived
	HANDLE			m_hEventArrived2;		//Sample at optional input arrived

//	LONG		   m_lThumbnailOffset; //Thumbnail grabbing stream offset in seconds
//  LONGLONG	   m_llThumbnailSampleNo;

	HBITMAP     m_DibSection;
    HDC         m_DC;
    void *      m_pDibBits;
    HGDIOBJ     m_OldObject;
    FRAME_INFO  m_FrameInfo;

private:
    CAdder(TCHAR *, LPUNKNOWN, HRESULT* phr);
    ~CAdder();

public:

    // =================================================================
    // ----- override these bits ---------------------------------------
    // =================================================================

    // These must be supplied in a derived class
    virtual HRESULT Transform(IMediaSample *pOut);

    // check if you can support mtIn
    virtual HRESULT CheckInputType(const CMediaType* mtIn);

	//method to handle optional input stream
    virtual HRESULT ReceiveOptional(IMediaSample *pSample);


protected:

	void CreateThumbnail(const HDC hDC, const HBITMAP hBMP, PTCHAR ptcFile, PBITMAPINFO pbi);
	void ClearGDIObjects(void);

	int         m_nWidth;
    int         m_nHeight;
	int			m_nFrameBuf1Size;
	int         m_nWidth2;
    int         m_nHeight2;
	BYTE *		pbBufIn2;
	BOOL		SecondExists;

    BOOL m_bEOSDelivered;              // have we sent EndOfStream
    BOOL m_bSampleSkipped;             // Did we just skip a frame
    BOOL m_bQualityChanged;            // Have we degraded?

    // critical section protecting filter state.

    CCritSec m_csFilter;
    CCritSec m_csReceive;

    // these hold our input and output pins

    friend class CAdderInputPin;
	CAdderInputPin*  m_pAInput2;
};