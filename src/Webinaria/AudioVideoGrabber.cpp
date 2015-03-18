#include "StdAfx.h"
#include "AudioVideoGrabber.h"

#include "..\RmBkFilter\iRGBFilters.h"
#include "..\RmBkFilter\RGBFilters_i.c"
#include "Folders.h"
#include "Branding.h"
#include "utils.h"

using namespace WebinariaApplication::WebinariaLogical;
//////////////////////////////////////////////////////////////////////////
//								Public methods							//
//////////////////////////////////////////////////////////////////////////
// Default constructor
CAudioVideoGrabber::CAudioVideoGrabber(HWND hWnd, WebinariaApplication::CDeviceEnumerator * ExistEnum, const bool & FreeVersion)
:CGrabber(hWnd)
,CAudioGrabber(hWnd)
,CWDMGrabber(hWnd)
,CScreenGrabber(hWnd)
,pBuilder(NULL)
,pCrossbar(NULL)
//,pRemoveBackFilter(NULL)
,pFirstCompressor(NULL)
,pSecondCompressor(NULL)
//,pAudioCompressor(NULL)
,pLogoFilter(NULL)
,pFg(NULL)
,DevEnum(ExistEnum)
,CapWebcam(false)
,CapAudio(false)
//,TBack(true)
,Logo(FreeVersion)
,CapAudioIsRelevant(true)
,CCAvail(false)
,PreviewFaked(false)
#ifdef DEBUG
,NeedPreview(false/*true*/)
#else
,NeedPreview(false)
#endif
,UseFrameRate(true)
,CaptureGraphBuilt(false)
,MasterStreamNum(1)	// Audio Stream is master
//,NumberOfVideoInputs(0)
,DroppedBase(0)
,NotBase(0)
,CapStartTime(0)
,CapStopTime(0)
,FrameRate(15.0)
{
	TearDownGraph();

	InitCaptureFilters();

	MakeOptionsAndDialogs();
	
}

// Virtual destructor
CAudioVideoGrabber::~CAudioVideoGrabber(void)
{
	FreeCaptureFilters();
	DevEnum = NULL;
}

// Function for start grabbing
bool CAudioVideoGrabber::Start()
{
HRESULT hr;

	// Or we'll crash
	if(pBuilder == NULL)
		return false;

	// Prepare to run the graph
	CComPtr <IMediaControl> cpMC;
	hr = pFg->QueryInterface(IID_IMediaControl, (void **)&cpMC);
	if(FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot get IMediaControl"), hr);
		return false;
	}
	hr = cpMC->Run();
	if(FAILED(hr))
	{
		// Stop parts that started
		cpMC->Stop();
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot start graph"), hr);
		return false;
	}

	// When did we start capture?
	CapStartTime = timeGetTime();

	SetTimer(hMainWnd, 1, 33, NULL);

	return true;
}

// Fuction for stop grabbing
bool CAudioVideoGrabber::Stop()
{
    if (pFg)
    {
	    // Stop the graph
	    IMediaControl *pMC = NULL;
	    HRESULT hr = pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	    if(SUCCEEDED(hr) && pMC)
	    {
		    hr = pMC->Stop();
		    pMC->Release();
	    }
	    if(FAILED(hr))
	    {
		    CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot stop graph"), hr);
		    return false;
	    }
    }

	// When the graph was stopped
	CapStopTime = timeGetTime();

	// No more updates
	KillTimer(hMainWnd, 1);

	// Temp
	// Reload avi to flv
	if (File)
	{
		// Run Webinaria Editor
		ShellExecute(NULL, _T("open"), _T("Editor.exe"), File, Folders::My(), SW_NORMAL);
		//DeleteFileW(File);
		delete[] File;
		File = NULL;
	}

    return true;
}

// Fuction for pause grabbing
bool CAudioVideoGrabber::Pause()
{
	// Stop the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if(SUCCEEDED(hr))
	{
		hr = pMC->Stop();
		pMC->Release();
	}
	if(FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot pause graph"), hr);
		return false;
	}

	KillTimer(hMainWnd, 1);
	
	return true;
}

// Fuction for resume grabbing
bool CAudioVideoGrabber::Resume()
{
	// Stop the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if(SUCCEEDED(hr))
	{
		hr = pMC->Run();
		pMC->Release();
	}
	if(FAILED(hr))
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot pause graph"), hr);
		return false;
	}

	SetTimer(hMainWnd, 1, 33, NULL);
	
	return true;
}
// Set capture possibility of audio narration
void CAudioVideoGrabber::SetAudioCapture(const bool & Capture)
{
	CapAudio = Capture;
}

// Set capture possibility of webcam video
void CAudioVideoGrabber::SetWebcamCapture(const bool & Capture)
{
	CapWebcam = Capture;
}

// Set on/off background removing
/*void CAudioVideoGrabber::SetRemoveBackground(const bool & Back)
{
	TBack = Back;
}*/

//////////////////////////////////////////////////////////////////////////
//							Protected methods							//
//////////////////////////////////////////////////////////////////////////
// Ñreate the capture filters of the graph.  We need to keep them loaded from
// the beginning, so we can set parameters on them and have them remembered
bool CAudioVideoGrabber::InitCaptureFilters()
{
	HRESULT hr = S_OK;

	//pBuilder = new DXShowFilters::CCaptureGraphBuilder();
	
	CoCreateInstance(	CLSID_CaptureGraphBuilder2,
						NULL, 
						CLSCTX_INPROC, 
						IID_ICaptureGraphBuilder2,
						(void **)&pBuilder);


	if( pBuilder == NULL )
	{
		CError::ErrMsg(hMainWnd, TEXT("Graph builder creating error\0"));
		CError::ErrMsg(hMainWnd, TEXT("Cannot instantiate filtergraph"));
		FreeCaptureFilters();
		return false;
	}

	// First, we need a UScreenCapture filter, and some interfaces
	if(DevEnum->IsScreenMoniker())
	{
		DevEnum->BindScreenToObject((void**)&pSCap);
	}

	if(pSCap == NULL)
	{
		FreeCaptureFilters();
		return false;
	}

	// Also we need a Video Capture filter, and some interfaces
	pVCap = NULL;

	if(DevEnum->IsVideoMoniker())
	{
		DevEnum->BindVideoToObject((void**)&pVCap);
	}

	if (CapWebcam)
	{
		if(pVCap == NULL)
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot create video capture filter"), hr);
			//FreeCaptureFilters();
			//return false;
		}
	}

	// Make a filtergraph, give it to the graph builder and put the video captures filters in the graph

	if(!MakeGraph())
	{
		CError::ErrMsg(hMainWnd, TEXT("Cannot instantiate filtergraph"));
		FreeCaptureFilters();
		return false;
	}

	hr = pBuilder->SetFiltergraph(pFg);
	if(hr != NOERROR)
	{
		CError::ErrMsg(hMainWnd, TEXT("Cannot give graph to builder"));
		FreeCaptureFilters();
		return false;
	}

	// Add the screen capture filter to the graph with its friendly name
	hr = pFg->AddFilter(pSCap, L"UScreenCapture");
	if(hr != NOERROR)
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot add screencap filter to filtergraph"), hr);
		FreeCaptureFilters();
		return false;
	}
	
	// Adding compressor filter
		
		// Video capture category
		WCHAR szMon[] = L"@device:cm:{33D9A760-90C8-11D0-BD43-00A0C911CE86}\\msvc";
		IBindCtx *pBindCtx;
		hr = CreateBindCtx(0, &pBindCtx);

		ULONG chEaten = 0;
		IMoniker *pMoniker = 0;
		hr = MkParseDisplayName(pBindCtx, szMon, &chEaten, &pMoniker);
		pBindCtx->Release();
		if (SUCCEEDED(hr))
		{
			pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pFirstCompressor);
			hr = pFg->AddFilter(pFirstCompressor, L"MSVC1_1");
			if(hr != S_OK)
			{
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot add video compressor filter"), hr);
			}
			pMoniker->Release();
		}
		else
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Does not exist Microsoft Video 1 Codec"), hr);
		}
	
		// End adding compressor filter

	if (CapWebcam)
	{
		// Video device friendly name
		wchar_t * VDN = DevEnum->GetVideoDeviceName();
		// Add the video capture filter to the graph with its friendly name
		hr = pFg->AddFilter(pVCap, VDN);
		delete[] VDN;
		if(hr != NOERROR)
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot add vidcap to filtergraph"), hr);
			//FreeCaptureFilters();
			//return false;
			CapWebcam = false;
		}
/*
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,  &MEDIATYPE_Interleaved, pVCap,
													IID_IAMVideoCompression, (void **)&pVC);
		if(hr != S_OK)
			hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pVCap, 
												IID_IAMVideoCompression, (void **)&pVC);
*/
		// What if this interface isn't supported?
		// We use this interface to set the frame rate and get the capture size
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pVCap, 
														IID_IAMStreamConfig, (void **)&pVSC);

		if(hr != NOERROR)
		{
			hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pVCap,
													IID_IAMStreamConfig, (void **)&pVSC);
			if(hr != NOERROR)
				// This means we can't set frame rate (non-DV only)
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot find VCapture:IAMStreamConfig"), hr);
		}
		
		// Adding compressor filter
		
		// Video capture category
		WCHAR szMon[] = L"@device:cm:{33D9A760-90C8-11D0-BD43-00A0C911CE86}\\msvc";
		IBindCtx *pBindCtx;
		hr = CreateBindCtx(0, &pBindCtx);

		ULONG chEaten = 0;
		IMoniker *pMoniker = 0;
		hr = MkParseDisplayName(pBindCtx, szMon, &chEaten, &pMoniker);
		pBindCtx->Release();
		if (SUCCEEDED(hr))
		{
			pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pSecondCompressor);
			hr = pFg->AddFilter(pSecondCompressor, L"MSVC1_2");
			if(hr != S_OK)
			{
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot add video compressor filter"), hr);
			}

			pMoniker->Release();
		}
		else
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Does not exist Microsoft Video 1 Codec"), hr);
		}
	
		// End adding compressor filter

		/*if (TBack)
		{
			hr = CoCreateInstance(	CLSID_CRemoveBackground, 
									NULL, 
									CLSCTX_INPROC, 
									IID_IBaseFilter, 
									(LPVOID *)&pRemoveBackFilter);

			if(hr == S_OK)
			{
				hr = pFg->AddFilter(pRemoveBackFilter, L"RemoveBackGround");
				if(hr != S_OK)
				{
					CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot add background remove filter"), hr);
				}
			}
			else
			{
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot create background remove filter"), hr);
			}
		}*/
	}
	else
	{
		pVSC = NULL;
/*
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,  &MEDIATYPE_Interleaved, pSCap,
													IID_IAMVideoCompression, (void **)&pVC);
		if(hr != S_OK)
			hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSCap, 
												IID_IAMVideoCompression, (void **)&pVC);

		// What if this interface isn't supported?
		// We use this interface to set the frame rate and get the capture size
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pSCap, 
														IID_IAMStreamConfig, (void **)&pVSC);

		if(hr != NOERROR)
		{
			hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSCap,
										IID_IAMStreamConfig, (void **)&pVSC);
			if(hr != NOERROR)
				// This means we can't set frame rate (non-DV only)
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot find IAMStreamConfig"), hr);
		}
*/
	}

	// Add filter for embedd logo to video if Webinaria is for free account
	if (Logo)
	{
		hr = CreateAndInsertFilter(pFg, CLSID_CWebinariaLogoFilter, &pLogoFilter, L"WebinariaLogo");
		if(FAILED(hr))
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot create logo filter"), hr);
			//return hr;
		}
		else if (Branding::Instance()->IsVideoLogoBranded())
		{
			CComQIPtr<ILogoFilter> logoFilter(pLogoFilter);
			CComBSTR smallLogoPath(Branding::Instance()->VideoSmallLogoPath());
			CComBSTR largeLogoPath(Branding::Instance()->VideoLargeLogoPath());

			logoFilter->SetLogoPath(smallLogoPath, largeLogoPath);
		}
	}

	CapAudioIsRelevant = true;

	AM_MEDIA_TYPE *pmt;

	// Default capture format
	if(pVSC && pVSC->GetFormat(&pmt) == S_OK)
	{
		// DV capture does not use a VIDEOINFOHEADER
		if(pmt->formattype == FORMAT_VideoInfo)
		{
			// resize our window to the default capture size
		}
		if(pmt->majortype != MEDIATYPE_Video)
		{
			// This capture filter captures something other that pure video.
			// Maybe it's DV or something?  Anyway, chances are we shouldn't
			// allow capturing audio separately, since our video capture    
			// filter may have audio combined in it already!
			CapAudioIsRelevant = false;
			CapAudio = false;
		}
		DeleteMediaType(pmt);
	}

	if (CapWebcam)
	{
		// Use the crossbar class to help us sort out all the possible video inputs
		// The class needs to be given the capture filters ANALOGVIDEO input pin
		IPin        *pP = 0;
		IEnumPins   *pins=0;
		ULONG        n;
		PIN_INFO     pinInfo;
		bool	     Found = false;
		IKsPropertySet *pKs=0;
		GUID guid;
		DWORD dw;
		bool fMatch = false;

		pCrossbar = NULL;

		if(SUCCEEDED(pVCap->EnumPins(&pins)))
		{            
			while(!Found && (S_OK == pins->Next(1, &pP, &n)))
			{
				if(S_OK == pP->QueryPinInfo(&pinInfo))
				{
					if(pinInfo.dir == PINDIR_INPUT)
					{
						// Is this pin an ANALOGVIDEOIN input pin?
						if(pP->QueryInterface(IID_IKsPropertySet, (void **)&pKs) == S_OK)
						{
							if(pKs->Get(AMPROPSETID_Pin,
								AMPROPERTY_PIN_CATEGORY, NULL, 0,
								&guid, sizeof(GUID), &dw) == S_OK)
							{
								if(guid == PIN_CATEGORY_ANALOGVIDEOIN)
									fMatch = true;
							}
							pKs->Release();
						}

						if(fMatch)
						{
/*
							HRESULT hrCreate=S_OK;
							hr = pBuilder->FindInterface(&LOOK_UPSTREAM_ONLY,  NULL, pVCaptureFilter, IID_IAMCrossbar, (void **)&pCrossbar);
							pCrossbar = new DXShowFilters::CVideoCrossbar(pP, &hrCreate);
							if (!pCrossbar || FAILED(hrCreate))
								break;                             
							hr = pCrossbar->get_PinCounts( GetInputCount(&NumberOfVideoInputs);
*/
							Found = true;
						}
					}
					pinInfo.pFilter->Release();
				}
				pP->Release();
			}
			pins->Release();
		}
	}

	// There's no point making an audio capture filter
	if(CapAudioIsRelevant == false)
		goto SkipAudio;

	// Create the audio capture filter, even if we are not capturing audio right
	// now, so we have all the filters around all the time.

	// We want an audio capture filter and some interfaces

	if(!DevEnum->IsAudioMoniker())
	{
		// there are no audio capture devices. We'll only allow video capture
		CapAudio = false;
		goto SkipAudio;
	}
	pACap = NULL;


	DevEnum->BindAudioToObject((void**)&pACap);

	if(pACap == NULL)
	{
		// There are no audio capture devices. We'll only allow video capture
		CapAudio = false;
		CError::ErrMsg(hMainWnd, TEXT("Cannot create audio capture filter"));
		goto SkipAudio;
	}

	// Put the audio capture filter in the graph
	// Audio device name
	wchar_t * ADN = DevEnum->GetAudioDeviceName();
	// We'll need this in the graph to get audio property pages
	hr = pFg->AddFilter(pACap, ADN);
	delete[] ADN;
	if(hr != NOERROR)
	{
		CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot add audio capture filter to filtergraph"), hr);
		//FreeCaptureFilters();
		//return false;
	}

	// Calling FindInterface below will result in building the upstream
	// section of the capture graph (any WDM TVAudio's or Crossbars we might
	// need).

	// !!! What if this interface isn't supported?
	// we use this interface to set the captured wave format
	SAFE_RELEASE(pASC);
	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, pACap, IID_IAMStreamConfig, (void **)&pASC);
	//ChooseAudioFormat();
	{
	CComPtr<IPin> cpAudioOutPin;
	hr = pBuilder->FindPin(pACap, PINDIR_OUTPUT, NULL, &MEDIATYPE_Audio, FALSE, 0, &cpAudioOutPin);
	if(SUCCEEDED(hr))
	{//set short audio samples:
		CComPtr<IAMBufferNegotiation> cpAMBufferNegotiation;
		hr = cpAudioOutPin->QueryInterface(IID_IAMBufferNegotiation,(void **)&cpAMBufferNegotiation);
		if(SUCCEEDED(hr))
		{
			ALLOCATOR_PROPERTIES AP;
            AP.cbPrefix =-1;
			AP.cbAlign  = 1;
			AP.cbBuffer = 7680;
			AP.cBuffers = 6;
			hr = cpAMBufferNegotiation->SuggestAllocatorProperties(&AP);
		}
	}
	}

	
	// Adding compressor filter
		
	/*if (CapAudio)
	{
		// Audio capture category
		WCHAR szMon[] = L"@device:cm:{33D9A761-90C8-11D0-BD43-00A0C911CE86}\66Microsoft G.723.1";
		IBindCtx *pBindCtx;
		hr = CreateBindCtx(0, &pBindCtx);

		ULONG chEaten = 0;
		IMoniker *pMoniker = 0;
		hr = MkParseDisplayName(pBindCtx, szMon, &chEaten, &pMoniker);
		pBindCtx->Release();
		if (SUCCEEDED(hr))
		{
			pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pAudioCompressor);
			hr = pFg->AddFilter(pAudioCompressor, L"Speech");
			if(hr != S_OK)
			{
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot add audio compressor filter"), hr);
			}

			pMoniker->Release();
		}
		else
		{
			CError::ErrMsg(hMainWnd, TEXT("Error %x: Does not exist Windows Speech Encoder DMO"), hr);
		}
	}*/		
	// End adding compressor filter

SkipAudio:


	// Potential debug output - what the graph looks like
	// DumpGraph(gcap.pFg, 1);

	// We use this interface to bring up the 3 dialogs
	DevEnum->InitDialogInterface(pBuilder,pVCap,pACap);

	return true;
}

// All done with the capture filters and the graph builder
void CAudioVideoGrabber::FreeCaptureFilters()
{	
	RemoveGraphFromRot(g_dwGraphRegister);

	SAFE_RELEASE(pSCap);
	SAFE_RELEASE(pVCap);
	SAFE_RELEASE(pACap);
	SAFE_RELEASE(pASC);
	SAFE_RELEASE(pVSC);
	SAFE_RELEASE(pSSC);
	//SAFE_RELEASE(pVC);
	SAFE_RELEASE(pSink);
	//SAFE_RELEASE(pRemoveBackFilter);
	SAFE_RELEASE(pFirstCompressor);
	SAFE_RELEASE(pSecondCompressor);			
	//SAFE_RELEASE(pAudioCompressor);
	SAFE_RELEASE(pLogoFilter);
	SAFE_RELEASE(pCrossbar);
	SAFE_RELEASE(pFg);
	SAFE_RELEASE(pBuilder);

}


// Tear down everything downstream of the capture filters, so we can build
// a different capture graph.  Notice that we never destroy the capture filters
// and WDM filters upstream of them, because then all the capture settings
// we've set would be lost.
void CAudioVideoGrabber::TearDownGraph()
{

	// Destroy the graph downstream of our capture filters
	
	NukeDownstream(pSCap, pFg);
	NukeDownstream(pVCap, pFg);
	NukeDownstream(pACap, pFg);

//	DemolishGraphFilters(pFg);

}

// Make a graph object we can use for capture graph building
bool CAudioVideoGrabber::MakeGraph()
{
	// We have one already
	if(pFg)
		return true;

	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID *)&pFg);
	AddGraphToRot(pFg, &g_dwGraphRegister);

	return (hr == NOERROR);
}

// Build the capture graph
bool CAudioVideoGrabber::BuildCaptureGraph()
{
	USES_CONVERSION;
	HRESULT hr;
	AM_MEDIA_TYPE *pmt=0;

	// No capture file name yet... we need one first
	if (!SelectFile())
		return false;

	TearDownGraph();
	
	// We need a rendering section that will write the capture file out in AVI file format
	hr = pBuilder->SetOutputFileName(&MEDIASUBTYPE_Avi, File, &pRender, &pSink);
	if(hr != NOERROR)
	{
		CError::ErrMsg(hMainWnd,TEXT("Cannot set output file"));
		TearDownGraph();
		return false;
	}

	// set the proper MASTER STREAM
	MasterStreamNum = 0;
	if (CapWebcam && CapAudio)
		MasterStreamNum = 2;
	else if (CapAudio)
		MasterStreamNum = 1;
	CComPtr<IConfigAviMux> cpConfigAviMux;	
	hr = pRender->QueryInterface(IID_IConfigAviMux, (void **)&cpConfigAviMux);
	if(S_OK==hr)
	{
		//cpConfigAviMux->SetOutputCompatibilityIndex(TRUE);
		hr = cpConfigAviMux->SetMasterStream(MasterStreamNum);
		if(hr != NOERROR)
			CError::ErrMsg(hMainWnd,TEXT("SetMasterStream failed!"));
	}

	if (Logo)
	{
		CComPtr<IBaseFilter> cpSync;
		hr = CreateAndInsertFilter(pFg, CLSID_CSynchronizer, &cpSync, L"SC Synchronizer");
		if(SUCCEEDED(hr))
		{
			hr = pBuilder->RenderStream(NULL, NULL, pSCap, NULL, cpSync);
			if(SUCCEEDED(hr))
			{
				hr = pBuilder->RenderStream(NULL, NULL, cpSync, NULL, pLogoFilter);
				if(SUCCEEDED(hr))
				{
					hr = pBuilder->RenderStream(NULL, NULL, pLogoFilter, NULL, pFirstCompressor);
				}
			}
		}
		if(hr == S_OK)
		{
			hr = pBuilder->RenderStream(NULL, &MEDIATYPE_Video, pFirstCompressor, NULL, pRender);
			if(hr != NOERROR)
			{
				CError::ErrMsg(hMainWnd,TEXT("Cannot save with compression"));
			}
		}
		else
		{
			hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSCap, NULL, pRender);
			if(hr != NOERROR)
			{
				CError::ErrMsg(hMainWnd,TEXT("Cannot render screen capture stream"));
				TearDownGraph();
				return false;
			}
		}
	}
	else
	{
		hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSCap, NULL, pRender);
		if(hr != NOERROR)
		{
			CError::ErrMsg(hMainWnd,TEXT("Cannot render screen capture stream"));
			TearDownGraph();
			return false;
		}
	}

	if (CapWebcam)
	{
		CComPtr<IBaseFilter> cpSync;
		hr = CreateAndInsertFilter(pFg, CLSID_CSynchronizer, &cpSync, L"Webcam Synchronizer");
		if(SUCCEEDED(hr))
		{
			hr = pBuilder->RenderStream(NULL, &MEDIATYPE_Video, pVCap, NULL, cpSync);
			if(SUCCEEDED(hr))
			{
				hr = pBuilder->RenderStream(NULL, NULL, cpSync, NULL, pSecondCompressor);
			}
		}
		if(FAILED(hr))
		{
			hr = pBuilder->RenderStream(NULL, &MEDIATYPE_Video, pVCap, NULL, pSecondCompressor);
		}
		if(hr == S_OK)
		{
			hr = pBuilder->RenderStream(NULL, &MEDIATYPE_Video, pSecondCompressor, NULL, pRender);
			if(hr != NOERROR)
			{
				CError::ErrMsg(hMainWnd,TEXT("Cannot save with compression"));
			}
		}
		else
		{
			CError::ErrMsg(hMainWnd,TEXT("Cannot add webcam stream"));
			hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pVCap, NULL, pRender);
			if(hr != NOERROR)
			{
				CError::ErrMsg(hMainWnd,TEXT("Cannot render webcam stream"));
			}
		}
			/*if (TBack)
			{
				hr = pBuilder->ConnectFilters(pVCap, pRemoveBackFilter);
				if(hr == S_OK)
				{
					hr = pBuilder->RenderStream(NULL, &MEDIATYPE_Video, pRemoveBackFilter, NULL, pRender);
					if(hr != NOERROR)
					{
						CError::ErrMsg(hMainWnd,TEXT("Cannot render video capture stream"));
					}
				}
				else
				{
					CError::ErrMsg(hMainWnd,TEXT("Cannot add removebackup filter to stream"));
					hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pVCap, NULL, pRender);
					if(hr != NOERROR)
					{
						CError::ErrMsg(hMainWnd,TEXT("Cannot render video capture stream"));
					}
				}
			}
			else
			{
				hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pVCap, NULL, pRender);
				if(hr != NOERROR)
				{
					CError::ErrMsg(hMainWnd,TEXT("Cannot render video capture stream"));
				}
			}*/
	}
	// Render the audio capture pin?
	if(CapAudio)
	{
		CComPtr<IBaseFilter> cpSync;
		hr = CreateAndInsertFilter(pFg, CLSID_CSynchronizer, &cpSync, L"Audio Synchronizer");
		if(SUCCEEDED(hr))
		{
			hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, pACap, NULL, cpSync);
			if(SUCCEEDED(hr))
			{
				hr = pBuilder->RenderStream(NULL, NULL, cpSync, NULL, pRender);
			}
		}
		if(FAILED(hr))
		{
			hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, pACap, NULL, pRender);
		}
		if(hr != NOERROR)
		{
			CError::ErrMsg(hMainWnd, TEXT("Cannot render audio capture stream."));
			TearDownGraph();
			return false;
		}				
	}

	// Now tell it what frame rate to capture at.  Just find the format it
	// is capturing with, and leave everything alone but change the frame rate
	hr = UseFrameRate ? E_FAIL : NOERROR;
	if(pVSC && UseFrameRate)
	{
		hr = pVSC->GetFormat(&pmt);
		// DV capture does not use a VIDEOINFOHEADER
		if(hr == NOERROR)
		{
			if(pmt->formattype == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
				pvi->AvgTimePerFrame = (LONGLONG)(10000000 / FrameRate);
				hr = pVSC->SetFormat(pmt);
			}
			DeleteMediaType(pmt);
		}
		if(hr != NOERROR)
			CError::ErrMsg(hMainWnd, TEXT("Cannot set frame rate for capture"));
	}
	// Now ask the filtergraph to tell us when something is completed or aborted
	// (EC_COMPLETE, EC_USERABORT, EC_ERRORABORT).  This is how we will find out
	// if the disk gets full while capturing
	hr = pFg->QueryInterface(IID_IMediaEventEx, (void **)&pME);
	if(hr == NOERROR)
	{
		pME->SetNotifyWindow((OAHWND)hMainWnd, WM_FGNOTIFY, 0);
	}

	// potential debug output - what the graph looks like
	// DumpGraph(gcap.pFg, 1);

	

	// All done.
	CaptureGraphBuilt = true;
	return true;
}

// Make some options
void CAudioVideoGrabber::MakeOptionsAndDialogs()
{

}
