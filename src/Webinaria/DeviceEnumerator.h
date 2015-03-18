#pragma once
#include "Error.h"
//#include "CaptureGraphBuilder.h"

namespace WebinariaApplication
{
	// Class for searching video and audio capturing devices
	class CDeviceEnumerator
	{
	// Public methods
	public:
		// Default constructor
		CDeviceEnumerator(const HWND & hWnd);
	
		// Virtual destructor
		virtual ~CDeviceEnumerator(void);

		// Get video devices names
		void GetVideoDevices(wchar_t **& Names, unsigned short & DeviceCount, unsigned short & CurrenDeviceNum);

		// Get audio devices names
		void GetAudioDevices(wchar_t **& Names, unsigned short & DeviceCount, unsigned short & CurrenDeviceNum);

		// Set current video device by index (calculate from 0)
		void SetVideoDevice(const unsigned short & index);

		// Set current audio device by index (calculate from 0)
		void SetAudioDevice(const unsigned short & index);

		// Is current audio device moniker not NULL?
		bool IsAudioMoniker();

		// Is current video device moniker not NULL?
		bool IsVideoMoniker();

		// Is current screen device moniker not NULL?
		bool IsScreenMoniker();

		// Bind to video capture filter
		void BindVideoToObject(void** pVCap);

		// Bind to audio capture filter
		void BindAudioToObject(void** pACap);

		// Bind to screen capture filter
		void BindScreenToObject(void** pSCap);

		// Get video devices count
		unsigned short GetVideoDevicesCount() const;

		// Get audio devices count
		unsigned short GetAudioDevicesCount() const;

		// Get availability of format dialog into current device
		bool GetFormatDialog() const;

		// Get availability of source dialog into current device
		bool GetSourceDialog() const;

		// Get current video device name
		wchar_t * GetVideoDeviceName();

		// Get current audio device name
		wchar_t * GetAudioDeviceName();

		// Initialize dialogs interface
		void InitDialogInterface(	ICaptureGraphBuilder2 * pBuilder, IBaseFilter * pVCAP, IBaseFilter * pACAP);

		// Set main window handler
		void SetMainWindow(HWND hWnd);

		// Show format dialog
		void ShowFromatDialog();

		// Show source dialog
		void ShowSourceDialog();

		// Show audio dialog
		void ShowAudioDialog();

	// Protected methods
	protected:
		// Get all installed audio devices
		void GetAllAudioCapturedDevices();

		// Get all installed video devices
		void GetAllVideoCapturedDevices();

	// Protected fields
	protected:
		IMoniker *VideoDevices[10];			// All system video devices
		IMoniker *AudioDevices[10];			// All system audio devices
		IMoniker *curVideoDevice;			// Current video device
		IMoniker *curAudioDevice;			// Current audio device
		IBaseFilter *UScreenCaptureDevice;		// UScreenCapture device
		IAMVfwCaptureDialogs	* pDlg;		// Provide device capabilities own dialogs
		IBaseFilter		* pACap;			// Pointer to external audio capture filter
		IBaseFilter		* pVCap;			// Pointer to external video capture filter
		IAMStreamConfig * pSC;				// Pointer to external video capabilities filter
		bool	IsFormatDialog;				// If current video device has Format Dialog
		bool	IsSourceDialog;				// If current video device has Source Dialog
		bool	IsAudioDialog;				// If current audio device has Settings Dialog
		HWND	hMainWnd;					// Main window handler
		unsigned short curVideoDeviceNum;	// Current video device number
		unsigned short VideoDeviceNum;		// Video device number
		unsigned short curAudioDeviceNum;	// Current audio device number
		unsigned short AudioDeviceNum;		// Audio device number
		wchar_t *VideoDeviceName[10];		// All system video devices names
		wchar_t *AudioDeviceName[10];		// All system audio devices names
	};
}