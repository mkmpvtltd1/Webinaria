#pragma once

#include "TextElement.h"
#include "OverlayElementEditor.h"

class TextElementEditor : public OverlayElementEditor
{

private:
	static int CALLBACK s_FontEnumerator(CONST LOGFONT *lplf, CONST TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData);

private:
	void UpdateTextAlignment();
	void TextAlignmentControlClicked(UINT controlId);

	void TextChanged();

	void ChangeFont();
	void ChangeFontSize();
	void ChangeTextColor();

	TextElement* MyElement() { return reinterpret_cast<TextElement*>(Element()); }

protected:
	virtual BOOL CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

	virtual BOOL WMInitDialog(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL WMCommand(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	TextElementEditor(HWND parent, OverlayElement *element);
	virtual ~TextElementEditor();

};
