#include "StdAfx.h"

#include "FileQualitySelector.h"
#include "utils.h"

const TCHAR* qualityItems[] = { _T("Low (200 kbps)"), _T("Medium (400 kbps)"), _T("High (700 kbps)") };

FileQualitySelector::FileQualitySelector(HWND parent)
	: m_Parent(parent)
	, m_SelectedQuality(FileQualityMedium)
{
}

FileQualitySelector::~FileQualitySelector()
{
}

BOOL CALLBACK FileQualitySelector::s_DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_INITDIALOG == message)
		SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);

	FileQualitySelector *dlg = (FileQualitySelector *)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	if (dlg)
		return dlg->DialogProc(hwndDlg, message, wParam, lParam);

	return FALSE;
}

BOOL CALLBACK FileQualitySelector::DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			CenterWindow(hwndDlg);

			//	Populate quality selector.
			for (int i = 0; i < _countof(qualityItems); i++)
				SendDlgItemMessage(hwndDlg, IDC_QUALITY, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)qualityItems[i]);

			//	Select 'Medium' quality by default.
			SendDlgItemMessage(hwndDlg, IDC_QUALITY, CB_SETCURSEL, (WPARAM)FileQualityMedium, 0);
		}
		return TRUE;

	case WM_COMMAND:
		{
			int id = LOWORD(wParam);

			if (IDOK == id || IDCANCEL == id)
			{
				if (IDOK == id)
					m_SelectedQuality = (FileQuality)SendDlgItemMessage(hwndDlg, IDC_QUALITY, CB_GETCURSEL, 0, 0);

				EndDialog(hwndDlg, id);
			}

			return TRUE;
		}
	}

	return FALSE;
}

bool FileQualitySelector::Show()
{
	return (IDOK == 
		DialogBoxParam(GetWindowInstance(m_Parent), MAKEINTRESOURCE(IDD_CONVERSION_QUALITY), m_Parent, s_DialogProc, (LPARAM)this));
}
