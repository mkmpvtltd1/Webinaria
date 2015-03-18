#include "StdAfx.h"
#include "Grabber.h"

using namespace WebinariaApplication::WebinariaLogical;
//////////////////////////////////////////////////////////////////////////
//								Public methods							//
//////////////////////////////////////////////////////////////////////////
// Default constructor
CGrabber::CGrabber(HWND hWnd):	AllocateBytes(0),
								IsCapturing(false),
								File(NULL),
								hMainWnd(hWnd),
								pME(NULL),
								pDF(NULL),
								pVW(NULL),
								pRender(NULL),
								pSink(NULL),
								//////////////////////
								ghDevNotify(0),
								gpUnregisterDeviceNotification(0),
								gpRegisterDeviceNotification(0),
								g_dwGraphRegister(0)
								//////////////////////
{
	/*File = new wchar_t[10];
	WIN32_FIND_DATA fd;
	HANDLE hr = FindFirstFile("tmp",&fd);
	FindClose(hr);
	if (hr  ==  INVALID_HANDLE_VALUE)
		CreateDirectory("tmp",NULL);
	else
	{
		hr = FindFirstFile("tmp\\~.avi",&fd); 
		if ( hr != INVALID_HANDLE_VALUE)
			DeleteFile("tmp\\~.avi");
		FindClose(hr);
	}
	wcscpy(File, L"tmp\\~.avi");*/
	//\\tmp

	// Register for device add/remove notifications
	DEV_BROADCAST_DEVICEINTERFACE filterData;
	ZeroMemory(&filterData, sizeof(DEV_BROADCAST_DEVICEINTERFACE));

	filterData.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	filterData.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filterData.dbcc_classguid = AM_KSCATEGORY_CAPTURE;

	gpUnregisterDeviceNotification = NULL;
	gpRegisterDeviceNotification = NULL;
	// dynload device removal APIs
	{
		HMODULE hmodUser = GetModuleHandle(TEXT("user32.dll"));
		//ASSERT(hmodUser);       // we link to user32
		gpUnregisterDeviceNotification = (PUnregisterDeviceNotification)
		GetProcAddress(hmodUser, "UnregisterDeviceNotification");

		// m_pRegisterDeviceNotification is prototyped differently in unicode
		gpRegisterDeviceNotification = (PRegisterDeviceNotification)
			GetProcAddress(hmodUser,
#ifdef UNICODE
			"RegisterDeviceNotificationW"
#else
			"RegisterDeviceNotificationA"
#endif
		);
        
		// failures expected on older platforms.
		/*ASSERT(gpRegisterDeviceNotification && gpUnregisterDeviceNotification ||
			  !gpRegisterDeviceNotification && !gpUnregisterDeviceNotification);*/
	}

	ghDevNotify = NULL;

	if(gpRegisterDeviceNotification)
	{
		ghDevNotify = gpRegisterDeviceNotification(hMainWnd, &filterData, DEVICE_NOTIFY_WINDOW_HANDLE);
		//ASSERT(ghDevNotify != NULL);
	}

}

// Virtual destructor
CGrabber::~CGrabber(void)
{
	if (File != NULL)
		delete[] File;
	IsCapturing = false;
	AllocateBytes = 0;
	SAFE_RELEASE(pSink);
	SAFE_RELEASE(pME);
	SAFE_RELEASE(pDF);
	SAFE_RELEASE(pVW);
	SAFE_RELEASE(pRender);
}

// Get interface for provide media events
IMediaEventEx * CGrabber::GetMediaEventInterface()
{
	return pME;
}

// Return full path to current file for captured data
wchar_t * CGrabber::GetSelectFile() const
{
	wchar_t * tmp = new wchar_t[wcslen(File)+1];
	wcscpy(tmp,File);
	return tmp;
}


// Update and retrive current capturing file size
unsigned long long CGrabber::GetCapFileSize()
{
	HANDLE hFile = CreateFileW(	File,
								GENERIC_READ,
								FILE_SHARE_READ,
								0,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								0 );

	if(hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	unsigned long SizeHigh;
	unsigned long SizeLow = GetFileSize(hFile, &SizeHigh);

	CapFileSize = SizeLow + ((unsigned long long)SizeHigh << 32);

	if(!CloseHandle(hFile))
	{
		CapFileSize = 0;
	}

	return CapFileSize;
}

// Function to Measure Available Disk Space
unsigned long long CGrabber::GetFreeDiskSpaceInBytes()
{
	DWORD dwFreeClusters, dwBytesPerSector, dwSectorsPerCluster, dwClusters;
	wchar_t RootName[MAX_PATH];
	LPWSTR ptmp=0;							// Required argument
	ULARGE_INTEGER ulA, ulB, ulFreeBytes;

	// Need to find path for root directory on drive containing this file.
	GetFullPathNameW( File, NUMELMS(RootName), RootName, &ptmp);

	// Truncate this to the name of the root directory
	if(RootName[0] == '\\' && RootName[1] == '\\')
	{
		// Path begins with  \\server\share\path so skip the first three backslashes
		ptmp = &RootName[2];
		while(*ptmp && (*ptmp != '\\'))
		{
			ptmp++;
		}
		if(*ptmp)
		{
			// Advance past the third backslash
			ptmp++;
		}
	}
	else
	{
		// Path must be drv:\path
		ptmp = RootName;
	}

	// Find next backslash and put a null after it
	while(*ptmp && (*ptmp != '\\'))
	{
		ptmp++;
	}

	// Found a backslash ?
	if(*ptmp)
	{
		// Skip it and insert null
		ptmp++;
		*ptmp = '\0';
	}

	// The only real way of finding out free disk space is calling
	// GetDiskFreeSpaceExA, but it doesn't exist on Win95

	HINSTANCE h = LoadLibrary(TEXT("kernel32.dll\0"));
	if(h)
	{
		typedef BOOL(WINAPI *ExtFunc)(LPCWSTR RootName, PULARGE_INTEGER pulA, PULARGE_INTEGER pulB, PULARGE_INTEGER pulFreeBytes);

		ExtFunc pfnGetDiskFreeSpaceExW = (ExtFunc)GetProcAddress(h, "GetDiskFreeSpaceExW");
		FreeLibrary(h);

		if(pfnGetDiskFreeSpaceExW)
		{
			if(!pfnGetDiskFreeSpaceExW(RootName, &ulA, &ulB, &ulFreeBytes))
				return -1;
			else
				return (ulFreeBytes.QuadPart);
		}
	}

	if(!GetDiskFreeSpaceW(RootName, &dwSectorsPerCluster, &dwBytesPerSector, &dwFreeClusters, &dwClusters))
		return (-1);
	else
		return(dwSectorsPerCluster * dwBytesPerSector * dwFreeClusters);
}

// Function for select file for saving captured information
bool CGrabber::SelectFile()
{
	USES_CONVERSION;
	if(OpenFileDialog())
	{
		OFSTRUCT os;

		// We have a capture file name

		// If this is a new file, then invite the user to allocate some space
		if(OpenFile(W2A(File), &os, OF_EXIST) == HFILE_ERROR)
		{
			//nothing
		}
	}
	else
		return false;

	if(pSink)
	{
		HRESULT hr = pSink->SetFileName(File, NULL);
	}
	return true;
}

