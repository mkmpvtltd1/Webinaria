#pragma once

class RegionSelector
{

private:
	RegionSelector();

	static BOOL CALLBACK RegionSelectorDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static void UpdateButtonsPosition(HWND hwndDlg);

public:
	static bool SelectRegion(HWND parent, RECT *pRegion);

};
