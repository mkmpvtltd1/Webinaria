#include "StdAfx.h"
#include "RegionSelector.h"

#include "Resource.h"
#include "Application.h"
#include "utils.h"

void RegionSelector::UpdateButtonsPosition(HWND hwndDlg)
{
	HWND hwndOK = GetDlgItem(hwndDlg, IDOK), hwndCancel = GetDlgItem(hwndDlg, IDCANCEL);
	if (0 == hwndOK || 0 == hwndCancel)
		return;

	const int delta = 25;

	RECT rectClient, rectOK, rectCancel;
	GetClientRect(hwndDlg, &rectClient);
	GetClientRect(hwndOK, &rectOK);
	GetClientRect(hwndCancel, &rectCancel);

	int left = (RectWidth(rectClient) - (RectWidth(rectOK) + delta + RectWidth(rectCancel))) / 2;
	int top = (RectHeight(rectClient) - RectHeight(rectOK)) / 2;

	MoveWindow(hwndOK, left, top, RectWidth(rectOK), RectHeight(rectOK), TRUE);
	MoveWindow(hwndCancel, left + RectWidth(rectOK) + delta, top, RectWidth(rectCancel), RectHeight(rectCancel), TRUE);
}

BOOL CALLBACK RegionSelector::RegionSelectorDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH bkgndBrush = 0;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			//	Save parameter in window's user data.
			SetWindowLong(hwndDlg, GWL_USERDATA, lParam);

			RECT *rect = (RECT*)lParam;
			if (rect)
				SetWindowPos(hwndDlg, 0, rect->left, rect->top, RectWidth(*rect), RectHeight(*rect), SWP_NOOWNERZORDER);

			bkgndBrush = CreateSolidBrush(RGB(0, 0, 0));

			//	Make selector window transparent.
			SetWindowLong(hwndDlg, GWL_EXSTYLE, GetWindowLong(hwndDlg, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hwndDlg, RGB(0, 0, 0), 200, LWA_ALPHA);

			UpdateButtonsPosition(hwndDlg);

			return TRUE;
		}

	case WM_DESTROY:
		DeleteObject(bkgndBrush);
		bkgndBrush = 0;
		return TRUE;

	case WM_CTLCOLORDLG:
		return (INT_PTR)bkgndBrush;

	case WM_COMMAND:
		{
			int id = LOWORD(wParam);
			if (BN_CLICKED == HIWORD(wParam) && (IDOK == id || IDCANCEL == id))
			{
				if (IDOK == id)
				{
					//	Save selected region.
					RECT *rect = (RECT*)GetWindowLong(hwndDlg, GWL_USERDATA);
					if (rect)
						GetWindowRect(hwndDlg, rect);
				}

				EndDialog(hwndDlg, id);
				return TRUE;
			}
		}

	case WM_NCHITTEST:
		{
			//	Enable client area dragging.
			LRESULT original = DefWindowProc(hwndDlg, message, wParam, lParam);

			SetWindowLong(hwndDlg, DWL_MSGRESULT, (HTCLIENT == original) ? HTCAPTION : original);
			return TRUE;
		}

	case WM_SIZING:
		{
			//	Set minimum size and make sure that size is in multiplies of four.
			RECT *rect = (RECT*)lParam;

			int width = RectWidth(*rect);
			int height = RectHeight(*rect);

			const int minWidth = 240;
			const int minHeight = 64;

			if (width < minWidth)
				rect->right = rect->left + (minWidth - width);
			if (height < minHeight)
				rect->bottom = rect->top + (minHeight - height);

			if (!(width % 4))
				rect->right = rect->left + (4 * ((width / 4) + 1));
			if (!(height % 4))
				rect->bottom = rect->top + (4 * ((height / 4) + 1));

			SetWindowLong(hwndDlg, DWL_MSGRESULT, TRUE);
			return TRUE;
		}

	case WM_SIZE:
		{
			UpdateButtonsPosition(hwndDlg);
			return TRUE;
		}
	}

	return FALSE;
}

bool RegionSelector::SelectRegion(HWND parent, RECT *pRegion)
{
	return (IDOK == DialogBoxParam(WebinariaApplication::CApplication::Instance(), MAKEINTRESOURCE(IDD_REGION_SELECTOR), parent, 
		RegionSelectorDialogProc, (LPARAM)pRegion));
}
