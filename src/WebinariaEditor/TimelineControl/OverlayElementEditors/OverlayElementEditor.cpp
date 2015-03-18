#include "StdAfx.h"
#include "OverlayElementEditor.h"

#include <assert.h>

#include "utils.h"

OverlayElementEditor::OverlayElementEditor(HWND parent, UINT resourceId, OverlayElement *element)
	: m_Parent(parent)
	, m_Element(element)
	, m_ResourceId(resourceId)
	, initialized(false)
{
	assert(parent);
	assert(resourceId);
	assert(element);
}

OverlayElementEditor::~OverlayElementEditor()
{
	if (m_Self)
		DestroyWindow(m_Self);
}

void OverlayElementEditor::CreateEditorWindow()
{
	m_Self = CreateDialogParam(GetWindowInstance(m_Parent), MAKEINTRESOURCE(m_ResourceId), m_Parent, 
		s_DialogProc, (LPARAM)this);

	if (m_Self)
	{
		RECT parentRc;
		GetWindowRect(m_Parent, &parentRc);

		RECT rc;
		GetWindowRect(m_Self, &rc);

		//	Position the editor depending on owner window position.
		RECT selfRect;
		selfRect.right = parentRc.right;
		selfRect.top = parentRc.top;
		selfRect.left = selfRect.right - RectWidth(rc);
		selfRect.bottom = selfRect.top + RectHeight(rc);
		MoveWindow(m_Self, selfRect.left, selfRect.top, RectWidth(rc), RectHeight(rc), FALSE);

		//	Make window semi-transparent.
		SetWindowLong(m_Self, GWL_EXSTYLE, GetWindowLong(m_Self, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(m_Self, RGB(0, 0, 0), 220, LWA_ALPHA);
	}
}

BOOL CALLBACK OverlayElementEditor::s_DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_INITDIALOG == message)
		SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);

	OverlayElementEditor *dlg = (OverlayElementEditor *)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	if (dlg)
		return dlg->DialogProc(hwndDlg, message, wParam, lParam);

	return FALSE;
}

BOOL CALLBACK OverlayElementEditor::DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	OverlayElementEditor *dlg = (OverlayElementEditor *)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	if (!dlg)
		return FALSE;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			m_Self = hwndDlg;
			BOOL result = dlg->WMInitDialog(hwndDlg, message, wParam, lParam);
			initialized = true;
			return result;
		}

	case WM_COMMAND:
		return dlg->WMCommand(hwndDlg, message, wParam, lParam);

	case WM_CLOSE:
		Hide();
		return TRUE;
	}

	return FALSE;
}

void OverlayElementEditor::Show()
{
	assert(m_Self);

	if (m_Self)
		ShowWindow(m_Self, SW_SHOWNOACTIVATE);
}

void OverlayElementEditor::Hide()
{
	assert(m_Self);

	if (m_Self)
		ShowWindow(m_Self, SW_HIDE);
}

void OverlayElementEditor::NotifyElementChanged()
{
	__raise ElementChanged();
}
