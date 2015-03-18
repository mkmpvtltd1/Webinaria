#pragma once

extern const AMOVIESETUP_FILTER sudTimeRemover;

class CTimeRemover : public CTransInPlaceFilter
{
public:
        static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

        DECLARE_IUNKNOWN;
        //
        // --- CTransInPlaceFilter Overrides --
        //
        HRESULT CheckInputType(const CMediaType* mtIn)
            { UNREFERENCED_PARAMETER(mtIn);  return S_OK; }

        HRESULT CheckConnect(PIN_DIRECTION dir, IPin *pPin);
        //
        // Overrides the PURE virtual Transform of base class
        //
    HRESULT Transform(IMediaSample *pSample);
        // Basic COM - used here to reveal our property interface.
        STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

private:
        CTimeRemover(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
};
