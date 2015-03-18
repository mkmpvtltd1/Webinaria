#pragma once

#include "OverlayElement.h"

[event_source(native)]
class OverlayElementEditor
{

private:
	static BOOL CALLBACK s_DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

	HWND m_Parent, m_Self;
	OverlayElement *m_Element;
	UINT m_ResourceId;

protected:
	virtual BOOL CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

	virtual BOOL WMInitDialog(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual BOOL WMCommand(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) = 0;

	void NotifyElementChanged();

	//	Creates editor window.
	//	NOTE: descendants MUST call this method eventually (constructor is OK), otherwise nothing will work.
	void CreateEditorWindow();

	HWND Parent() const { return m_Parent; }
	HWND Self() const { return m_Self; }
	OverlayElement* Element() { return m_Element; }

	bool initialized;

public:
	OverlayElementEditor(HWND parent, UINT resourceId, OverlayElement *element);
	virtual ~OverlayElementEditor();

	virtual void Show();
	virtual void Hide();

public:
	__event void ElementChanged();

};
