#pragma once

extern const AMOVIESETUP_FILTER sudSynchronizer;

#define MAX_SYNC_INSTANCES 4




class CSynchronizer : public CTransInPlaceFilter
					, public IParams
{

enum FrameGateType {fgtUnknown = 0, fgtMaster, fgtSlave};

protected:
		static CSynchronizer*   s_pInstances[MAX_SYNC_INSTANCES];
		static	CCritSec		s_ccVideoGate;
		static	bool			s_fGateOpen;
		static CSynchronizer*	s_pMaster;
				int				m_InstanceId;
				bool			m_fFirstSampleArrived;
				int				m_iGateType;
				LONGLONG		m_llMediaTime;
				LONGLONG		m_llTotal;
public:
		virtual					~CSynchronizer();
        static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
		
        DECLARE_IUNKNOWN;
        //
        // --- CTransInPlaceFilter Overrides --
        //
		STDMETHODIMP Pause();
		STDMETHODIMP Stop();
        HRESULT CheckInputType(const CMediaType* mtIn)
            { UNREFERENCED_PARAMETER(mtIn);  return S_OK; }

        HRESULT CheckConnect(PIN_DIRECTION dir, IPin *pPin);
        //
        // Overrides the PURE virtual Transform of base class
        //
    HRESULT Transform(IMediaSample *pSample);
        // Basic COM - used here to reveal our property interface.
        STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
		STDMETHODIMP SetLong( LONG lParamId, LONGLONG llParamValue);
		STDMETHODIMP GetLong( LONG lParamId, LONGLONG* pllParamValue);

private:
        CSynchronizer(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
};
