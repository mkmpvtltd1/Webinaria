#pragma once

#include "FileStream.h"

struct RECTF
{
	double left;
	double top;
	double width;
	double height;
};

enum ElementType
{
	ElementType_Text
};

class OverlayElement
{

private:
	static const int Version;								//	Data record version in file.

private:
	RECTF position;
	COLORREF foreColor, backColor;
	short transparency;										//	0..100% (0 -- opaque, 100% -- no background).

public:
	static const int MinimalSize;

public:
	OverlayElement();
	virtual ~OverlayElement();

	virtual bool Load(FileStream &in);
	virtual bool Save(FileStream &out);

	virtual ElementType GetElementType() const = 0;

	virtual void ConfigureDefault(const RECT *overlayRect) = 0;
	virtual void Paint(HDC dc, const RECT *overlayRect) = 0;

	void GetRect(const RECT *overlayRect, RECT *rect) const;
	void SetRect(const RECT *overlayRect, const RECT *rect);

public:
	RECTF GetPosition() const;
	void SetPosition(const RECTF *newPosition);
	COLORREF GetForeColor() const;
	void SetForeColor(COLORREF color);
	COLORREF GetBackColor() const;
	void SetBackColor(COLORREF color);
	short GetTransparency() const;
	void SetTransparency(short value);

public:
	__declspec(property(get = GetPosition, put = SetPosition)) RECTF Position;
	__declspec(property(get = GetForeColor, put = SetForeColor)) COLORREF ForeColor;
	__declspec(property(get = GetBackColor, put = SetBackColor)) COLORREF BackColor;
	__declspec(property(get = GetTransparency, put = SetTransparency)) short Transparency;

};
