#pragma once

class CTimelineFilter : public ITimelineFilter
{
protected:
            //ITimeline*      m_pTimeline;
            IOverlaysEnum*  m_pOverlayEnum;
            IFrameOverlay*  m_pActualOverlay;
            FRAME_INFO      m_FrameInfo;
            HDC             m_DC;
	        HBITMAP         m_DibSection;
            void *          m_pDibBits;

public:
    CTimelineFilter(void) 
        : m_pOverlayEnum(NULL)
        , m_pActualOverlay(NULL)
        , m_DC(NULL)
        , m_DibSection(NULL)
        , m_pDibBits(NULL)
    {
        ZeroMemory(&m_FrameInfo, sizeof(m_FrameInfo));
    }
    virtual  ~CTimelineFilter(void)
    {
        OverlaysDestroy();
        if(m_pOverlayEnum)
        {
            m_pOverlayEnum->Release();
        }
    }
    //  ITimelineFilter implementation
    HRESULT STDMETHODCALLTYPE AssignTimeline( ITimeline *pTimeline)
    {
        HRESULT hr = S_OK;
        if(!pTimeline) return E_POINTER;
        if(m_pActualOverlay) return E_FAIL; //wrong state

        if(m_pOverlayEnum)
        {            
            m_pOverlayEnum->Release();
            m_pOverlayEnum = NULL;
        }     
        hr = pTimeline->GetOverlaysEnum(&m_pOverlayEnum);
        return hr;
    }
    // Own public methods:
    HRESULT OverlaysDestroy(void)
    {
        if( m_DC )
        {
            DeleteDC( m_DC );
            m_DC = NULL;
        }
        if( m_DibSection )
        {
            DeleteObject( m_DibSection );
            m_DibSection = NULL;
        }
        ZeroMemory(&m_FrameInfo, sizeof(m_FrameInfo));
        if(m_pActualOverlay)
        {
            m_pActualOverlay->Release();
            m_pActualOverlay = NULL;
        }
        return S_OK;
    }

    HRESULT OverlaysInit(VIDEOINFO* pVI)
    {
        HRESULT hr;
        OverlaysDestroy();
        if(!m_pOverlayEnum) return E_UNEXPECTED;
        
        hr = m_pOverlayEnum->Rewind();
        if(SUCCEEDED(hr))
        {
            hr = m_pOverlayEnum->GetNext(&m_pActualOverlay);
        }
        if(S_OK!=hr) return hr;


		HDC hdc = GetDC( NULL );
		m_DC = CreateCompatibleDC( hdc );
		ReleaseDC( NULL, hdc );

		if( !m_DC )
			return E_FAIL;

#if 0
        if(pVI->bmiHeader.biBitCount!=24 && pVI->bmiHeader.biBitCount!=32)
        {
            return E_INVALIDARG;
        }

        BYTE bBmiBuf[sizeof(BITMAPINFO)+ sizeof(RGBQUAD) * 256];
        BITMAPINFO* pbmi = (BITMAPINFO*) bBmiBuf;

        ZeroMemory(bBmiBuf, sizeof(bBmiBuf));

        pbmi->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth    = pVI->bmiHeader.biWidth;
        pbmi->bmiHeader.biHeight   = pVI->bmiHeader.biHeight;
        pbmi->bmiHeader.biPlanes   = 1;
        pbmi->bmiHeader.biBitCount = pVI->bmiHeader.biBitCount;
        pbmi->bmiHeader.biCompression = BI_RGB;

        if(32==pbmi->bmiHeader.biBitCount)
        {
            pbmi->bmiColors[0] = 0x00ff0000;
            pbmi->bmiColors[1] = 0x0000ff00;
            pbmi->bmiColors[2] = 0x000000ff;
        }
		m_DibSection = CreateDIBSection(NULL,
										pbmi,
										DIB_RGB_COLORS,
										&m_pDibBits,
										NULL,
										0);

#else
		m_DibSection = CreateDIBSection(m_DC,
										(BITMAPINFO*) &pVI->bmiHeader,
										DIB_RGB_COLORS,
										&m_pDibBits,
										NULL,
										0);
#endif
		if( !m_DibSection )
		{
			return E_FAIL;
		}

        m_FrameInfo.rect.bottom = pVI->bmiHeader.biHeight;
        m_FrameInfo.rect.right  = pVI->bmiHeader.biWidth;

        return hr;
    }
    virtual HRESULT OverlayDraw(const LONGLONG& llFrameNo, IMediaSample* pMediaSample)
    {
        HRESULT hr = S_OK;
        if(!m_pOverlayEnum) return E_UNEXPECTED;
        if(!pMediaSample) return E_POINTER;        
        if(!m_pActualOverlay) return E_FAIL;

        LONG lBegin(0);
        LONG lEnd(0);
        m_FrameInfo.lFrame = (LONG)llFrameNo;
        while(true)
        {
            hr = m_pActualOverlay->GetBoundaries(&lBegin, &lEnd);
            if(FAILED(hr)) return hr;

            if(m_FrameInfo.lFrame < lBegin)
            {//nothing to do
                return S_FALSE;
            }
            if(m_FrameInfo.lFrame > lEnd)
            {
                m_pActualOverlay->Release();
                m_pActualOverlay = NULL;
                hr = m_pOverlayEnum->GetNext(&m_pActualOverlay);
                if(S_OK!=hr)
                {
                    return hr;
                }
                continue;
            }
            break;
        }
        m_FrameInfo.hdc     = m_DC;
        
        BYTE* pBufOut(NULL);
    	pMediaSample->GetPointer( &pBufOut );
	    if(!pBufOut) return E_OUTOFMEMORY;

        LONG lDataLength = pMediaSample->GetActualDataLength();
        memcpy( m_pDibBits, pBufOut,  lDataLength);
        HGDIOBJ hOldObj = SelectObject(m_DC, m_DibSection);
        hr = m_pActualOverlay->Draw(&m_FrameInfo);
        SelectObject(m_DC, hOldObj);
        if(SUCCEEDED(hr))
        {
            memcpy( pBufOut, m_pDibBits, lDataLength);
        }

        return hr;
    }
};
