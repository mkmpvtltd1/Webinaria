#pragma once

#include <string>

#include "FileStream.h"
#include "OverlayElement.h"

//	NOTE: all textual data used by TextElement is in Unicode.

enum TextAlignment
{
	taLeft = 1,
	taCenter,
	taRight
};

enum FontStyle
{
	fsNormal = 0,
	fsBold = 1,
	fsItalic = 2,
	fsUnderline = 4
};

class TextElement : public OverlayElement
{

private:
	static const int Version;								//	Data record version in file.

private:
	static const int baseSize;
	double size;											//	Base size multiplier: 0.0...1.0...2.0.
	TextAlignment alignment;
	int fontStyle;
	std::wstring font;										//	Font family.
	std::wstring text;

public:
	TextElement();
	~TextElement();

	virtual ElementType GetElementType() const
	{
		return ElementType_Text;
	}

	virtual bool Load(FileStream &in);
	virtual bool Save(FileStream &out);

public:
	virtual void ConfigureDefault(const RECT *overlayRect);
	virtual void Paint(HDC dc, const RECT *overlayRect);

	void FillLogFont(LOGFONT *lf);

public:
	double GetSize() const;
	void SetSize(double value);
	const wchar_t* GetText() const;
	void SetText(const wchar_t *value);
	const wchar_t* GetFont() const;
	void SetFont(const wchar_t *value);
	TextAlignment GetAlignment() const;
	void SetAlignment(TextAlignment value);
	int GetFontStyle() const;
	void SetFontStyle(int value);

public:
	__declspec(property(get = GetSize, put = SetSize)) double Size;
	__declspec(property(get = GetText, put = SetText)) const wchar_t* Text;
	__declspec(property(get = GetFont, put = SetFont)) const wchar_t* Font;
	__declspec(property(get = GetAlignment, put = SetAlignment)) TextAlignment Alignment;
	__declspec(property(get = GetFontStyle, put = SetFontStyle)) int FontStyle;

};
