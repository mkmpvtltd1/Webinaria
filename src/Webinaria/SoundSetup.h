#pragma once

class SoundSetup
{

private:
	HWND hwnd;

	int m_iMuxerIdx;
	int volume;
	int	minVolume, maxVolume;
	int	meterVolume;
	bool mute;
	MMRESULT m_mmr;
	HMIXER mixer;
	HWAVEIN	waveInHandle;
	WAVEHDR	waveHeader[2];    
	WAVEFORMATEX waveFormat;

	void Initialize();
	void Destroy();

	void UpdateVolumeData();
	void UpdateMuteState();
	void SetupInputDevice();
	void SetupWaveIn();

	void SetVolume();
	void SetMute();

protected:
	SoundSetup(UINT uMixerId, MMRESULT& mmr);

	BOOL CALLBACK SoundSetupDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK s_SoundSetupDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static ULONG GetMixerId(TCHAR* ptcDeviceName);
public:
	~SoundSetup();

	static HRESULT Show(HWND parent, TCHAR* ptcDeviceName);
	static bool HasMixer(TCHAR* ptcDeviceName);

};
