#include "StdAfx.h"
#include "Viewer.h"
#include "utils.h"

using namespace EditorApplication::EditorLogical;
//////////////////////////////////////////////////////////////////////////
//							Public methods								//
//////////////////////////////////////////////////////////////////////////
// Default constructor
CViewer::CViewer(	HWND hWnd,	HWND hClip)
:hMainWnd(hWnd)
,hClipWnd(hClip)
,hEditMainWnd(hWnd)
,hEditClipWnd(hClip)
,pGB(NULL)
,pMS(NULL)
,pMC(NULL)
,pWC(NULL)
,pMix(NULL)
,m_pVmrMixerBitmap9(NULL)
,pNotes(NULL)
,g_dwGraphRegister(0)
,MainStreamWidth(0)
,MainStreamHeight(0)
,MainStreamVisibleHeight(0)
,MainStreamVisibleWidth(0)
,MainStreamReduction(0.0f)
,WebStreamWidth(0)
,WebStreamHeight(0)
,IsWebStream(false)
,IsAudioStream(false)
//,IsPlayWebVideo(true)
//,IsPlayAudio(true)
,IsLoaded(false)
,StartFrame(0)
,EndFrame(0)
,PreviewMode(false)
,m_CameraLeftDelta(0)
,m_CameraTopDelta(0)
,m_lDuration(0)
,m_rgbTranspColor(RGB(254, 254, 254))
,m_hdcOveraly(NULL)
,m_hbmOveraly(NULL)
{
	FileName[0] = 0;
	VerifyVMR9();
}

// Virtual destructor
CViewer::~CViewer(void)
{
	FreeFilters();
}

// Open Webinaria Recorder non compression avi
bool CViewer::Open(const wchar_t * NewFileToOpen)
{
	wcscpy_s(FileName, N_ELEMENTS(FileName), NewFileToOpen);

	if (FileExists(FileName))
	{
		HRESULT hr;
		FreeFilters();
		

		if (SUCCEEDED(InitializeVideoAudio()))
        {
		    StartFrame = 0;
		    EndFrame = 0;
		    hr = pMS->SetRate(1.0);
            hr = pMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
            if(SUCCEEDED(hr))
            {
                LONGLONG llDuration(0);
                hr = pMS->GetDuration(&llDuration);
                if(SUCCEEDED(hr))
                {
                    m_lDuration = (long) (llDuration / 10000000);
                }
            }
		    // Set frame time format
		    hr = pMS->SetTimeFormat(&TIME_FORMAT_FRAME);
		    hr = Pause();
		    IsLoaded = true;
		    return SUCCEEDED(hr) ? true : false;
        }
        else
            return false;
	}
	else
	{
		CError::ErrMsg(hMainWnd, L"Editor tried to open the recording, but video file doesn't exist");
		return false;
	}			

	IsLoaded = false;
	return false;
}

// Start playback
HRESULT CViewer::Play()
{
	if (pMC)
	{
		if ((PreviewMode && !IsRunning())||(!PreviewMode))
		{
			return pMC->Run();
		}
		return S_OK;
	}
	else
	{
		return E_ABORT;
	}
}

// Pause playback
HRESULT CViewer::Pause()
{
HRESULT hr = E_ABORT;
	if (pMC)
	{
		hr = pMC->Pause();
	}
	return hr;
}
HRESULT CViewer::PauseToggle()
{
	if (pMC)
	{
		if (IsRunning())
		{
			return pMC->Pause();
		}
		else
		{
			return pMC->Run();
		}
	}
	else
		return E_ABORT;
}

// Stop playback
HRESULT CViewer::Stop()
{
HRESULT hr = E_FAIL;
	if(pMC)
	{
		hr = pMC->Stop();
	}
	return hr;
}

// Set playback to start frame
long long CViewer::GoToStart()
{
	long long lResult = SetPFrame(0);
	return lResult;
}


// Set playback to specified in frame position
long long CViewer::SetPFrame(const long long & TrackPosition)
{
HRESULT hr;
	if (IsLoaded)
	{
		long long Temp;
		pMS->GetDuration(&Temp);
		if ( TrackPosition < 0 )
			StartFrame = 0;
		else if ( TrackPosition > Temp )
			StartFrame = Temp;
		else
			StartFrame = TrackPosition;
		EndFrame = StartFrame;
		LONGLONG llPos = StartFrame;//+2;
		hr = Stop();
		hr = pMS->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning, 0, AM_SEEKING_NoPositioning);
		hr = Pause();
		InvalidateRect(hEditClipWnd, NULL,FALSE);
//		Update();
	}
	return StartFrame;
}

// Get frames count
long long CViewer::GetFramesCount() const
{
long long FramesCount = -1;
	if (pMS)
	{
		pMS->GetDuration(&FramesCount);
	}
	return FramesCount;
}


void CViewer::SetAudio(const bool & flag)
{
//	IsPlayAudio = flag;
}

void CViewer::SetWebcamVideo(const bool & flag)
{
	if (IsLoaded)
	{
		pMix->SetAlpha(1, flag ? 1.0f : 0.0f);
		Update();
	}
}

bool CViewer::Loaded() const
{
	return IsLoaded;
}

// Move render window
void CViewer::MoveVideoWindow(bool NonConstRatio)
{
	HRESULT hr;
	RECT rcDest={0};

	// Track the movement of the container window and resize as needed
	if (pWC)
	{
		GetWindowRect(hClipWnd, &rcDest);
		rcDest.right -= rcDest.left;
		rcDest.bottom -= rcDest.top;
		rcDest.left = 0;
		rcDest.top = 0;
		hr = pWC->SetVideoPosition(NULL, &rcDest);
	}

	if (NonConstRatio)
	{
		RECT rc;
		GetWindowRect(hClipWnd, &rc);
		int ClipWidth = rc.right - rc.left;
		int ClipHeight = rc.bottom - rc.top;

		VMR9NormalizedRect vmr9nrc;
		if (ClipHeight * MainStreamWidth >= ClipWidth * MainStreamHeight)
		{
			MainStreamVisibleHeight = (int)((float)MainStreamHeight * ((float)ClipWidth/(float)MainStreamWidth));
			MainStreamVisibleWidth = ClipWidth;
			MainStreamReduction = (float)ClipWidth/(float)MainStreamWidth;
			vmr9nrc.left = 0.0f;
			vmr9nrc.right = 1.0f;
			vmr9nrc.bottom = 0.5f + (float)MainStreamHeight * (float)ClipWidth / 2.0f / (float)ClipHeight / (float)MainStreamWidth;
			vmr9nrc.top = 1.0f - vmr9nrc.bottom;
		}
		else
		{
			MainStreamVisibleHeight = ClipHeight;
			MainStreamVisibleWidth = (int)((float)MainStreamWidth * ((float)ClipHeight/(float)MainStreamHeight));
			MainStreamReduction = ClipHeight/(float)MainStreamHeight;
			vmr9nrc.right = 0.5f + (float)MainStreamWidth * (float)ClipHeight / 2.0f / (float)ClipWidth / (float)MainStreamHeight;
			vmr9nrc.left = 1.0f - vmr9nrc.right;
			vmr9nrc.top = 0.0f;
			vmr9nrc.bottom = 1.0f;
		}

		hr = pMix->SetOutputRect(0,&vmr9nrc);

		if (IsWebStream)
		{
			if ( ClipHeight * MainStreamWidth > ClipWidth * MainStreamHeight )
			{
				vmr9nrc.bottom = (float)WebStreamHeight * (float)ClipWidth / (float)ClipHeight / (float)MainStreamWidth + vmr9nrc.top;
				vmr9nrc.left = -((float)WebStreamHeight * (float)MainStreamWidth / (float)MainStreamHeight - (float)WebStreamWidth)/2.0f/(float)ClipWidth;
				vmr9nrc.right = (float)WebStreamHeight / (float)MainStreamHeight + vmr9nrc.left;
			}
			else
			{
				vmr9nrc.right = (float)WebStreamWidth * (float)ClipHeight / (float)ClipWidth / (float)MainStreamHeight + vmr9nrc.left;
				vmr9nrc.top = -((float)WebStreamWidth * (float)MainStreamHeight / (float)MainStreamWidth - (float)WebStreamHeight)/2.0f/(float)ClipHeight;
				//-(1.0f - (float)WebStreamHeight * (float)ClipWidth / (float)WebStreamWidth / (float)ClipHeight) / 2 * 
				//(float)WebStreamHeight / (float)MainStreamHeight;
				vmr9nrc.bottom = (float)WebStreamWidth / (float)MainStreamWidth + vmr9nrc.top;
			}

			hr = pMix->SetOutputRect(1, &vmr9nrc);
			// Temporary
			hr = pMix->SetAlpha(1, 1);

			HorCoeff = 1.0f - 2.0f*vmr9nrc.left;
			VerCoeff = 1.0f - 2.0f*vmr9nrc.top;
		}
	}
	else
	{
//!!!		Update();
		//Stop();
		//Play();
	}
}

// Move web stream by percent
void CViewer::MoveWebStream(const short & hPerShift, const short & vPerShift)
{
	if (IsWebStream && pMix != NULL)
	{
		VMR9NormalizedRect main, camera;
		pMix->GetOutputRect(0, &main);
		pMix->GetOutputRect(1, &camera);

		float mainWidth = main.right - main.left, mainHeight = main.bottom - main.top;
		float cameraWidth = camera.right - camera.left, cameraHeight = camera.bottom - camera.top;

		camera.left = m_CameraLeftDelta + ((float)hPerShift * mainWidth) / 100.0;
		camera.right = camera.left + cameraWidth;
		camera.top = m_CameraTopDelta + ((float)(100.0 - vPerShift) * mainHeight) / 100.0;
		camera.bottom = camera.top + cameraHeight;

		pMix->SetOutputRect(1, &camera);

		Update();
	}
}

// Repaint render window
HRESULT CViewer::RepaintVideo(HDC hdc)
{
	HRESULT hr = E_FAIL;
	if (pWC)
	{
		hr = pWC->RepaintVideo(hClipWnd, hdc);
	}
	return hr;
}

//// Get media event handler
//IMediaEventEx * CViewer::GetME()
//{
//	return pME;
//}

wchar_t * CViewer::GetFileName() const
{
	return (wchar_t *) FileName;
}

//////////////////////////////////////////////////////////////////////////
//							Protected methods							//
//////////////////////////////////////////////////////////////////////////

// VMR9 verifier
bool CViewer::VerifyVMR9(void)
{
	HRESULT hres;
	// Verify that the VMR exists on this system
	CComPtr<IBaseFilter> cpBF;
	hres = CreateFilter(CLSID_VideoMixingRenderer9, &cpBF);

	if(SUCCEEDED(hres))
	{
		return true;
	}
	else
	{
		CError::ErrMsg(	hMainWnd,
						TEXT("This application requires the Video Mixing Renderer 9, which is present\r\n")
						TEXT("only on DirectX 9 systems with hardware video acceleration enabled.\r\n\r\n")
						TEXT("The Video Mixing Renderer (VMR) is not enabled when viewing a \r\n")
						TEXT("remote Windows XP machine through a Remote Desktop session.\r\n")
						TEXT("You can run VMR-enabled applications only on your local machine.\r\n\r\n")
						TEXT("To verify that hardware acceleration is enabled on a Windows XP\r\n")
						TEXT("system, follow these steps:\r\n")
						TEXT("-----------------------------------------------------------------------\r\n")
						TEXT(" - Open 'Display Properties' in the Control Panel\r\n")
						TEXT(" - Click the 'Settings' tab\r\n")
						TEXT(" - Click on the 'Advanced' button at the bottom of the page\r\n")
						TEXT(" - Click on the 'Troubleshooting' tab in the window that appears\r\n")
						TEXT(" - Verify that the 'Hardware Acceleration' slider is at the rightmost position\r\n"));
		return false;
	}
}

// Initialize windowless video media renderer
HRESULT CViewer::InitializeWindowlessVMR(IBaseFilter **ppVmr9)
{
	if (!ppVmr9)
		return E_POINTER;
	*ppVmr9 = NULL;

	CComPtr<IBaseFilter> cpVmr9;
	// Create the VMR and add it to the filter graph.
	HRESULT hr = CreateAndInsertFilter(pGB, CLSID_VideoMixingRenderer9, &cpVmr9, L"VMR9");
	if (SUCCEEDED(hr)) 
	{
		// Set the rendering mode and number of streams
		CComPtr <IVMRFilterConfig9> pConfig;
		hr = cpVmr9->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot get IVMRFilterConfig9"), hr);
			return hr;
		}

		hr = pConfig->SetRenderingMode(VMR9Mode_Windowless);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Set rendering mode"), hr);
			return hr;
		}

		// Set the bounding window and border for the video
		hr = cpVmr9->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWC);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Initialize windowless control"), hr);
			return hr;
		}

		hr = pWC->SetVideoClippingWindow(hClipWnd);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Initialize clipping window"), hr);
			return hr;
		}

		hr = pWC->SetBorderColor(RGB(0,0,0));    // Black border
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Set border color"), hr);
			return hr;
		}
		pWC->SetAspectRatioMode( VMR_ARMODE_NONE);

		// Get the mixer control interface for later manipulation of video 
		// stream output rectangles and alpha values
		hr = cpVmr9->QueryInterface(IID_IVMRMixerControl9, (void**)&pMix);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Initialize mixer control"), hr);
			return hr;
		}
        hr = cpVmr9->QueryInterface(IID_IVMRMixerBitmap9, (void**) &m_pVmrMixerBitmap9);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Initialize IVMRMixerBitmap9"), hr);
			return hr;
		}


		*ppVmr9 = cpVmr9;
		(*ppVmr9)->AddRef();
	}
	return hr;
}

// Initialize graph and display video and audio streams
HRESULT CViewer::InitializeVideoAudio(void)
{
	IsWebStream = false;
	IsAudioStream = false;
	// Create a DirectShow GraphBuilder object
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGB);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Filter graph built failed"), hr);
		return hr;
	}

	if(SUCCEEDED(hr))
	{
		hr = ConfigureMultiStreamVMR9();
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Configure video streams to VMR9"), hr);
			return hr;
		}

		// Get DirectShow interfaces
		hr = pGB->QueryInterface(IID_IMediaControl, (void **)&pMC);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Create filter graph control"), hr);
			return hr;
		}

		/*hr = pGB->QueryInterface(IID_IMediaEventEx, (void **)&pME);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Create filter graph event handler"), hr);
			return hr;
		}*/
		
		hr = pGB->QueryInterface(IID_IMediaSeeking, (void **)&pMS);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Create filter graph frame seeking"), hr);
			return hr;
		}

		// Have the graph signal event via window callbacks
		/*hr = pME->SetNotifyWindow((OAHWND)hMainWnd, WM_FGNOTIFY, 0);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed graph notify message"), hr);
			return hr;
		}*/

		g_dwGraphRegister = 0;
		AddGraphToRot(pGB, &g_dwGraphRegister);

/*
		// Get AVI Splitter filter for initialize sizes of input streams
		IBaseFilter *pAS = NULL;

		HRESULT hr = pGB->FindFilterByName(L"AVI Splitter",&pAS);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot retrieve AVI Splitter Filter"), hr);
			return hr;
		}

		IEnumPins *pEnum = 0;
		IPin *pPin = 0;

		// Get a pin enumerator
		hr = pAS->EnumPins(&pEnum);
		if (FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot retrieve pin enumeration"), hr);
			return hr;
		}

		// Go to the beginning of the enumeration
		pEnum->Reset();
		int nVideoStreams = 0;
		// Look for the first and second output pins
		while (pEnum->Next(1, &pPin, NULL) == S_OK)
		{
			PIN_DIRECTION ThisPinDir;

			hr = pPin->QueryDirection(&ThisPinDir);
			if (ThisPinDir == PINDIR_OUTPUT)
			{
				IEnumMediaTypes * pEMT = NULL;
				hr = pPin->EnumMediaTypes(&pEMT);
				hr = pEMT->Reset();
				AM_MEDIA_TYPE * pamt = NULL;
				ULONG fetched;				
				if ((pEMT->Next(1,&pamt,&fetched)) == S_OK)
				{
					if (pamt->cbFormat >= sizeof(VIDEOINFOHEADER))
					{
						VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(pamt->pbFormat);
						if (0==nVideoStreams)
						{
							MainStreamWidth = pVih->bmiHeader.biWidth;
							MainStreamHeight = pVih->bmiHeader.biHeight;							
						}
						else
						{
							WebStreamWidth = pVih->bmiHeader.biWidth;
							WebStreamHeight = pVih->bmiHeader.biHeight;
							IsWebStream = true;
						}
						nVideoStreams++;
					}
					DeleteMediaType(pamt);
				}
				pEMT->Release();
			}
			pPin->Release();
		}
		// Release the enumerator
		pEnum->Release();

		// Release the AVI Splitter
		pAS->Release();
*/
		VMR9NormalizedRect rc;
		if (3 * MainStreamWidth >= 4 * MainStreamHeight)
		{
			MainStreamVisibleHeight = (int)((float)MainStreamHeight * (320.0f/(float)MainStreamWidth));
			MainStreamVisibleWidth = 320;
			MainStreamReduction = 320.0f/(float)MainStreamWidth;
			rc.left = 0.0f;
			rc.right = 1.0f;
			rc.bottom = 0.5f + (float)MainStreamHeight * 2.0f / 3.0f / (float)MainStreamWidth;
			rc.top = 1.0f - rc.bottom;
		}
		else
		{
			MainStreamVisibleHeight = 240;
			MainStreamVisibleWidth = (int)((float)MainStreamWidth * (240.0f/(float)MainStreamHeight));
			MainStreamReduction = 240.0f/(float)MainStreamHeight;
			rc.right = 0.5f + (float)MainStreamWidth * 0.375f / (float)MainStreamHeight;
			rc.left = 1.0f - rc.right;
			rc.top = 0.0f;
			rc.bottom = 1.0f;
		}
		
		hr = pMix->SetOutputRect(0,&rc);

		if (IsWebStream)
		{
			float hx = (float)MainStreamWidth * (float)WebStreamHeight / 
					  ((float)WebStreamWidth * (float)MainStreamHeight);
			float wx = (float)MainStreamHeight * (float)WebStreamWidth / 
					  ((float)WebStreamHeight * (float)MainStreamWidth);
			if ( 3*MainStreamWidth >= 4*MainStreamHeight)
			{
				rc.bottom = (float)WebStreamHeight * 4.0f / 3.0f / (float)MainStreamWidth + rc.top;
				rc.left = (1 - 1 / wx) * 0.3f * wx;
				rc.right = (float)WebStreamHeight / (float)MainStreamHeight + rc.left;
			}
			else
			{
				rc.right = (float)WebStreamWidth * 0.75f / (float)MainStreamHeight + rc.left;
				rc.top = (1 - 1 / hx)* 0.375f * hx; 
				rc.bottom = (float)WebStreamWidth / (float)MainStreamWidth + rc.top;
			}

			hr = pMix->SetOutputRect(1, &rc);
			// Temporary
			hr = pMix->SetAlpha(1, 1);

			HorCoeff = 1.0f - 2.0f*rc.left;
			VerCoeff = 1.0f - 2.0f*rc.top;

			m_CameraLeftDelta = rc.left;
			m_CameraTopDelta = rc.top;
		}
		
		// Set initial movie position
		MoveVideoWindow();

		//RepaintVideo();
	}

	return hr;
}


// Configure all video streams from source file
HRESULT CViewer::ConfigureMultiStreamVMR9()
{
	HRESULT hr=S_OK;
	CComPtr <IBaseFilter> pVmr;
	IPin * pOutputPin;
	IPin * pInputPin;
	CComPtr <IBaseFilter> pSource;
//	CComPtr <IBaseFilter> pAudioRenderer;
	IBaseFilter * pNotesAdder;
	CComPtr <IFilterGraph2> pFG;
	CComPtr <IBaseFilter> pAVISplitter;

	// Create the Video Mixing Renderer and add it to the graph

	InitializeWindowlessVMR(&pVmr);


	// Add the splitter filter to the graph
	hr = CreateAndInsertFilter(pGB, CLSID_AviSplitter, &pAVISplitter, L"AVI Splitter");
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed creating splitter filter"), hr);
		return hr;
	}

            
	// Render the files programmatically to use the VMR9 as renderer
	// Add the source filter to the graph

	hr = pGB->AddSourceFilter(FileName, L"SOURCE", &pSource);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed adding source filter"), hr);
		return hr;
	}

	// Get the interface for the first output pin
	hr = GetUnconnectedPin(pSource, PINDIR_OUTPUT, &pOutputPin);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed unconnected pin search"), hr);
		return hr;
	}

	// Get an IFilterGraph2 interface to assist in building the
	// multifile graph with the non-default VMR9 renderer
	hr = pGB->QueryInterface(IID_IFilterGraph2, (void **)&pFG);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed filter graph"), hr);
		return hr;
	}

	hr = CreateAndInsertFilter(pGB, CLSID_CNotesAdder, &pNotesAdder, L"Note Adder");
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed initialize of Notes Adder Filter"), hr);
		return hr;
	}
	hr = pNotesAdder->QueryInterface(IID_INotesAdder, (void **)&pNotes);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed using of Notes Adder Filter"), hr);
		return hr;
	}

	hr = GetUnconnectedPin(pNotesAdder, PINDIR_INPUT, &pInputPin);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed unconnected pin search"), hr);
		return hr;
	}

	hr = pGB->Connect(pOutputPin, pInputPin);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed connect"), hr);
		return hr;
	}

	SAFE_RELEASE(pOutputPin);
	SAFE_RELEASE(pInputPin);

	hr = GetUnconnectedPin(pNotesAdder, PINDIR_OUTPUT, &pOutputPin);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed unconnected pin search"), hr);
		return hr;
	}
	
	// Render the output pin, using the VMR9 as the specified renderer.  This is 
	// necessary in case the GraphBuilder needs to insert a Color Space convertor,
	// or if multiple filters insist on using multiple allocators.
	// The audio renderer will also be used, if the media file contains audio.
	hr = pFG->RenderEx(pOutputPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed initialize of first video pin connection"), hr);
		return hr;
	}
	hr = GetFrameSize(pOutputPin, &MainStreamWidth, &MainStreamHeight);

	SAFE_RELEASE(pOutputPin);
	SAFE_RELEASE(pNotesAdder);

/*
	hr = pGB->FindFilterByName(L"AVI Splitter", &pAVISplitter);
	if (FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed get AVI Splitter"), hr);
		return hr;
	}
*/
	IsWebStream = false;
	CComPtr<ICaptureGraphBuilder2> cpCgb2;
	hr =   CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&cpCgb2);
	if(SUCCEEDED(hr))
	{
		hr = cpCgb2->SetFiltergraph(pGB);
		if(SUCCEEDED(hr))
		{
			IPin* pEndPin = NULL;
			hr = cpCgb2->FindPin(pAVISplitter, PINDIR_OUTPUT, NULL, &MEDIATYPE_Video, TRUE, 0, &pEndPin);
			if(S_OK==hr)
			{
				CComPtr<IBaseFilter> cpTimeRemover;
				hr = CreateFilter(CLSID_CTimeRemover, &cpTimeRemover);
				if(SUCCEEDED(hr))
				{	
					hr = JoinFilterToChain(pGB, cpTimeRemover, L"TimeStamp remover", &pEndPin); 
					if(SUCCEEDED(hr))
					{
						pGB->Render(pEndPin);
						hr = GetFrameSize(pEndPin, &WebStreamWidth, &WebStreamHeight);
						if(SUCCEEDED(hr))
						{
							IsWebStream = true;
						}
					}
				}
				SAFE_RELEASE(pEndPin);
			}
			hr = cpCgb2->FindPin(pAVISplitter, PINDIR_OUTPUT, NULL, &MEDIATYPE_Audio, FALSE, 0, &pEndPin);
			if(S_OK==hr)
			{
				IsAudioStream = true;
				pEndPin->Release();
			}
			else
			{
				IsAudioStream = false;
				hr = S_OK;									//	Otherwise it will fail the whole thing, which is not correct.
			}
		}
	}
    CComPtr <IVMRFilterConfig9> cpVMRFilterConfig9;
    hr = pVmr->QueryInterface(IID_IVMRFilterConfig9, (void**)&cpVMRFilterConfig9);

    cpVMRFilterConfig9->SetNumberOfStreams(IsWebStream ? 3 : 2);

    hr = OverlayCreate();


/*
	while (SUCCEEDED(GetUnconnectedPin(pAVISplitter, PINDIR_OUTPUT, &pOutputPin)))
	{
		pFG->RenderEx(pOutputPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);

		SAFE_RELEASE(pOutputPin);
	}

	IPin * pAudio = NULL;

	IsAudioStream = false;
	GetUnconnectedPin(pAudioRenderer, PINDIR_INPUT, &pAudio);
	if (pAudio == NULL)
	{
		IsAudioStream = true;
	}
	else
	{
		pAudio->Release();
		pAudio = NULL;
	}
		pFG->RemoveFilter(pAudioRenderer);//at any case
//	}
*/
	return hr;
}

// All done with the filters and the graph builder
void CViewer::FreeFilters()
{
	IsLoaded = false;
	// Stop the position timer
	//StopTimer();

	// Stop playback
	Stop();

	DemolishGraphFilters(pGB);

	RemoveGraphFromRot(g_dwGraphRegister);

	// Disable event callbacks
	/*if (pME)
		pME->SetNotifyWindow((OAHWND)NULL, 0, 0);*/
	

	// Release all active interfaces
	SAFE_RELEASE(pMC);
	SAFE_RELEASE(pMS);
	//SAFE_RELEASE(pME);
	SAFE_RELEASE(pNotes);
	SAFE_RELEASE(pWC);      // IVMRWindowlessControl9
	SAFE_RELEASE(pMix);     // IVMRMixerControl9
    SAFE_RELEASE(m_pVmrMixerBitmap9);//IVMRMixerBitmap9
	SAFE_RELEASE(pGB);
    OverlayDestroy();
}


void CViewer::UpdateNotes(const STextInterval * Texts, const SArrowInterval * Arrows)
{
	pNotes->SetIntervals((void *)Texts, (void *)Arrows);
}

unsigned long CViewer::GetMainStreamLeftShift()
{
	RECT rc;
	GetWindowRect(hClipWnd, &rc);
	return ((rc.right - rc.left - MainStreamVisibleWidth)>>1);
}

unsigned long CViewer::GetMainStreamTopShift()
{
	RECT rc;
	GetWindowRect(hClipWnd, &rc);
	return ((rc.bottom - rc.top - MainStreamVisibleHeight)>>1);
}

/*unsigned long CViewer::GetMainStreamWidth()
{
	return MainStreamWidth;
}

unsigned long CViewer::GetMainStreamHeight()
{
	return MainStreamHeight;
}*/


float CViewer::GetReduction()
{
	return MainStreamReduction;
}

void CViewer::SetWindows(const HWND & hMain, const HWND & hClip)
{
	hMainWnd = hMain;
	hClipWnd = hClip;
	
	RECT rc;
	GetWindowRect(hClipWnd, &rc);
	if (MainStreamWidth >= 427)
		MoveWindow(hMainWnd, rc.left, rc.top, MainStreamWidth + 2, MainStreamHeight + 157, TRUE);
	else
		MoveWindow(hMainWnd, rc.left, rc.top, 429, MainStreamHeight + 157, TRUE);

	HRESULT hr = pWC->SetVideoClippingWindow(hClipWnd);
	if (FAILED(hr))
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Initialize clipping window"), hr);
	
	// Set initial movie position
	MoveVideoWindow(true);
}

void CViewer::SetPreviewMode(const bool & Mode)
{
	PreviewMode = Mode;
	if (!Mode)
	{
		hMainWnd = hEditMainWnd;
		hClipWnd = hEditClipWnd;
		HRESULT hr = pWC->SetVideoClippingWindow(hClipWnd);
		if (FAILED(hr))
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Initialize clipping window"), hr);
		MoveVideoWindow();
		Update();
	}
}

bool CViewer::IsRunning(void)				// Clip is running
{
bool bResult = false;
	if(pMC)
	{
		OAFilterState ofs = State_Running;
		pMC->GetState(5000, &ofs);
		bResult = (State_Running==ofs);
	}
	return bResult;
}
HRESULT CViewer::Update(void)
{
	if(pMC)
	{//update renderer surface:
		pMC->Stop();
		pMC->Pause();
	}
	//Update window
	InvalidateRect(hEditClipWnd, NULL,FALSE);
	return S_OK;
}
bool CViewer::WebcamPresent(void) const
{
	return IsWebStream;
}
bool CViewer::AudioPresent(void) const
{
	return IsAudioStream;
}
long CViewer::GetDuration(void) const
{
    return m_lDuration;
}
void CViewer::OverlayClear(void)
{
    if(!m_hbmOveraly || !m_hdcOveraly)
    {
        return ;
    }
    HBRUSH hbrBkgnd = CreateSolidBrush(m_rgbTranspColor); 
    RECT rcFrame;
    SetRect(&rcFrame, 0, 0, MainStreamWidth, MainStreamHeight);
    int result = ::FillRect(m_hdcOveraly, &rcFrame, hbrBkgnd);
    DeleteObject(hbrBkgnd); 
}
void CViewer::OverlayCommit(void)
{
const float X_EDGE_BUFFER=0.05f; // Pixel buffer between bitmap and window edge
                                 // (represented in composition space [0 - 1.0f])
const float Y_EDGE_BUFFER=0.05f;
const float TRANSPARENCY_VALUE=1.0f;

    if(!m_pVmrMixerBitmap9 || !m_hdcOveraly) return;

// Configure the VMR's bitmap structure
    VMR9AlphaBitmap bmpInfo;
    ZeroMemory(&bmpInfo, sizeof(bmpInfo) );
    bmpInfo.dwFlags = VMR9AlphaBitmap_hDC;
    bmpInfo.hdc = m_hdcOveraly;  // DC which has selected our bitmap

    // Set initial bitmap settings
    RECT rcBitmap;
    SetRect(&rcBitmap, 0, 0, MainStreamWidth, MainStreamHeight);

    // Display the bitmap in the bottom right corner.
    // rSrc specifies the source rectangle in the GDI device context 
    // rDest specifies the destination rectangle in composition space (0.0f to 1.0f)
    bmpInfo.rDest.left   = 0.0f;
    bmpInfo.rDest.right  = 1.0f;
    bmpInfo.rDest.top    = 0.0f;
    bmpInfo.rDest.bottom = 1.0f;
    bmpInfo.rSrc = rcBitmap;

    // Transparency value 1.0 is opaque, 0.0 is transparent.
    bmpInfo.fAlpha = TRANSPARENCY_VALUE;

    bmpInfo.clrSrcKey = m_rgbTranspColor;  
    bmpInfo.dwFlags |= VMRBITMAP_SRCCOLORKEY;

    // Give the bitmap to the VMR for display
    m_pVmrMixerBitmap9->SetAlphaBitmap(&bmpInfo);
    Update();
}
HRESULT CViewer::OverlayCreate(void)
{
HRESULT hr = S_OK;
    OverlayDestroy();
    

    HDC hdc = GetDC(hClipWnd);
    m_hdcOveraly = CreateCompatibleDC(hdc);

    m_hbmOveraly = CreateCompatibleBitmap(hdc, MainStreamWidth, MainStreamHeight);

    ReleaseDC(hClipWnd, hdc);

    SetBkColor(m_hdcOveraly, m_rgbTranspColor); 

    HBITMAP hbmOld = (HBITMAP)SelectObject(m_hdcOveraly, m_hbmOveraly);
    OverlayClear();
    return hr;
}
void CViewer::OverlayDestroy(void)
{
    if(m_hdcOveraly) DeleteDC(m_hdcOveraly);
    if(m_hbmOveraly) DeleteObject(m_hbmOveraly);
    m_hbmOveraly = NULL;
    m_hdcOveraly = NULL;
}