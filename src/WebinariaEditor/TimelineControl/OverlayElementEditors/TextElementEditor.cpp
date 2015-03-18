#include "StdAfx.h"
#include "TextElementEditor.h"

#include <math.h>

#include "resource.h"

TextElementEditor::TextElementEditor(HWND parent, OverlayElement *element)
	: OverlayElementEditor(parent, IDD_TEXT_ELEMENT_EDITOR, element)
{
	CreateEditorWindow();
}

TextElementEditor::~TextElementEditor()
{
}

int CALLBACK TextElementEditor::s_FontEnumerator(CONST LOGFONT *lplf, CONST TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData)
{
	if (0 != (dwType & TRUETYPE_FONTTYPE))
	{
		HWND fontsCombo = reinterpret_cast<HWND>(lpData);
		SendMessage(fontsCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(lplf->lfFaceName));
	}

	return TRUE;
}

BOOL CALLBACK TextElementEditor::DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_HSCROLL == message)
	{
		ChangeFontSize();
		return FALSE;
	}

	return OverlayElementEditor::DialogProc(hwndDlg, message, wParam, lParam);
}

BOOL TextElementEditor::WMInitDialog(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//	Element text.
	SetDlgItemText(Self(), IDC_TEXT_ELEMENT_TEXT, MyElement()->Text);

	//	Element text alignment.
	CheckDlgButton(Self(), IDC_TEXT_ELEMENT_ALIGN_LEFT, 
		(taLeft == MyElement()->Alignment) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(Self(), IDC_TEXT_ELEMENT_ALIGN_CENTER, 
		(taCenter == MyElement()->Alignment) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(Self(), IDC_TEXT_ELEMENT_ALIGN_RIGHT, 
		(taRight == MyElement()->Alignment) ? BST_CHECKED : BST_UNCHECKED);
	UpdateTextAlignment();

	//	Element font.
	HWND fontSelector = GetDlgItem(Self(), IDC_TEXT_ELEMENT_FONT_NAME);
	HDC dc = GetDC(0);
	EnumFonts(dc, 0, &s_FontEnumerator, (LPARAM)fontSelector);
	ReleaseDC(0, dc);
	SendMessage(fontSelector, CB_SELECTSTRING, -1, reinterpret_cast<LPARAM>(MyElement()->Font));

	//	Font size.
	HWND sizeSelector = GetDlgItem(Self(), IDC_TEXT_ELEMENT_FONT_SIZE);
	SendMessage(sizeSelector, TBM_SETRANGE, FALSE, static_cast<LPARAM>(MAKELONG(1, 20)));
	SendMessage(sizeSelector, TBM_SETPOS, TRUE, static_cast<LPARAM>((long)ceil(MyElement()->Size * 10)));

	return TRUE;
}

BOOL TextElementEditor::WMCommand(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!initialized)
		return FALSE;

	WORD notification = HIWORD(wParam);
	WORD controlId = LOWORD(wParam);

	switch (controlId)
	{
	case IDC_TEXT_ELEMENT_TEXT:
		if (EN_CHANGE == notification)
			TextChanged();
		break;

	case IDC_TEXT_ELEMENT_ALIGN_LEFT:
	case IDC_TEXT_ELEMENT_ALIGN_CENTER:
	case IDC_TEXT_ELEMENT_ALIGN_RIGHT:
		if (BN_CLICKED == notification)
			TextAlignmentControlClicked(controlId);
		break;

	case IDC_TEXT_ELEMENT_FONT_NAME:
		if (CBN_SELCHANGE == notification)
			ChangeFont();
		break;

	case IDC_TEXT_ELEMENT_COLOR:
		if (BN_CLICKED == notification)
			ChangeTextColor();
		break;
	}

	return FALSE;
}

void TextElementEditor::TextChanged()
{
	int len = GetWindowTextLength(GetDlgItem(Self(), IDC_TEXT_ELEMENT_TEXT)) + 1;
	TCHAR *text = (TCHAR *)calloc(len, sizeof(TCHAR));
	GetDlgItemText(Self(), IDC_TEXT_ELEMENT_TEXT, text, len);
	MyElement()->Text = text;
	free(text);

	NotifyElementChanged();
}

void TextElementEditor::ChangeFont()
{
	HWND fontSelector = GetDlgItem(Self(), IDC_TEXT_ELEMENT_FONT_NAME);

	int currentFont = SendMessage(fontSelector, CB_GETCURSEL, 0, 0);
	if (CB_ERR == currentFont)
		return;

	int len = SendMessage(fontSelector, CB_GETLBTEXTLEN, currentFont, 0);
	if (CB_ERR == len)
		return;

	TCHAR *font = (TCHAR *)calloc(len + 1, sizeof(TCHAR));
	SendMessage(fontSelector, CB_GETLBTEXT, currentFont, reinterpret_cast<LPARAM>(font));
	MyElement()->Font = font;
	free(font);
}

void TextElementEditor::ChangeFontSize()
{
	HWND sizeSelector = GetDlgItem(Self(), IDC_TEXT_ELEMENT_FONT_SIZE);
	MyElement()->Size = (double)SendMessage(sizeSelector, TBM_GETPOS, 0, 0) / 10.0;

	NotifyElementChanged();
}

void TextElementEditor::ChangeTextColor()
{
	static COLORREF customColors[16] = { 0 };

	CHOOSECOLOR colorInfo = { 0 };
	colorInfo.lStructSize = sizeof(colorInfo);
	colorInfo.hwndOwner = Self();
	colorInfo.rgbResult = MyElement()->ForeColor;
	colorInfo.Flags = CC_ANYCOLOR | CC_RGBINIT | CC_FULLOPEN;
	colorInfo.lpCustColors = customColors;

	if (ChooseColor(&colorInfo))
	{
		MyElement()->ForeColor = colorInfo.rgbResult;

		NotifyElementChanged();
	}
}

void TextElementEditor::TextAlignmentControlClicked(UINT controlId)
{
	bool checked = (BST_CHECKED == IsDlgButtonChecked(Self(), controlId));

	if (checked)
	{
		switch (controlId)
		{
		case IDC_TEXT_ELEMENT_ALIGN_LEFT:
			MyElement()->Alignment = taLeft;
			break;

		case IDC_TEXT_ELEMENT_ALIGN_CENTER:
			MyElement()->Alignment = taCenter;
			break;

		case IDC_TEXT_ELEMENT_ALIGN_RIGHT:
			MyElement()->Alignment = taRight;
			break;
		}

		UpdateTextAlignment();

		NotifyElementChanged();
	}
}

void TextElementEditor::UpdateTextAlignment()
{
	HWND elementText = GetDlgItem(Self(), IDC_TEXT_ELEMENT_TEXT);

	DWORD style = GetWindowLong(elementText, GWL_STYLE);
	style &= ~(ES_LEFT | ES_RIGHT | ES_CENTER);

	switch (MyElement()->Alignment)
	{
	case taLeft:
		style |= ES_LEFT;
		break;

	case taCenter:
		style |= ES_CENTER;
		break;

	case taRight:
		style |= ES_RIGHT;
		break;
	}

	SetWindowLong(elementText, GWL_STYLE, style);
	InvalidateRect(elementText, 0, TRUE);
}
