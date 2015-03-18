#include "StdAfx.h"
#include "TextElement.h"

#include <strsafe.h>
#include <math.h>

const int TextElement::baseSize = 36;

const int TextElement::Version = 1;

TextElement::TextElement()
	: OverlayElement()
	, size(1.0)
	, alignment(taCenter)
	, fontStyle(fsNormal)
{
	Font = L"Arial";
}

TextElement::~TextElement()
{
}

void TextElement::ConfigureDefault(const RECT *overlayRect)
{
	Text = L"Your Text Here";
	ForeColor = RGB(0xff, 0x0, 0x0);
}

void TextElement::FillLogFont(LOGFONT *lf)
{
	HDC dc = GetDC(0);

	lf->lfHeight = -MulDiv((int)floor((double)baseSize * size), GetDeviceCaps(dc, LOGPIXELSY), 72);
	lf->lfWeight = (0 != (FontStyle & fsBold)) ? FW_BOLD : FW_NORMAL;
	lf->lfItalic = (0 != (FontStyle & fsItalic));
	lf->lfUnderline = (0 != (FontStyle & fsUnderline));
	lf->lfQuality = CLEARTYPE_QUALITY;
	StringCchCopyN(lf->lfFaceName, LF_FACESIZE, Font, min(LF_FACESIZE, font.length()));

	ReleaseDC(0, dc);
}

void TextElement::Paint(HDC dc, const RECT *overlayRect)
{
	RECT rect;
	GetRect(overlayRect, &rect);

	//	Create font.
	LOGFONT lf = { 0 };
	FillLogFont(&lf);

	HFONT font = CreateFontIndirect(&lf);
	HGDIOBJ oldFont = SelectObject(dc, font);
	COLORREF oldFore = SetTextColor(dc, ForeColor);
	int oldBkMode = SetBkMode(dc, TRANSPARENT);

	//	Draw text.
	UINT format = DT_NOPREFIX | DT_VCENTER | DT_WORDBREAK;
	if (taRight == Alignment)
		format |= DT_RIGHT;
	else if (taCenter == Alignment)
		format |= DT_CENTER;
	else
		format |= DT_LEFT;
	RECT rc;
	GetRect(overlayRect, &rc);
	DrawTextW(dc, Text, text.length(), &rc, format);

	SetTextColor(dc, oldFore);
	SetBkMode(dc, oldBkMode);
	SelectObject(dc, oldFont);
	DeleteObject(font);
}

double TextElement::GetSize() const
{
	return size;
}

void TextElement::SetSize(double value)
{
	size = value;
	if (size < 0)
		size = 0;
	if (size > 2.0)
		size = 2.0;
}

const wchar_t* TextElement::GetText() const
{
	return text.c_str();
}

void TextElement::SetText(const wchar_t *value)
{
	text = value;
}

const wchar_t* TextElement::GetFont() const
{
	return font.c_str();
}

void TextElement::SetFont(const wchar_t *value)
{
	font = value;
}

TextAlignment TextElement::GetAlignment() const
{
	return alignment;
}

void TextElement::SetAlignment(TextAlignment value)
{
	alignment = value;
}

int TextElement::GetFontStyle() const
{
	return fontStyle;
}

void TextElement::SetFontStyle(int value)
{
	fontStyle = value;
}

bool TextElement::Load(FileStream &in)
{
	try
	{
		if (!OverlayElement::Load(in))
			return false;

		int version;
		in >> version;

		short s;

		in >> size;
		in >> s;
		Alignment = static_cast<TextAlignment>(s);
		in >> fontStyle;
		in >> font;											//	Reading wstring, not wchar_t*.
		in >> text;											//	Reading wstring, not wchar_t*.

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool TextElement::Save(FileStream &out)
{
	try
	{
		if (!OverlayElement::Save(out))
			return false;

		out << Version;

		out << Size;
		out << (short)Alignment;
		out << FontStyle;
		out << font;										//	Writing wstring, not wchar_t*.
		out << text;										//	Writing wstring, not wchar_t*.

		return true;
	}
	catch (...)
	{
		return false;
	}
}
