#include "stdafx.h"

#include "FLVConverter.h"
#include "RGBFilters_i.c"
#include "utils.h"
#include "FileSelector.h"
#include "Folders.h"

using namespace  EditorApplication::EditorLogical;

const TCHAR *g_ThumbnailSuffix = _T("_thumb");


IGraphBuilder * CFLVConverter::pGB = NULL;
IMediaControl * CFLVConverter::pMC = NULL;
IMediaSeeking * CFLVConverter::pMS = NULL;
IMediaEventEx * CFLVConverter::pME = NULL;
IBaseFilter *	CFLVConverter::pSource = NULL;
IBaseFilter *	CFLVConverter::pAVISplitter = NULL;
IBaseFilter *	CFLVConverter::pAdder = NULL;
IAdder *		CFLVConverter::pAdderSettings = NULL;
IBaseFilter *	CFLVConverter::pLogo = NULL;
IBaseFilter *	CFLVConverter::pAVIMux = NULL;
IBaseFilter *	CFLVConverter::pWriter = NULL;
IBaseFilter *	CFLVConverter::pCompressor = NULL;
wchar_t *		CFLVConverter::DistFileName = NULL;
wchar_t *		CFLVConverter::tmpAVI = NULL;
LONGLONG		CFLVConverter::m_llVideoFramesCount = 0;
LONGLONG		CFLVConverter::m_llAudioSamplesCount = 0;
WE_WAVEFORMATEX CFLVConverter::m_wfx;
DWORD			CFLVConverter::dwRotReg = 0;
FileQuality		CFLVConverter::SelectedFileQuality = FileQualityMedium;

CFLVConverter::CFLVConverter(void)
{
}
CFLVConverter::~CFLVConverter(void)
{
}

bool CFLVConverter::PreSaveToFLV(	const HWND & hMainWnd,
									const CParameters & param,
									const wchar_t * SourceFileName,
									IMediaEventEx ** pMEE,
                                    Timeline *  timeline)
{
IPin * pOutputPin = NULL;
IPin * pInputPin = NULL;
bool showLogo = true;


			m_llAudioSamplesCount = m_llVideoFramesCount = 0;
			

			int iLen = wcslen(SourceFileName)+1;
			DistFileName = new wchar_t[MAX_PATH];
			wcscpy_s(DistFileName, MAX_PATH, SourceFileName);
			DistFileName[iLen-2] = 'v';
			DistFileName[iLen-3] = 'l';
			DistFileName[iLen-4] = 'f';
			if (!OpenFileDialog(hMainWnd, SourceFileName, DistFileName))
			{
				delete[] DistFileName;
				return false;
			}

			//	Ask user about file quality.
			FileQualitySelector dlg(hMainWnd);
			if (IDOK == dlg.Show())
				SelectedFileQuality = dlg.SelectedQuality();
			else
				return false;

			SendMessage(hMainWnd, WM_NCPAINT, 1,0);

			iLen = wcslen(DistFileName)+14;
			tmpAVI = new wchar_t[iLen ];
			wcscpy_s(tmpAVI, iLen, DistFileName);
			wcscat_s(tmpAVI, iLen, L"59temp44.avi");
			// Create a DirectShow GraphBuilder object
			HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGB);
			if (FAILED(hr))
			{
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Filter graph built failed"), hr);
				Stop();
				return false;
			}
		
			// Get DirectShow interfaces
			hr = pGB->QueryInterface(IID_IMediaControl, (void **)&pMC);
			if (FAILED(hr))
			{
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Create filter graph control"), hr);
				Stop();
				return false;
			}
			hr = pGB->QueryInterface(IID_IMediaEventEx, (void **)&pME);
			if (FAILED(hr))
			{
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Create filter graph event handler"), hr);
				Stop();
				return false;
			}

			// Have the graph signal event via window callbacks
			hr = pME->SetNotifyWindow((OAHWND)hMainWnd, WM_FGNOTIFY2, 0);
			if (FAILED(hr))
			{
				CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed graph notify message"), hr);
				Stop();
				return false;
			}

			*pMEE = pME;
			(*pMEE)->AddRef();

			if(SUCCEEDED(hr))
			{
				// Add the source filter to the graph
				hr = pGB->AddSourceFilter(SourceFileName, L"SOURCE", &pSource);
				if (FAILED(hr))
				{
					CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed adding source filter"), hr);
					Stop();
					return false;
				}

				// Add the splitter filter to the graph
				hr = CoCreateInstance(CLSID_AviSplitter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pAVISplitter);
				if (FAILED(hr))
				{
					CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed creating splitter filter"), hr);
					Stop();
					return false;
				}
				// Add the mixer filter to the graph
				hr = CoCreateInstance(CLSID_CAdder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pAdder);
				if (FAILED(hr))
				{
					CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed creating adder filter"), hr);
					Stop();
					return false;
				}

                if(timeline)
                {
	                CComPtr <ITimelineFilter> cpTimelineFilter;
		            hr = pAdder->QueryInterface(IID_ITimelineFilter, (void **)&cpTimelineFilter);
		            if (FAILED(hr))
		            {
			            CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed creating ITimelineFilter interface"), hr);
			            Stop();
			            return false;
		            }
                    CComPtr <ITimeline> cpTimeline;
                    hr = timeline->QueryInterface(IID_ITimeline, (void**) &cpTimeline);
                    if(SUCCEEDED(hr))
                    {
                        hr = cpTimelineFilter->AssignTimeline(cpTimeline);
                        if(FAILED(hr))
                        {
			                CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed assigning ITimeline instance to filter"), hr);
			                Stop();
			                return false;
                        }
                    }
                    else
                    {
			            CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed creating ITimeline interface"), hr);
			            Stop();
			            return false;
                    }
                }

				hr = pAdder->QueryInterface(IID_IAdder, (void **)&pAdderSettings);
				if (FAILED(hr))
				{
					CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed creating adder settings interface"), hr);
					Stop();
					return false;
				}				
				
				pAdderSettings->SetIntervals((void*)param.GetAllDeletedInterval(),
											 (void*)param.GetWebDeletedInterval(),
											 (void*)param.GetWebPosInterval(),
											 (void*)param.GetArrowInterval(),
											 (void*)param.GetTextInterval());

                //  Generate thumbnail file name.
                TCHAR thumbnail[MAX_PATH];
                if (0 == _tcscpy_s(thumbnail, _countof(thumbnail), DistFileName))
                {
                    TCHAR *end = _tcsrchr(thumbnail, _T('.'));
                    if (!end)
                        end = &thumbnail[_tcslen(thumbnail)];

                    if ((end - &thumbnail[0]) + _tcslen(g_ThumbnailSuffix) < MAX_PATH)
                    {
                        _tcscpy(end, g_ThumbnailSuffix);
                        CT2W wThumbnail(thumbnail);
                        pAdderSettings->SetThumbnailPath(CComBSTR(wThumbnail));
                    }
                }

				// Add the logo filter to the graph
				if(showLogo)
				{
					hr = CoCreateInstance(CLSID_CWebinariaLogoFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pLogo);
					if (FAILED(hr))
					{
						CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed creating logo filter"), hr);
						Stop();
						return false;
					}
				}
				
				// Add the mux filter to the graph
				hr = CoCreateInstance(CLSID_AviDest, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pAVIMux);
				if (FAILED(hr))
				{
					CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed creating destination avi filter"), hr);
					Stop();
					return false;
				}

				// Add the writer filter to the graph
				hr = CoCreateInstance(CLSID_FileWriter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pWriter);
				if (FAILED(hr))
				{
					CError::ErrMsg(hMainWnd, TEXT("Error %x: Failed creating destination avi filter"), hr);
					Stop();
					return false;
				}

				IFileSinkFilter2 * pSink;
				hr = pWriter->QueryInterface(IID_IFileSinkFilter2, (void**)&pSink);
				pSink->SetFileName(tmpAVI, NULL);
				pSink->SetMode(AM_FILE_OVERWRITE);
				pSink->Release();

				// Get the interface for the first output pin
				hr = GetUnconnectedPin(pSource, PINDIR_OUTPUT, &pOutputPin);

				hr = JoinFilterToChain(pGB, pAVISplitter, L"AVI Splitter", &pOutputPin);
				if(SUCCEEDED(hr))
				{
					//asking Out pin of the avi splitter for IMediaSeeking:
					hr = pOutputPin->QueryInterface(IID_IMediaSeeking, (void **)&pMS);
					if (FAILED(hr))
					{
						CError::ErrMsg(hMainWnd, TEXT("Error %x: Create filter graph frame seeking"), hr);
						Stop();
						return false;
					}

					if(showLogo)
					{
						hr = JoinFilterToChain(pGB, pLogo, L"WebinariaLogoFilter", &pOutputPin);
					}

					hr = pMS->SetTimeFormat( &TIME_FORMAT_FRAME );
					if(SUCCEEDED(hr))
					{
						pMS->GetDuration( &m_llVideoFramesCount );
					}

				}
				if(SUCCEEDED(hr))
				{
					hr = JoinFilterToChain(pGB, pAdder, L"Adder", &pOutputPin);
				}
				// Add compressor
				if(SUCCEEDED(hr))
				{
					hr = CreateVideoCompressor(&pCompressor);
					if(SUCCEEDED(hr))
					{
						hr = JoinFilterToChain(pGB, pCompressor, L"Video Compressor", &pOutputPin);
					}
					else
					{
						CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot add video compressor filter"), hr);
					}
				}
				if(SUCCEEDED(hr))
				{
					hr = JoinFilterToChain(pGB, pAVIMux, L"AVI MUX", &pOutputPin);
				}
				if(SUCCEEDED(hr))
				{
					hr = JoinFilterToChain(pGB, pWriter, L"File Writer", &pOutputPin);
				}						
				SAFE_RELEASE(pOutputPin);
				
				if(SUCCEEDED(hr))
				{//try to connect optional streams:
					for(int i=0; i<2; i++)
					{
						GetUnconnectedPin(pAVISplitter, PINDIR_OUTPUT, &pOutputPin);
						if(pOutputPin)
						{
							if(ptVideo==GetPinType(pOutputPin ))
							{//second video stream (webcam)
								GetUnconnectedPin(pAdder, PINDIR_INPUT, &pInputPin);
								if(pInputPin)
								{
									hr = pGB->Connect(pOutputPin, pInputPin);
									SAFE_RELEASE(pInputPin);
									if(FAILED(hr))
									{
										CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot connect webcam stream to muxer"), hr);
									}

								}
							}
							else if(ptAudio==GetPinType(pOutputPin ))
							{// audio stream
								GetUnconnectedPin(pAVIMux, PINDIR_INPUT, &pInputPin);
								if (pOutputPin && pInputPin)
								{
									hr = AssembeAudioBranch(param, pOutputPin, pInputPin);
									SAFE_RELEASE(pInputPin);
									if(FAILED(hr))
									{
										CError::ErrMsg(hMainWnd, TEXT("Error %x: Cannot add render audio branch"), hr);
									}
								}
							}
							SAFE_RELEASE(pOutputPin);
							if(FAILED(hr))
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
				}
				if(FAILED(hr))
				{
					Stop();
					return false;
				}
				AddGraphToRot(pGB, &dwRotReg);
			}

			hr = pMC->Run();

			return true;
		}
void CFLVConverter::ReleaseDirectShow()
{
	if (pMC)
		pMC->Stop();
	if (pME)
		pME->SetNotifyWindow((OAHWND)NULL, 0, 0);

	RemoveGraphFromRot(dwRotReg);

	DemolishGraphFilters(pGB);

	SAFE_RELEASE(pCompressor);
	SAFE_RELEASE(pWriter);
	SAFE_RELEASE(pAVIMux);
	SAFE_RELEASE(pLogo);
	SAFE_RELEASE(pAdderSettings);
	SAFE_RELEASE(pAdder);
	SAFE_RELEASE(pAVISplitter);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pGB);
	SAFE_RELEASE(pME);
	SAFE_RELEASE(pMS);
	SAFE_RELEASE(pMC);

}
void CFLVConverter::Stop()
{

	ReleaseDirectShow();
	// Delete file if it is exist
#ifndef DEBUG
	DeleteFileW(tmpAVI);
#endif
	if (tmpAVI != NULL)
		delete[] tmpAVI;
	if (DistFileName != NULL)
		delete[] DistFileName;

}

bool CFLVConverter::PostSaveToFLV(int iBitrate)
{
	ReleaseDirectShow();

	if (!ConvertionShell(tmpAVI, DistFileName, iBitrate))
		return false;

	DeleteFileW(tmpAVI);

	bool result = FileExists(DistFileName);

	if (tmpAVI != NULL)
		delete[] tmpAVI;
	if (DistFileName != NULL)
		delete[] DistFileName;

	return result;
}

bool CFLVConverter::ConvertionShell(const wchar_t * Source, const wchar_t * Dist, int iBitrate)
{
	TCHAR CommandString[(MAX_PATH*2)];
	TCHAR tcBitrate[32];
	GetAppFolder(CommandString, N_ELEMENTS(CommandString));

	//wcscpy(CommandString, L"ffmpeg\\ffmpeg.exe -i \"");
	wcscat_s(CommandString, N_ELEMENTS(CommandString), L"ffmpeg\\ffmpeg.exe -i ");
	
	wcscat_s(CommandString, N_ELEMENTS(CommandString), L"\"");
	wcscat_s(CommandString, N_ELEMENTS(CommandString), Source);
	wcscat_s(CommandString, N_ELEMENTS(CommandString), L"\"");

	_sntprintf(tcBitrate, N_ELEMENTS(tcBitrate), TEXT("\x20-b %d\x20"), iBitrate);
	wcscat_s(CommandString, N_ELEMENTS(CommandString), tcBitrate);

	wcscat_s(CommandString, N_ELEMENTS(CommandString), L" -acodec copy -y ");

	wcscat_s(CommandString, N_ELEMENTS(CommandString), L"\"");
	wcscat_s(CommandString, N_ELEMENTS(CommandString), Dist);
	wcscat_s(CommandString, N_ELEMENTS(CommandString), L"\"");

	wcscat_s(CommandString, N_ELEMENTS(CommandString), L" -threads 2");
	

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process. 
	if( !CreateProcessW(NULL,			  // No module name (use command line). 
						CommandString,	  // Command line. 
						NULL,             // Process handle not inheritable. 
						NULL,             // Thread handle not inheritable. 
						FALSE,            // Set handle inheritance to FALSE. 
						CREATE_NO_WINDOW, // No creation flags. 
						NULL,             // Use parent's environment block. 
                        Folders::My(), // Use parent's starting directory. 
						&si,              // Pointer to STARTUPINFO structure.
						&pi ))            // Pointer to PROCESS_INFORMATION structure.
	{
		CError::ErrMsg(NULL, L"Doesn't exist ffmpeg");

		if (tmpAVI != NULL)
			delete[] tmpAVI;
		if (DistFileName != NULL)
			delete[] DistFileName;

		return false;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	return true;
}

// Put up the open file dialog
bool CFLVConverter::OpenFileDialog(const HWND hMainWnd, const wchar_t * AVI, wchar_t * FLV)
{
    FileSelector selector;

    if (selector.Select(hMainWnd, SelectToSave, FLV, FileTypeFlv))
	{
        wcscpy_s(FLV, MAX_PATH, selector.FileName());
		DeleteFileW(FLV);
		return true;
	}

    return false;
}
LONGLONG CFLVConverter::GetDuration(void)
{
	return m_llVideoFramesCount;
}
HRESULT CFLVConverter::CreateVideoCompressor(IBaseFilter** ppFilter)
{
// Video capture category
static WCHAR szMon[] = L"@device:cm:{33D9A760-90C8-11D0-BD43-00A0C911CE86}\\msvc";
IBindCtx *pBindCtx;
ULONG chEaten = 0;
IMoniker *pMoniker = 0;
	if(!ppFilter) return E_POINTER;
	*ppFilter = NULL;
	HRESULT hr = CreateBindCtx(0, &pBindCtx);

	hr = MkParseDisplayName(pBindCtx, szMon, &chEaten, &pMoniker);
	pBindCtx->Release();
	if (SUCCEEDED(hr))
	{
		hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**) ppFilter);
		//need not: hr = pGB->AddFilter(ppFilter, L"MSVC1");
		pMoniker->Release();
	}
	return hr;
}
HRESULT CFLVConverter::CreateAudioCompressor(WAVEFORMATEX* pWfx, IBaseFilter** ppFilter)
{
HRESULT hr = S_FALSE;
	if(!ppFilter || !pWfx)
		return E_POINTER;
	*ppFilter = NULL;
	if(pWfx->wFormatTag!=WAVE_FORMAT_PCM)
	{
		CComPtr<IFilterMapper2> cpMapper = NULL;
		CComPtr<IEnumMoniker> cpEnum = NULL;
		GUID arrayOutTypes[2];
		IMoniker *pMoniker;
		ULONG cFetched;

		hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC, IID_IFilterMapper2,  (void **) &cpMapper);
		if(FAILED(hr))
			return hr;
		arrayOutTypes[0]= MEDIATYPE_Audio;
		arrayOutTypes[1] =FOURCCMap(pWfx->wFormatTag);
		hr = cpMapper->EnumMatchingFilters(	&cpEnum,				// *out* Moniker to create
											0,						// Reserved.
											TRUE,					// Use exact match?
											MERIT_SW_COMPRESSOR,	// Minimum merit.
											TRUE,					// At least one input pin?
											0,//  1,                // Number of major type/subtype pairs for input.
											0,						// Array of major type/subtype pairs for input.
											NULL,					// Input medium.
											NULL,					// Input pin category.
											FALSE,					// Must be a renderer?
											TRUE,					// At least one output pin?
											1,						// Number of major type/subtype pairs for output.
											arrayOutTypes,			// Array of major type/subtype pairs for output.
											NULL,					// Output medium.
											NULL);					// Output pin category.
		if(SUCCEEDED(hr))
		{
			if(cpEnum->Next(1, &pMoniker, &cFetched) == S_OK)
			{//Give First
				hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**) ppFilter);
				pMoniker->Release();
			}
		}
    }
	return hr;
}
HRESULT CFLVConverter::AssembeAudioBranch(const CParameters & param, IPin* pSrc, IPin* pDest)
{
HRESULT hr=S_OK;
IPin* pEndPin = NULL;
AM_MEDIA_TYPE mtSrc;
AM_MEDIA_TYPE *pmtWrp    = NULL;
WE_WAVEFORMATEX* pwfxSrc = NULL;
WE_WAVEFORMATEX* pwfxWrp = NULL;
ULONG ulFetched = 0;
	if(!pSrc || !pDest)
		return E_POINTER;

	m_wfx.wFormatTag      = WAVE_FORMAT_MPEGLAYER3;
	m_wfx.nChannels       = 1;
	m_wfx.nSamplesPerSec  = 11025;
	m_wfx.wBitsPerSample  = 0;
	m_wfx.nBlockAlign     = 1;
	m_wfx.nAvgBytesPerSec = 2500;
	m_wfx.cbSize          = MPEGLAYER3_WFX_EXTRA_BYTES;
	MPEGLAYER3WAVEFORMAT* pMp3Format = (MPEGLAYER3WAVEFORMAT*) &m_wfx;
	pMp3Format->wID             = MPEGLAYER3_ID_MPEG;
	pMp3Format->fdwFlags        = MPEGLAYER3_FLAG_PADDING_ON;//MPEGLAYER3_FLAG_PADDING_OFF;
	pMp3Format->nBlockSize      = 132;
	pMp3Format->nFramesPerBlock = 1;
	pMp3Format->nCodecDelay     = 1393;

	CComPtr<IMediaSeeking> cpMediaSeeking;
	hr = pSrc->QueryInterface(IID_IMediaSeeking, (void **)&cpMediaSeeking);
	if(SUCCEEDED(hr))
	{
		hr = cpMediaSeeking->SetTimeFormat(&TIME_FORMAT_SAMPLE);
		if(SUCCEEDED(hr))
		{
			cpMediaSeeking->GetDuration(&m_llAudioSamplesCount);
		}
	}

	pEndPin = pSrc;
	pEndPin->AddRef();

	CComPtr<IBaseFilter> cpAudioSkipFilter;
	hr = CreateAndInsertFilter(pGB, CLSID_CAudioSkipper, &cpAudioSkipFilter, L"AudioSkipper");
	if(SUCCEEDED(hr))
	{

		hr = JoinFilterToChain(pGB, cpAudioSkipFilter, &pEndPin);

		CComPtr<IAudioSkip> cpAudioSkip;
		hr = cpAudioSkipFilter->QueryInterface(IID_IAudioSkip, (void**) &cpAudioSkip);
		if(SUCCEEDED(hr))
		{
			cpAudioSkip->SetSamplesCount(m_llVideoFramesCount, m_llAudioSamplesCount);
			cpAudioSkip->SetIntervals((void*)param.GetAllDeletedInterval(), (void*)param.GetAudioDeletedInterval());
		}
	}
	if(FAILED(hr))
	{
		SAFE_RELEASE(pEndPin);
		return hr;
	}
	hr = pSrc->ConnectionMediaType(&mtSrc);
	if(SUCCEEDED(hr))
	{
		pwfxSrc = (WE_WAVEFORMATEX*) mtSrc.pbFormat;
		if((pwfxSrc->wFormatTag!=m_wfx.wFormatTag) || (pwfxSrc->nSamplesPerSec!=m_wfx.nSamplesPerSec))
		{
			CComPtr<IBaseFilter> cpAcmWrapper = NULL;
			hr = CoCreateInstance(CLSID_ACMWrapper, NULL, CLSCTX_INPROC, IID_IBaseFilter, (LPVOID *)&cpAcmWrapper);
			if(SUCCEEDED(hr))
			{
				hr = JoinFilterToChain(pGB, cpAcmWrapper, L"ACM Wrapper", &pEndPin);
				if(SUCCEEDED(hr))
				{
					CComPtr<IAMStreamConfig> cpAudioStreamConfig;
					hr = pEndPin->QueryInterface(IID_IAMStreamConfig,(void **)&cpAudioStreamConfig);
					if(SUCCEEDED(hr))
					{
						hr = cpAudioStreamConfig->GetFormat(&pmtWrp);
						if(SUCCEEDED(hr))
						{
							pwfxWrp = (WE_WAVEFORMATEX*) pmtWrp->pbFormat;
							if(WAVE_FORMAT_PCM!=m_wfx.wFormatTag)
							{
								WAVEFORMATEX wfxSrs;
								wfxSrs.nChannels  = m_wfx.nChannels;
								wfxSrs.wFormatTag = WAVE_FORMAT_PCM;
								MMRESULT mmr = acmFormatSuggest(NULL, &m_wfx, &wfxSrs, sizeof(wfxSrs),ACM_FORMATSUGGESTF_NCHANNELS| ACM_FORMATSUGGESTF_WFORMATTAG);
								if(MMSYSERR_NOERROR==mmr)
								{
									pwfxWrp->nChannels       = wfxSrs.nChannels;
									pwfxWrp->nSamplesPerSec  = wfxSrs.nSamplesPerSec;
									pwfxWrp->nAvgBytesPerSec = wfxSrs.nAvgBytesPerSec;
									pwfxWrp->nBlockAlign     = wfxSrs.nBlockAlign;
									pwfxWrp->wBitsPerSample  = wfxSrs.wBitsPerSample;
								}
							}
							else
							{
								pwfxWrp->nChannels       = m_wfx.nChannels;
								pwfxWrp->nSamplesPerSec  = m_wfx.nSamplesPerSec;
								pwfxWrp->nAvgBytesPerSec = m_wfx.nAvgBytesPerSec;
								pwfxWrp->nBlockAlign     = m_wfx.nBlockAlign;
								pwfxWrp->wBitsPerSample  = m_wfx.wBitsPerSample;
							}
							hr = cpAudioStreamConfig->SetFormat(pmtWrp);
							DeleteMediaType(pmtWrp);
						}
					}
				}
			}

			if(WAVE_FORMAT_PCM!=m_wfx.wFormatTag)
			{
				CComPtr<IBaseFilter> cpAComp;
				hr = CreateAudioCompressor(&m_wfx, &cpAComp);
				if(S_OK==hr)
				{
					hr = JoinFilterToChain(pGB, cpAComp, L"Audio compressor", &pEndPin); 
					if(SUCCEEDED(hr))
					{
						CComPtr<IBaseFilter> cpTimeRemover;
						hr = CoCreateInstance(CLSID_CTimeRemover, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&cpTimeRemover);
						if(SUCCEEDED(hr))
						{	//this filter needs as workaround for 0x80040228 error code generated by some
							//audio compressors:
							hr = JoinFilterToChain(pGB, cpTimeRemover, L"TimeStamp remover", &pEndPin); 
						}
					}
				}
			}
		}
	}
	hr = pGB->Connect(pDest, pEndPin);
	FreeMediaType(mtSrc);
	SAFE_RELEASE(pEndPin);
	return hr;
}

/*
//it is only for live audio sources:
HRESULT CFLVConverter::AssembeAudioBranch(IPin* pSrc, IPin* pDest)
{
HRESULT hr=S_OK;
CComPtr <IAMBufferNegotiation> cpAMBufferNegotiation;
CComPtr<IAMStreamConfig> cpAudioStreamConfig;
IPin* pEndPin = NULL;

	if(!pSrc || !pDest)
		return E_POINTER;

	m_wfx.wFormatTag      = WAVE_FORMAT_PCM;
	m_wfx.nChannels       = 2;
	m_wfx.nSamplesPerSec  = 44100;
	m_wfx.wBitsPerSample  = 16;
	m_wfx.nBlockAlign     = (m_wfx.nChannels * m_wfx.wBitsPerSample)/8;
	m_wfx.nAvgBytesPerSec = m_wfx.nBlockAlign * m_wfx.nSamplesPerSec;

	pEndPin = pSrc;
	pEndPin->AddRef();

	hr = pSrc->QueryInterface(IID_IAMStreamConfig,(void **)&cpAudioStreamConfig);
	if(SUCCEEDED(hr))
	{
		AM_MEDIA_TYPE *pAmMediaType;
        WAVEFORMATEX *wf;
		hr = cpAudioStreamConfig->GetFormat(&pAmMediaType);
		if(SUCCEEDED(hr))
		{
			WE_WAVEFORMATEX wfxSrs;
            wf = (WAVEFORMATEX *) pAmMediaType->pbFormat;
			memcpy(&wfxSrs, wf, sizeof(WAVEFORMATEX) + wf->cbSize);
			wfxSrs.nChannels = m_wfx.nChannels;
			MMRESULT mmr = acmFormatSuggest(NULL, &m_wfx, &wfxSrs, sizeof(wfxSrs),ACM_FORMATSUGGESTF_NCHANNELS| ACM_FORMATSUGGESTF_WFORMATTAG);
			if(MMSYSERR_NOERROR==mmr)
            {
                wf->nChannels       = wfxSrs.nChannels;
                wf->nSamplesPerSec  = wfxSrs.nSamplesPerSec;
                wf->nAvgBytesPerSec = wfxSrs.nAvgBytesPerSec;
                wf->nBlockAlign     = wfxSrs.nBlockAlign;
                wf->wBitsPerSample  = wfxSrs.wBitsPerSample;
		
                hr = cpAudioStreamConfig->SetFormat(pAmMediaType);
                if(FAILED(hr))
                {//format not supported. Try to use resampler:
                    CComPtr<IBaseFilter> cpAcmWrapper = NULL;
                    hr = CoCreateInstance(CLSID_ACMWrapper, NULL, CLSCTX_INPROC, IID_IBaseFilter, (LPVOID *)&cpAcmWrapper);
                    if(SUCCEEDED(hr))
                    {
                        hr = JoinFilterToChain(pGB, cpAcmWrapper, L"ACM Wrapper", &pEndPin);
                        if(SUCCEEDED(hr))
                        {
                            CComPtr<IAMStreamConfig> cpAudioStreamConfig2;
                            hr = pEndPin->QueryInterface(IID_IAMStreamConfig,(void **)&cpAudioStreamConfig2);
                            if(SUCCEEDED(hr))
                            {
                                hr = cpAudioStreamConfig2->SetFormat(pAmMediaType);
                                //LogInfo("AudioResampler will used");
                            }
                        }
                    }
                }
			}
			else
			{
				hr = E_FAIL;
			}
			DeleteMediaType(pAmMediaType);
		}
		else
		{

		}
	}

	if(WAVE_FORMAT_PCM!=m_wfx.wFormatTag)
	{
		CComPtr<IBaseFilter> cpAComp;
		hr = CreateAudioCompressor(&m_wfx, &cpAComp);
		if(S_OK==hr)
		{
			hr = JoinFilterToChain(pGB, cpAComp, L"Audio compressor", &pEndPin); 
		}
	}
//	if(SUCCEEDED(hr))
	{
		hr = pGB->Connect(pDest, pEndPin);
	}
	SAFE_RELEASE(pEndPin);
	return hr;
}
*/