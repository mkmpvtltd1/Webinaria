#include "StdAfx.h"
#include "SoundSetup.h"

#include "resource.h"
#include "Application.h"
#include "utils.h"

SoundSetup::SoundSetup(UINT uMixerId, MMRESULT& mmr)
	: hwnd(0)
	, mixer(0)
	, waveInHandle(0)
	, volume(0)
	, minVolume(0)
	, maxVolume(0)
	, meterVolume(0)
	, mute(false)
	,m_iMuxerIdx(-1)
{
	mmr = mixerOpen(&mixer, uMixerId, 0, 0, MIXER_OBJECTF_MIXER);

	if (MMSYSERR_NOERROR != mmr)
	{
		mixer = 0;
	}
	else
	{
		SendDlgItemMessage(hwnd, IDC_VOLUME, PBM_SETRANGE32, 0, 32767);
	}
}

SoundSetup::~SoundSetup()
{
	if (waveInHandle)
	{
		// Stop recording and tell the driver to unqueue/return all of our WAVEHDRs.
		// The driver will return any partially filled buffer that was currently
		// recording. Because we use waveInReset() instead of waveInStop(),
		// all of the other WAVEHDRs will also be returned via MM_WIM_DONE too
		waveInStop(waveInHandle);
		waveInReset(waveInHandle);                 
		waveInUnprepareHeader(waveInHandle, &waveHeader[0], sizeof(WAVEHDR));
		waveInUnprepareHeader(waveInHandle, &waveHeader[1], sizeof(WAVEHDR));

		if (waveHeader[0].lpData)
			VirtualFree(waveHeader[0].lpData, (waveHeader[0].dwBufferLength << 1), MEM_RELEASE);
	}

	if (mixer)
		mixerClose(mixer);
}

#pragma region Sound Controls
void SoundSetup::UpdateVolumeData()
{
	if (!mixer)
		return;

	MIXERLINECONTROLS mxlc = { 0 };
	tMIXERCONTROLDETAILS mxcd = { 0 };
	tMIXERCONTROLDETAILS_UNSIGNED vol = { 0 };
	MIXERCONTROL mxc = { 0 };
	MIXERLINE mxl = { 0 };

	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;//MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
	mxl.cbStruct = sizeof(mxl);

	if (MMSYSERR_NOERROR != mixerGetLineInfo((HMIXEROBJ)mixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE))
		return;

	int nConnections = mxl.cConnections;
	for (int i=0; i<nConnections; i++)
	{
        mxl.dwSource = i; 
        mixerGetLineInfo((HMIXEROBJ) mixer, &mxl, MIXER_GETLINEINFOF_SOURCE); 
        if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == mxl.dwComponentType) 
			break;
	}


	mxlc.cbStruct = sizeof(mxlc);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(mxc);

	mxlc.pamxctrl = &mxc;
	if (MMSYSERR_NOERROR != mixerGetLineControls((HMIXEROBJ)mixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE))
		return;

	mxcd.dwControlID = mxc.dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cMultipleItems = 0;
	vol.dwValue = 0;
	mxcd.cbDetails = sizeof(vol);
	mxcd.paDetails = &vol;
	mxcd.cChannels = 1;

	minVolume = mxlc.pamxctrl->Bounds.lMinimum;
	maxVolume = mxlc.pamxctrl->Bounds.lMaximum;
	
	SendDlgItemMessage(hwnd, IDC_VOLUME_CONTROL, TBM_SETRANGEMIN, TRUE, minVolume);
	SendDlgItemMessage(hwnd, IDC_VOLUME_CONTROL, TBM_SETRANGEMAX, TRUE, maxVolume);

	if (MMSYSERR_NOERROR == mixerGetControlDetails((HMIXEROBJ)mixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE))
	{
		volume = vol.dwValue;
		if (maxVolume - minVolume > 0)
			SendDlgItemMessage(hwnd, IDC_VOLUME_CONTROL, TBM_SETPOS, TRUE,  maxVolume - volume);
			//SendDlgItemMessage(hwnd, IDC_VOLUME_CONTROL, TBM_SETPOS, TRUE, (int)((float)volume /(float)(maxVolume - minVolume) * 100.0f));
	}
}

void SoundSetup::UpdateMuteState()
{
	if (!mixer)
		return;

	MIXERLINECONTROLS mxlc = { 0 };
	tMIXERCONTROLDETAILS mxcd = { 0 };
	tMIXERCONTROLDETAILS_BOOLEAN mute = { 0 };
	MIXERCONTROL mxc = { 0 };
	MIXERLINE mxl = { 0 };

	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
	mxl.cbStruct = sizeof(mxl);

	if (MMSYSERR_NOERROR != mixerGetLineInfo((HMIXEROBJ)mixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE))
		return;

	mxlc.cbStruct = sizeof(mxlc);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(mxc);

	mxlc.pamxctrl = &mxc;
	if (MMSYSERR_NOERROR != mixerGetLineControls((HMIXEROBJ)mixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE))
		return;

	mxcd.dwControlID = mxc.dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cMultipleItems = 0;
	mute.fValue = false;
	mxcd.cbDetails = sizeof(mute);
	mxcd.paDetails = &mute;
	mxcd.cChannels = 1;

	if (MMSYSERR_NOERROR == mixerGetControlDetails((HMIXEROBJ)mixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE))
	{
		this->mute = mute.fValue;
		CheckDlgButton(hwnd, IDC_MUTE_SOUND, (this->mute) ? BST_CHECKED : BST_UNCHECKED);
	}
}

//	DONE: Looks like it doesn't care about device which was chosen by user.
void SoundSetup::SetupInputDevice()
{
	if (!mixer)
		return;

	int m_iMuxerIdx = -1;

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct        = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	if (mixerGetLineInfo((HMIXEROBJ)mixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
		return;

	// get dwControlID
	MIXERCONTROL      mxc;
	MIXERLINECONTROLS mxlc;
	DWORD             dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;

	mxlc.cbStruct      = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID      = mxl.dwLineID;
	mxlc.dwControlType = dwControlType;
	mxlc.cControls     = 0;
	mxlc.cbmxctrl      = sizeof(MIXERCONTROL);
	mxlc.pamxctrl      = &mxc;
	m_mmr = mixerGetLineControls((HMIXEROBJ)mixer, &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
	if (MMSYSERR_NOERROR!=m_mmr)
	{
		// no mixer, try MUX
		dwControlType      = MIXERCONTROL_CONTROLTYPE_MUX;
		mxlc.cbStruct      = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID      = mxl.dwLineID;
		mxlc.dwControlType = dwControlType;
		mxlc.cControls     = 0;
		mxlc.cbmxctrl      = sizeof(MIXERCONTROL);
		mxlc.pamxctrl      = &mxc;
		m_mmr = mixerGetLineControls((HMIXEROBJ)mixer, &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if (MMSYSERR_NOERROR!=m_mmr )
			return;
	}

	if (mxc.cMultipleItems <= 0)
		return;

	// get the index of the inDevice from available controls
	MIXERCONTROLDETAILS_LISTTEXT*  pmxcdSelectText = new MIXERCONTROLDETAILS_LISTTEXT[mxc.cMultipleItems];

	if (pmxcdSelectText != NULL)
	{
		MIXERCONTROLDETAILS mxcd;

		mxcd.cbStruct       = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID    = mxc.dwControlID;
		mxcd.cChannels      = 1;
		mxcd.cMultipleItems = mxc.cMultipleItems;
		mxcd.cbDetails      = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
		mxcd.paDetails      = pmxcdSelectText;

		if (mixerGetControlDetails ((HMIXEROBJ)mixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_LISTTEXT) == MMSYSERR_NOERROR)
		{
			// determine which controls the inputDevice source line
			DWORD dwi;
			for (dwi = 0; dwi < mxc.cMultipleItems; dwi++)
			{
				// get the line information
				MIXERLINE mxl;
				mxl.cbStruct = sizeof(MIXERLINE);
				mxl.dwLineID = pmxcdSelectText[dwi].dwParam1;
				if (mixerGetLineInfo ((HMIXEROBJ)mixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID) == MMSYSERR_NOERROR && mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)
				{
					// found, dwi is the index.
					m_iMuxerIdx = dwi;
					break;
				}
			}
		}
		delete []pmxcdSelectText;
	}

	if (m_iMuxerIdx < 0)
		return;

	// get all the values first
	MIXERCONTROLDETAILS_BOOLEAN* pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[mxc.cMultipleItems];

	if (pmxcdSelectValue != NULL)
	{
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct       = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID    = mxc.dwControlID;
		mxcd.cChannels      = 1;
		mxcd.cMultipleItems = mxc.cMultipleItems;
		mxcd.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		mxcd.paDetails      = pmxcdSelectValue;
		m_mmr = mixerGetControlDetails((HMIXEROBJ)mixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
		if (MMSYSERR_NOERROR==m_mmr)
		{
			// ASSERT(m_dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER || m_dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);
			// MUX restricts the line selection to one source line at a time.
			if (dwControlType == MIXERCONTROL_CONTROLTYPE_MUX)
			{
				ZeroMemory(pmxcdSelectValue, mxc.cMultipleItems * sizeof(MIXERCONTROLDETAILS_BOOLEAN));
			}
			// Turn on this input device
			pmxcdSelectValue[m_iMuxerIdx].fValue = 0x1;
			m_mmr = mixerSetControlDetails ((HMIXEROBJ)mixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE);
		}
		delete []pmxcdSelectValue;
	}
}

void SoundSetup::SetupWaveIn()
{
	MMRESULT err;	               

	// Clear out both of our WAVEHDRs. At the very least, waveInPrepareHeader() expects the dwFlags field to be cleared
	ZeroMemory(&waveHeader[0], sizeof(WAVEHDR) * 2);

	// Initialize the WAVEFORMATEX for 16-bit, 44KHz, stereo. That's what I want to record 
	waveFormat.wFormatTag      = WAVE_FORMAT_PCM;
	waveFormat.nChannels       = 2;
	waveFormat.nSamplesPerSec  = 44100;
	waveFormat.wBitsPerSample  = 16;
	waveFormat.nBlockAlign     = waveFormat.nChannels * (waveFormat.wBitsPerSample/8);
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize          = 0;

	// Open the default WAVE In Device, specifying my callback. Note that if this device doesn't
	// support 16-bit, 44KHz, stereo recording, then Windows will attempt to open another device
	// that does. So don't make any assumptions about the name of the device that opens. After
	// waveInOpen returns the handle, use waveInGetID to fetch its ID, and then waveInGetDevCaps
	// to retrieve the actual name	                         
	if ((err = waveInOpen(&waveInHandle, WAVE_MAPPER, &waveFormat, (DWORD)hwnd, 0, CALLBACK_WINDOW)) != 0)
		return;

	// Allocate, prepare, and queue two buffers that the driver can use to record blocks of audio data.
	// (ie, We're using double-buffering. You can use more buffers if you'd like, and you should do that
	// if you suspect that you may lag the driver when you're writing a buffer to disk and are too slow
	// to requeue it with waveInAddBuffer. With more buffers, you can take your time requeueing
	// each).
	//
	// I'll allocate 2 buffers large enough to hold 2 seconds worth of waveform data at 44Khz. NOTE:
	// Just to make it easy, I'll use 1 call to VirtualAlloc to allocate both buffers, but you can
	// use 2 separate calls since the buffers do NOT need to be contiguous. You should do the latter if
	// using many, large buffers
	waveHeader[1].dwBufferLength = waveHeader[0].dwBufferLength = 512;
	if (!(waveHeader[0].lpData = (char*)VirtualAlloc(0, (waveHeader[0].dwBufferLength << 1), MEM_COMMIT, PAGE_READWRITE)))
	{
		waveInClose(waveInHandle);
		waveInHandle = 0;
		return;
	}

	// Fill in WAVEHDR fields for buffer starting address. We've already filled in the size fields above */
	waveHeader[1].lpData = waveHeader[0].lpData + waveHeader[0].dwBufferLength;

	// Leave other WAVEHDR fields at 0 

	// Prepare the 2 WAVEHDR's 
	if ((err = waveInPrepareHeader(waveInHandle, &waveHeader[0], sizeof(WAVEHDR))))
		return;

	if ((err = waveInPrepareHeader(waveInHandle, &waveHeader[1], sizeof(WAVEHDR))))
		return;

	// Queue first WAVEHDR (recording hasn't started yet) 
	if ((err = waveInAddBuffer(waveInHandle, &waveHeader[0], sizeof(WAVEHDR))))
		return;

	// Queue second WAVEHDR 
	if ((err = waveInAddBuffer(waveInHandle, &waveHeader[1], sizeof(WAVEHDR))))
		return;

	// Start recording
	if ((err = waveInStart(waveInHandle)))
		return;
}

void SoundSetup::SetVolume()
{
	if (!mixer)
		return;

	MIXERLINECONTROLS mxlc;
	tMIXERCONTROLDETAILS mxcd;
	tMIXERCONTROLDETAILS_UNSIGNED vol;
	MIXERCONTROL mxc;
	MIXERLINE mxl;
	int intRet;

	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;//MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
	mxl.cbStruct = sizeof(mxl);

	if (MMSYSERR_NOERROR != mixerGetLineInfo((HMIXEROBJ)mixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE))
		return;
	int nConnections = mxl.cConnections;
	for (int i=0; i<nConnections; i++)
	{
        mxl.dwSource = i; 
        mixerGetLineInfo((HMIXEROBJ) mixer, &mxl, MIXER_GETLINEINFOF_SOURCE); 
        if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == mxl.dwComponentType) 
			break;
	}

	ZeroMemory(&mxlc, sizeof(mxlc));
	mxlc.cbStruct = sizeof(mxlc);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(mxc);

	mxlc.pamxctrl = &mxc;

	if (MMSYSERR_NOERROR != mixerGetLineControls((HMIXEROBJ)mixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE))
		return;

	ZeroMemory(&mxcd, sizeof(mxcd));
	mxcd.dwControlID = mxc.dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(vol);
	mxcd.paDetails = &vol;
	mxcd.cChannels = 1;

	vol.dwValue = volume;

	mixerSetControlDetails((HMIXEROBJ)mixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);

}

void SoundSetup::SetMute()
{
	if (!mixer)
		return;

	MIXERLINECONTROLS mxlc;
	tMIXERCONTROLDETAILS mxcd;
	tMIXERCONTROLDETAILS_BOOLEAN mute;
	MIXERCONTROL mxc;
	MIXERLINE mxl;
	
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
	mxl.cbStruct = sizeof(mxl);

	if (MMSYSERR_NOERROR != mixerGetLineInfo((HMIXEROBJ)mixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE))
		return;

	ZeroMemory(&mxlc, sizeof(mxlc));
	mxlc.cbStruct = sizeof(mxlc);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(mxc);

	mxlc.pamxctrl = &mxc;

	if (MMSYSERR_NOERROR != mixerGetLineControls((HMIXEROBJ)mixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE))
		return;

	ZeroMemory(&mxcd, sizeof(mxcd));
	mxcd.dwControlID = mxc.dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(mute);
	mxcd.paDetails = &mute;
	mxcd.cChannels = 1;

	mute.fValue = this->mute;

	mixerSetControlDetails((HMIXEROBJ)mixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);
}
#pragma endregion

BOOL CALLBACK SoundSetup::SoundSetupDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			hwnd = hwndDlg;
			Initialize();

			return TRUE;
		}

	case WM_COMMAND:
		{
			int id = LOWORD(wParam);

			if (IDOK == id || IDCANCEL == id)
				EndDialog(hwndDlg, id);
			else if (IDC_MUTE_SOUND == id)
			{
				mute = (BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_MUTE_SOUND));
				SetMute();
			}

			return TRUE;
		}

	case MM_WIM_DATA:
		{
			WAVEHDR* pHeader = (WAVEHDR*) lParam;
			static int maxLeftSave = 0;
			static int maxRightSave = 0;

			if (pHeader->dwBytesRecorded > 0)
			{										
				int          idx       = 0;
				short int*   pSamples  = (short int*)pHeader->lpData;
				int          sampleCnt = pHeader->dwBytesRecorded/(sizeof(short int));

				// ******* Calculate Power Meter *******
				int lm = 0, rm = 0, maxLeft = 0, maxRight = 0;

				for (idx = 0; idx < sampleCnt; idx += 2)
				{
					if ((lm = abs(pSamples[idx]) >> 6) > maxLeft)
						maxLeft = lm;

					if ((rm = abs(pSamples[idx+1]) >> 6) > maxRight)
						maxRight = rm;
				}

				if (maxLeft < maxLeftSave)
				{
					if ((maxLeft = maxLeftSave - 4) < 0)
						maxLeft = 0;
				}

				if (maxRight < maxRightSave)
				{
					if ((maxRight = maxRightSave - 4) < 0)
						maxRight = 0;
				}

				meterVolume = (maxLeft + maxRight) / 2;

				// Set the sound meter progress bars
				SendDlgItemMessage(hwnd, IDC_VOLUME, PBM_SETPOS, meterVolume, 0);
			}

			// Yes. Now we need to requeue this buffer so the driver can use it for another block of audio
			// data. NOTE: We shouldn't need to waveInPrepareHeader() a WAVEHDR that has already been prepared once					
			waveInAddBuffer(waveInHandle, pHeader, sizeof(WAVEHDR));
		}
		break;

	case WM_VSCROLL:
		{
			//	There's only one track bar in this window, so we don't check the origin of this message.
			int v = SendDlgItemMessage(hwnd, IDC_VOLUME_CONTROL, TBM_GETPOS, 0, 0);

//			waveInStop(waveInHandle);
			if (maxVolume - minVolume > 0)
			{
				//volume = (float)v * (float)(maxVolume - minVolume) / 100.0f + minVolume;
				volume = maxVolume-v;
				SetVolume();
			}
//			waveInReset(waveInHandle);
//			waveInStart(waveInHandle);
		}
		break;

	case WM_DESTROY:
		Destroy();
		break;
	}

	return FALSE;
}

void SoundSetup::Initialize()
{
	CenterWindow(hwnd);

	//	Setup volume control.
	//SendDlgItemMessage(hwnd, IDC_VOLUME_CONTROL, TBM_SETRANGE, TRUE, MAKELONG(0, 100));

	//	Update sound controls.
	UpdateVolumeData();
	UpdateMuteState();
	SetupInputDevice();
	SetupWaveIn();
}

void SoundSetup::Destroy()
{
}

BOOL CALLBACK SoundSetup::s_SoundSetupDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_INITDIALOG == message)
		SetWindowLong(hwndDlg, GWL_USERDATA, lParam);

	SoundSetup *dlg = (SoundSetup *)GetWindowLong(hwndDlg, GWL_USERDATA);
	if (dlg)
		return dlg->SoundSetupDialogProc(hwndDlg, message, wParam, lParam);

	return FALSE;
}

#define INVALID_MIXER_ID ((DWORD) -1)

ULONG SoundSetup::GetMixerId(TCHAR* ptcDeviceName)
{
	MMRESULT mmr;
	MIXERCAPS caps;
	UINT uiDevId(INVALID_MIXER_ID);

    for(UINT ui=0; ui<mixerGetNumDevs(); ui++)
    {
        mmr = mixerGetDevCaps(ui,&caps, sizeof(caps));
        if (MMSYSERR_NOERROR == mmr)
        {
            if (!_tcscmp(_tcstrim(caps.szPname), ptcDeviceName))
            {
				uiDevId = ui;
                break;
            }
        }
    }

	return uiDevId;
}

HRESULT SoundSetup::Show(HWND parent, TCHAR* ptcDeviceName)
{
MMRESULT mmr;
	ULONG uiDevId = GetMixerId(ptcDeviceName);
	if(INVALID_MIXER_ID==uiDevId)
	{//audio device has not a mixer. It is true fo some tv-tuner where simplest audio device present
		return E_FAIL;
	}

	SoundSetup dlg(uiDevId, mmr);
	if(MMSYSERR_NOERROR != mmr)
	{
		//Error during mixer instance creation.
		return E_UNEXPECTED;
	}

	DialogBoxParam(WebinariaApplication::CApplication::Instance(), 
		MAKEINTRESOURCE(IDD_SOUND_INPUT_SETUP), parent, s_SoundSetupDialogProc, (LPARAM)&dlg);
	return S_OK;
}
bool SoundSetup::HasMixer(TCHAR* ptcDeviceName)
{
	return GetMixerId(ptcDeviceName)!=INVALID_MIXER_ID;
}