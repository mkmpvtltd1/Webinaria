#include "Remover.h"

#pragma once

extern const AMOVIESETUP_FILTER sudTransSmpte;

class CRemoveBackground : public CTransInPlaceFilter
{
public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    DECLARE_IUNKNOWN;

    HRESULT Transform(IMediaSample *pSample);
    HRESULT CheckInputType(const CMediaType *mtIn);
    HRESULT SetMediaType( PIN_DIRECTION pindir, const CMediaType *pMediaType);

private:

    // Constructor
    CRemoveBackground( LPUNKNOWN punk, HRESULT *phr );
    ~CRemoveBackground( );

    int         m_nWidth;
    int         m_nHeight;
};

