#pragma once
#include "Error.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		// Device notification definitions
		#if (WINVER < 0x0500)

		#define DBT_DEVTYP_DEVICEINTERFACE      0x00000005  // device interface class
		#define DEVICE_NOTIFY_WINDOW_HANDLE     0x00000000

		typedef PVOID HDEVNOTIFY;

		#endif

		extern "C"
		{
			typedef BOOL (/* WINUSERAPI */ WINAPI *PUnregisterDeviceNotification)(
				IN HDEVNOTIFY Handle
				);

			typedef HDEVNOTIFY (/* WINUSERAPI */ WINAPI *PRegisterDeviceNotificationA)(
				IN HANDLE hRecipient,
				IN LPVOID NotificationFilter,
				IN DWORD Flags
				);

			typedef HDEVNOTIFY (/* WINUSERAPI */ WINAPI *PRegisterDeviceNotificationW)(
				IN HANDLE hRecipient,
				IN LPVOID NotificationFilter,
				IN DWORD Flags
				);
		}

		#ifdef UNICODE
		#define PRegisterDeviceNotification  PRegisterDeviceNotificationW
		#else
		#define PRegisterDeviceNotification  PRegisterDeviceNotificationA
		#endif // !UNICODE

		// Abstract class. Provide interface for grab device
		class CGrabber  
		{
		// Public methods
		public:
			// Default constructor
			CGrabber(HWND hWnd);
		
			// Virtual destructor
			virtual ~CGrabber(void);

			// Function for select file for saving captured information
			virtual bool SelectFile();

			// Return full path to current file for captured data
			// Don't alloc memory for using variable but destroy it after using
			wchar_t * GetSelectFile() const;

			// Update and retrive current capturing file size
			unsigned long long GetCapFileSize();

			// Return free space of the capturing file logical disk
			unsigned long long GetFreeDiskSpaceInBytes();


			// Abstract function for start grabbing
			virtual bool Start() = 0;

			// Abstract fuction for stop grabbing
			virtual bool Stop() = 0;

			// Abstract fuction for pause grabbing
			virtual bool Pause() = 0;

			// Abstract fuction for resume grabbing
			virtual bool Resume() = 0;

			// Tear down everything downstream of the capture filters
			virtual void TearDownGraph() = 0;
			
			// Get interface for provide media events
			IMediaEventEx * GetMediaEventInterface();

		// Protected methods
		protected:
			// Abstract function for configure open dialog
			virtual bool OpenFileDialog() = 0;
		
		// Protected fields
		protected:
			const HWND			hMainWnd;		// Main window handler
			unsigned long int	AllocateBytes;
			unsigned long long  CapFileSize;	// Capturing file size in bytes
			/* may be need to discard*/
			bool				IsCapturing;	// true		- capturing ; 
												// false	- stopped.*/
			wchar_t *			File;			// Current file for saving capturing data
			IFileSinkFilter		*pSink;			// File writer filter
			IMediaEventEx		*pME;			// Use for handling media events
			IAMDroppedFrames	*pDF;			// Dropped frames handling
			IVideoWindow		*pVW;			// Interface for handling with video rendering video
			IBaseFilter			*pRender;		// For render stream

			HDEVNOTIFY							ghDevNotify;
			PUnregisterDeviceNotification		gpUnregisterDeviceNotification;
			PRegisterDeviceNotification			gpRegisterDeviceNotification;
			DWORD								g_dwGraphRegister;

		};
	}
}