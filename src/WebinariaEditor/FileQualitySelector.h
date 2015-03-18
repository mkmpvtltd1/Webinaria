#pragma once

enum FileQuality
{
	FileQualityLow = 0,
	FileQualityMedium,
	FileQualityHigh
};

class FileQualitySelector
{

private:
	static BOOL CALLBACK s_DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

	HWND m_Parent;
	FileQuality m_SelectedQuality;

public:
	FileQualitySelector(HWND parent);
	~FileQualitySelector();

	bool Show();

	FileQuality SelectedQuality() const { return m_SelectedQuality; }

};
