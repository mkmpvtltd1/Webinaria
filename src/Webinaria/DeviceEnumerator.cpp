#include "StdAfx.h"
#include "DeviceEnumerator.h"

namespace WebinariaApplication
{

	const CLSID CLSID_UScreenCapture = {0x3755D805, 0x2E56, 0x4A74, {0xBD, 0x0F, 0x13, 0xC7, 0x49, 0x3A, 0x35, 0xC8}};

	//////////////////////////////////////////////////////////////////////////
	//								Public methods							//
	//////////////////////////////////////////////////////////////////////////
	// Default constructor
	CDeviceEnumerator::CDeviceEnumerator(const HWND & hWnd):	curAudioDevice(NULL),
																curVideoDevice(NULL),
																UScreenCaptureDevice(NULL),
																curAudioDeviceNum(0),
																curVideoDeviceNum(0),
																VideoDeviceNum(0),
																AudioDeviceNum(0),
																hMainWnd(hWnd),
																pDlg(NULL),
																pACap(NULL),
																pVCap(NULL),
																pSC(NULL),
																IsFormatDialog(false),
																IsSourceDialog(false),
																IsAudioDialog(false)
	{
		for (short i = 0;i<10;++i)
		{
			AudioDevices[i] = NULL;
			VideoDevices[i] = NULL;
			AudioDeviceName[i] = NULL;
			VideoDeviceName[i] = NULL;
		}

		GetAllVideoCapturedDevices();
		GetAllAudioCapturedDevices();
	}

	// Virtual destructor
	CDeviceEnumerator::~CDeviceEnumerator(void)
	{
		for(int i = 0; i < VideoDeviceNum; i++)
		{
			VideoDevices[i]->Release();
			VideoDevices[i] = NULL;
			delete[] VideoDeviceName[i];
			VideoDeviceName[i] = NULL;
		}
		for(int i = 0; i < AudioDeviceNum; i++)
		{
			AudioDevices[i]->Release();
			AudioDevices[i] = NULL;
			delete[] AudioDeviceName[i];
			AudioDeviceName[i] = NULL;
		}
		curAudioDevice = NULL;
		curVideoDevice = NULL;
		curAudioDeviceNum = 0;
		curVideoDeviceNum = 0;
		AudioDeviceNum = 0;
		VideoDeviceNum = 0;
		SAFE_RELEASE(UScreenCaptureDevice);
		SAFE_RELEASE(pDlg);
		SAFE_RELEASE(pSC);
		pACap = NULL;
		pVCap = NULL;
	}

	// Get video devices names
	void CDeviceEnumerator::GetVideoDevices(wchar_t **& Names, unsigned short & DeviceCount, unsigned short & CurrenDeviceNum)
	{
		GetAllVideoCapturedDevices();
		Names = new wchar_t*[VideoDeviceNum];
		for (unsigned short i = 0;i<VideoDeviceNum;++i)
		{
			Names[i] = new wchar_t[wcslen(VideoDeviceName[i])+1];
			wcscpy(Names[i],VideoDeviceName[i]);
		}
		DeviceCount = VideoDeviceNum;
		CurrenDeviceNum = curVideoDeviceNum;
	}

	// Get audio devices names
	void CDeviceEnumerator::GetAudioDevices(wchar_t **& Names, unsigned short & DeviceCount, unsigned short & CurrenDeviceNum)
	{
		GetAllAudioCapturedDevices();
		Names = new wchar_t*[AudioDeviceNum];
		for (unsigned short i = 0;i<AudioDeviceNum;++i)
		{
			Names[i] = new wchar_t[wcslen(AudioDeviceName[i])+1];
			wcscpy(Names[i],AudioDeviceName[i]);
		}
		DeviceCount = AudioDeviceNum;
		CurrenDeviceNum = curAudioDeviceNum;
	}

	// Set current video device by index (calculate from 0)
	void CDeviceEnumerator::SetVideoDevice(const unsigned short & index)
	{
		if ((index >= 0)&&(index < VideoDeviceNum))
		{
			curVideoDeviceNum = index;
			curVideoDevice = VideoDevices[index];
		}
	}

	// Set current audio device by index (calculate from 0)
	void CDeviceEnumerator::SetAudioDevice(const unsigned short & index)
	{
		if ((index >= 0) && (index < AudioDeviceNum))
		{
			curAudioDeviceNum = index;
			curAudioDevice = AudioDevices[index];
		}
	}

	// Is current audio device moniker not NULL?
	bool CDeviceEnumerator::IsAudioMoniker()
	{
		return (curAudioDevice != NULL);
	}

	// Is current video device moniker not NULL?
	bool CDeviceEnumerator::IsVideoMoniker()
	{
		return (curVideoDevice !=  NULL);
	}

	// Is current screen device moniker not NULL?
	bool CDeviceEnumerator::IsScreenMoniker()
	{
		return (UScreenCaptureDevice != NULL);
	}

	// Bind to video capture filter
	void CDeviceEnumerator::BindVideoToObject(void** pVCap)
	{
		curVideoDevice->BindToObject(0, 0, IID_IBaseFilter, pVCap);
		//UScreenCaptureDevice->BindToObject(0, 0, IID_IBaseFilter, pVCap);
	}

	// Bind to audio capture filter
	void CDeviceEnumerator::BindAudioToObject(void** pACap)
	{
		curAudioDevice->BindToObject(0, 0, IID_IBaseFilter, pACap);
	}

	// Bind to screen capture filter
	void CDeviceEnumerator::BindScreenToObject(void** pSCap)
	{
		*pSCap = UScreenCaptureDevice;
		UScreenCaptureDevice->AddRef();
	}

	// Get video devices count
	unsigned short CDeviceEnumerator::GetVideoDevicesCount() const
	{
		return VideoDeviceNum;
	}

	// Get audio devices count
	unsigned short CDeviceEnumerator::GetAudioDevicesCount() const
	{
		return AudioDeviceNum;
	}

	// Get availability of format dialog into current device
	bool CDeviceEnumerator::GetFormatDialog() const
	{
		return IsFormatDialog;
	}

	// Get availability of source dialog into current device
	bool CDeviceEnumerator::GetSourceDialog() const
	{
		return IsSourceDialog;
	}

	// Get current video device name
	wchar_t * CDeviceEnumerator::GetVideoDeviceName()
	{
		if (VideoDeviceName[curVideoDeviceNum] == NULL)
			return NULL;
		wchar_t * tmp = new wchar_t[wcslen(VideoDeviceName[curVideoDeviceNum])+1];
		wcscpy(tmp, VideoDeviceName[curVideoDeviceNum]);
		return tmp;
	}

	// Get current audio device name
	wchar_t * CDeviceEnumerator::GetAudioDeviceName()
	{
		if (AudioDeviceName[curAudioDeviceNum] == NULL)
			return NULL;
		wchar_t * tmp = new wchar_t[wcslen(AudioDeviceName[curAudioDeviceNum])+1];
		wcscpy(tmp, AudioDeviceName[curAudioDeviceNum]);
		return tmp;
	}

	// Initialize dialogs interface
	void CDeviceEnumerator::InitDialogInterface(ICaptureGraphBuilder2 * pBuilder, IBaseFilter * pVCAP, IBaseFilter * pACAP)
	{
		SAFE_RELEASE(pDlg);
		HRESULT hr = S_OK;
		pVCap = pVCAP;
		if (pVCap)
		{
			hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pVCap, 
											IID_IAMVfwCaptureDialogs, (void **)&pDlg);

			if (hr == S_OK)
			{
				IsFormatDialog = (pDlg && SUCCEEDED(pDlg->HasDialog(VfwCaptureDialog_Format)));
				IsSourceDialog = (pDlg && SUCCEEDED(pDlg->HasDialog(VfwCaptureDialog_Source)));
			}
			else
			{
				ISpecifyPropertyPages *pSpec;
				CAUUID cauuid;

				// The video capture filter itself

				hr = pVCap->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
				if(hr == S_OK)
				{
					hr = pSpec->GetPages(&cauuid);
					if(hr == S_OK && cauuid.cElems > 0)
					{
						IsSourceDialog = true;
						CoTaskMemFree(cauuid.pElems);
					}
					pSpec->Release();
					pSpec = NULL;
				}
				else
					IsSourceDialog = false;
				
				hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pVCap,
											IID_IAMStreamConfig, (void **)&pSC);

                if(hr != NOERROR)
                    hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pVCap,
													IID_IAMStreamConfig, (void **)&pSC);
				if (pSC)
				{
					hr = pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
					if(hr == S_OK)
					{
						hr = pSpec->GetPages(&cauuid);
						if(hr == S_OK && cauuid.cElems > 0)
						{
							IsFormatDialog = true;
							CoTaskMemFree(cauuid.pElems);
						}
						pSpec->Release();
					}
					else
						IsFormatDialog = false;
				}
				else
					IsFormatDialog = false;
			}
		}
		else
		{
			IsFormatDialog = false;
			IsSourceDialog = false;
		}

		pACap = pACAP;

		if(pACap != NULL)
		{
			ISpecifyPropertyPages *pSpec;
			CAUUID cauuid;

			hr = pACap->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
			if(hr == S_OK)
			{
				hr = pSpec->GetPages(&cauuid);
				if(hr == S_OK && cauuid.cElems > 0)
				{
					IsAudioDialog = true;
					CoTaskMemFree(cauuid.pElems);
				}
				pSpec->Release();
			}
		}
		else
			IsAudioDialog = false;
	}

	// Set main window handler
	void CDeviceEnumerator::SetMainWindow(HWND hWnd)
	{
		hMainWnd = hWnd;
	}

	// Show format dialog
	void CDeviceEnumerator::ShowFromatDialog()
	{
		if (IsFormatDialog)
		{
			if (pDlg)
			{
				pDlg->ShowDialog(VfwCaptureDialog_Format, NULL);
			}
			/*else
			{
				ISpecifyPropertyPages *pSpec;
                CAUUID cauuid;

				if (pSC)
				{
					HRESULT hr = pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);

					if(hr == S_OK)
					{
						hr = pSpec->GetPages(&cauuid);
						hr = OleCreatePropertyFrame(hMainWnd, 30, 30, NULL, 1,(IUnknown **)&pSC, cauuid.cElems,
													(GUID *)cauuid.pElems, 0, 0, NULL);
						CoTaskMemFree(cauuid.pElems);
						pSpec->Release();
					}
				}
			}*/
		}
	}

	// Show source dialog
	void CDeviceEnumerator::ShowSourceDialog()
	{
		if (IsSourceDialog)
		{
			if (pDlg)
			{
				pDlg->ShowDialog(VfwCaptureDialog_Source, NULL);
			}
			else
			{
				CAUUID cauuid;
				ISpecifyPropertyPages	*pSpec;
				HRESULT hr = pVCap->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
				
				if(hr == S_OK)
				{
					hr = pSpec->GetPages(&cauuid);

					hr = OleCreatePropertyFrame(hMainWnd, 30, 30, NULL, 1,(IUnknown **)&pVCap, cauuid.cElems,
												(GUID *)cauuid.pElems, 0, 0, NULL);
					CoTaskMemFree(cauuid.pElems);
					pSpec->Release();
				}
			}
		}
	}

	// Show audio dialog
	void CDeviceEnumerator::ShowAudioDialog()
	{
		if (IsAudioDialog)
		{
			CAUUID cauuid;
			ISpecifyPropertyPages	*pSpec;
			HRESULT hr = pACap->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
			
			if(hr == S_OK)
			{
				hr = pSpec->GetPages(&cauuid);

				hr = OleCreatePropertyFrame(hMainWnd, 30, 30, NULL, 1,(IUnknown **)&pACap, cauuid.cElems,
											(GUID *)cauuid.pElems, 0, 0, NULL);

				CoTaskMemFree(cauuid.pElems);
				pSpec->Release();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//							Protected methods							//
	//////////////////////////////////////////////////////////////////////////
	// Get all installed audio devices
	void CDeviceEnumerator::GetAllAudioCapturedDevices()
	{
		for(int i = 0; i < NUMELMS(AudioDevices); i++)
		{
			if (AudioDevices[i]!=NULL)
			{
				AudioDevices[i]->Release();
				AudioDevices[i] = NULL;
				delete[] AudioDeviceName[i];
				AudioDeviceName[i] = NULL;
			}
		}
		curAudioDevice = NULL;

		// Enumerate all audio capture devices

		ICreateDevEnum *pCreateDevEnum=0;
		HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, 
										IID_ICreateDevEnum, (void**)&pCreateDevEnum);
		if(hr != NOERROR)
		{
			//CError::ErrMsg(hMainWnd, TEXT("Error Creating Device Enumerator"));
			return;
		}

		IEnumMoniker *pEm=0;
		hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEm, 0);
		pCreateDevEnum->Release();
		if(hr != NOERROR)
			return;

		pEm->Reset();
		ULONG cFetched;
		IMoniker *pM;

		unsigned short iAud = 0;

		while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK && iAud<10)
		{
			IPropertyBag *pBag;
			hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
			if(SUCCEEDED(hr))
			{
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read(L"FriendlyName", &var, NULL);
				if(hr == NOERROR)
				{
					AudioDeviceName[iAud] = new wchar_t[wcslen(var.bstrVal)+1];
					wcscpy(AudioDeviceName[iAud], var.bstrVal);
					SysFreeString(var.bstrVal);
					AudioDevices[iAud] = pM;
					pM->AddRef();                
					iAud++;
				}
				pBag->Release();
			}
			pM->Release();
		}

		if (AudioDeviceNum != iAud)
		{
			AudioDeviceNum = iAud;
			curAudioDeviceNum = 0;
		}
		if ( AudioDeviceNum > 0 )
		{
			//curAudioDeviceNum = 0;
			curAudioDevice = AudioDevices[curAudioDeviceNum];
		}

		pEm->Release();
	}

	// Get all installed video devices
	void CDeviceEnumerator::GetAllVideoCapturedDevices()
	{
		// Release all monikers
		for(int i = 0; i < NUMELMS(VideoDevices); i++)
		{
			if (VideoDevices[i]!=NULL)
			{
				VideoDevices[i]->Release();
				VideoDevices[i] = NULL;
				delete[] VideoDeviceName[i];
				VideoDeviceName[i] = NULL;
			}
		}
		curVideoDevice = NULL;

		// Enumerate all video capture devices
		ICreateDevEnum *pCreateDevEnum=0;
		HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
		if(hr != NOERROR)
		{
			//CError::ErrMsg(hMainWnd, TEXT("Error Creating Device Enumerator"));
			return;
		}

		IEnumMoniker *pEm=0;
		hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
		pCreateDevEnum->Release();
		if(hr != NOERROR)
		{
			//CError::ErrMsg(hMainWnd, TEXT("Sorry, you have no video capture hardware.\r\n\r\n"), TEXT("Video capture will not function properly."));
			// VideoDeviceNum = 0;
			if ( VideoDeviceNum > 0 )
				curVideoDeviceNum = 0;
			return;
		}

		// Going to beginning of enumeration
		pEm->Reset();
		ULONG cFetched;
		IMoniker *pM;

		unsigned char iVid = 0;

		while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK && iVid<10)
		{
			IPropertyBag *pBag=0;

			hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
			if(SUCCEEDED(hr))
			{
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read(L"FriendlyName", &var, NULL);
				if(hr == NOERROR)
				{
					if (wcscmp(var.bstrVal,L"UScreenCapture")!=0)       //  Anything, but known UScreenCapture should be in our list.
					{
						VideoDeviceName[iVid] = new wchar_t[wcslen(var.bstrVal)+1];
						wcscpy(VideoDeviceName[iVid], var.bstrVal);
						VideoDevices[iVid] = pM;
						iVid++;
					}

					SysFreeString(var.bstrVal);

					pM->AddRef();
				}
				pBag->Release();
			}

			pM->Release();
		}
		pEm->Release();

		//	Create know screen capture filter instance.
		hr = CoCreateInstance(CLSID_UScreenCapture, 0, CLSCTX_INPROC, IID_IBaseFilter, (void **)&UScreenCaptureDevice);

		if (VideoDeviceNum != iVid)
		{
			VideoDeviceNum = iVid;
			curVideoDeviceNum = 0;
		}
		if ( VideoDeviceNum > 0 )
		{
			curVideoDevice = VideoDevices[curVideoDeviceNum];
		}
	}
}
