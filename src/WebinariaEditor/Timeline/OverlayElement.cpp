#include "StdAfx.h"
#include "OverlayElement.h"

#include <assert.h>
#include <math.h>

#include "utils.h"

const int OverlayElement::MinimalSize = 10;

const int OverlayElement::Version = 1;

OverlayElement::OverlayElement()
	: transparency(0)
	, backColor(RGB(255, 255, 255))
	, foreColor(RGB(0, 0, 0))
{
	memset(&position, 0, sizeof(position));

	//	Default size and position.
	position.left = 0.1;		//	10%
	position.top = 0.3;			//	30%
	position.height = 0.3;
	position.width = 0.7;
}

OverlayElement::~OverlayElement()
{
}

RECTF OverlayElement::GetPosition() const
{
	return position;
}

void OverlayElement::SetPosition(const RECTF *newPosition)
{
	assert(newPosition);

	position = *newPosition;
}

void OverlayElement::GetRect(const RECT *overlayRect, RECT *rect) const
{
	assert(overlayRect);
	assert(rect);

	rect->left = (int)floor(RectWidth(*overlayRect) * position.left);
	rect->right = rect->left + (int)floor(RectWidth(*overlayRect) * position.width);
	rect->top = (int)floor(RectHeight(*overlayRect) * position.top);
	rect->bottom = rect->top + (int)floor(RectHeight(*overlayRect) * position.height);
}

void OverlayElement::SetRect(const RECT *overlayRect, const RECT *rect)
{
	assert(overlayRect);
	assert(rect);
	assert(RectHeight(*overlayRect));
	assert(RectWidth(*overlayRect));

	double ow = (double)RectWidth(*overlayRect), oh = (double)RectHeight(*overlayRect);

	position.left = (double)rect->left / ow;
	position.width = (double)RectWidth(*rect) / ow;
	position.top = (double)rect->top / oh;
	position.height = (double)RectHeight(*rect) / oh;
}

COLORREF OverlayElement::GetForeColor() const
{
	return foreColor;
}

void OverlayElement::SetForeColor(COLORREF color)
{
	foreColor = color;
}

COLORREF OverlayElement::GetBackColor() const
{
	return backColor;
}

void OverlayElement::SetBackColor(COLORREF color)
{
	backColor = color;
}

short OverlayElement::GetTransparency() const
{
	return transparency;
}

void OverlayElement::SetTransparency(short value)
{
	transparency = value;
}

bool OverlayElement::Load(FileStream &in)
{
	try
	{
		int version;
		in >> version;

		in >> position.left >> position.top >> position.width >> position.height;
		in >> foreColor >> backColor;
		in >> transparency;

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool OverlayElement::Save(FileStream &out)
{
	try
	{
		out << Version;

		out << position.left << position.top << position.width << position.height;
		out << foreColor << backColor;
		out << transparency;

		return true;
	}
	catch (...)
	{
		return false;
	}
}
