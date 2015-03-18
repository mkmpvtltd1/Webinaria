
#pragma once

extern const AMOVIESETUP_FILTER sudWebinLogo;
#pragma pack (push,1)
struct SRGB
{
	BYTE b;
	BYTE g;
	BYTE r;
};
#pragma pack (pop)
class CWebinariaLogoFilter : public CTransInPlaceFilter, ILogoFilter
{
private:

	// Constructor
							CWebinariaLogoFilter(LPUNKNOWN punk, HRESULT *phr);
	// Destructor
							~CWebinariaLogoFilter();

			int				m_nWidth;
			int				m_nHeight;
			HBITMAP			m_hBmpLogo;
			HBITMAP			m_hDibSectionLogo;
			HDC				m_DC;
			PBYTE			m_pBytesLogo;
			LONG			m_lBytesLogoSize;
			int				m_nLogoWidth;
			int				m_nLogoHeight;
			int				m_nLogoX;
			int				m_nLogoY;
			int				m_nFrameLineLength;
			int				m_nFrameLogoRawOffset; //in bytes

			TCHAR *m_smallLogoPath, *m_largeLogoPath;

			void DeleteLogoPaths();

protected:
	void __fastcall			OverlayLogo(PBYTE pbFrame, PBYTE pbLogo);
public:

	static	CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

	STDMETHOD(SetLogoPath)(BSTR smallLogoPath, BSTR largeLogoPath);

    DECLARE_IUNKNOWN;

			HRESULT			Transform(IMediaSample *pSample);
			HRESULT			CheckInputType(const CMediaType *mtIn);
			HRESULT			SetMediaType( PIN_DIRECTION pindir, const CMediaType *pMediaType);
			STDMETHODIMP	Run(REFERENCE_TIME tStart);
			STDMETHODIMP	Pause();
			void			FreeGdiObjects(void);
			STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

};
