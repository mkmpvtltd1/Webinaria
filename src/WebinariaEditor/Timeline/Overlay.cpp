#include "StdAfx.h"
#include "Overlay.h"

#include <assert.h>

#include "utils.h"
#include "TextElement.h"

const int g_SizingBorderWidth = 2;							//	Sizing border has a width, so user could easily hook it with mouse.
const int g_SizingBumpSize = 4;								//	Size of lasso bumps.
const int g_DragMarkerSize = 6;								//	Drag marker size.

const int Overlay::Version = 1;

Overlay::Overlay()
	: refCount(0)
	, start(0)
	, end(0)
	, activeElement(0)
	, deleted(false)
	, enableSound(true)
	, enableWebcam(true)
	, webcamX(g_DefaultWebcamX)
	, webcamY(g_DefaultWebcamY)
{
}

Overlay::~Overlay()
{
	for each (OverlayElement *element in elements)
		delete element;
	elements.clear();
}

STDMETHODIMP Overlay::QueryInterface(const IID &iid, void **ppv)
{
	if (IID_IUnknown == iid || IID_IFrameOverlay == iid)
	{
		*ppv = static_cast<IFrameOverlay*>(this);
	}
	else
	{
		*ppv = 0;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG __stdcall Overlay::AddRef()
{
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall Overlay::Release()
{
	if (0 == InterlockedDecrement(&refCount))
	{
		delete this;
		return 0;
	}

	return refCount;
}

int Overlay::GetStart() const
{
	return start;
}

void Overlay::SetStart(int newStart)
{
	start = newStart;
}

int Overlay::GetEnd() const
{
	return end;
}

void Overlay::SetEnd(int newEnd)
{
	end = newEnd;
}

int Overlay::GetLength() const
{
	return End - Start;
}

OverlayElement* Overlay::GetActiveElement()
{
	return activeElement;
}

void Overlay::SetActiveElement(OverlayElement *element)
{
	activeElement = element;
}

bool Overlay::GetDeleted() const
{
	return deleted;
}

void Overlay::SetDeleted(bool value)
{
	deleted = value;
}

bool Overlay::GetEnableSound() const
{
	return enableSound;
}

void Overlay::SetEnableSound(bool value)
{
	enableSound = value;
}

bool Overlay::GetEnableWebcam() const
{
	return enableWebcam;
}

void Overlay::SetEnableWebcam(bool value)
{
	enableWebcam = value;
}

int Overlay::GetWebcamX() const
{
	return webcamX;
}

void Overlay::SetWebcamX(int value)
{
	webcamX = value;
}

int Overlay::GetWebcamY() const
{
	return webcamY;
}

void Overlay::SetWebcamY(int value)
{
	webcamY = value;
}

bool Overlay::HitTestElement(const OverlayElement* element, const RECT *rect, const POINT *point, OverlayHitTest *hitTest)
{
	assert(element);
	assert(rect);
	assert(point);
	assert(hitTest);

	RECT elementRect;
	element->GetRect(rect, &elementRect);

	if (point->x >= elementRect.left && point->x <= elementRect.right && point->y >= elementRect.top && 
		point->y <= elementRect.bottom)
	{
		int ht = 0;

		if (point->x - elementRect.left <= g_SizingBorderWidth)
			ht |= ohtLeftBorder;
		if (elementRect.right - point->x <= g_SizingBorderWidth)
			ht |= ohtRightBorder;
		if (point->y - elementRect.top <= g_SizingBorderWidth)
			ht |= ohtTopBorder;
		if (elementRect.bottom - point->y <= g_SizingBorderWidth)
			ht |= ohtBottomBorder;

		if (0 == ht)
			ht = ohtInside;

		*hitTest = static_cast<OverlayHitTest>(ht);

		return true;
	}

	return false;
}

OverlayElement* Overlay::FindElementFromPoint(const RECT *rect, const POINT *point, OverlayHitTest *hitTest)
{
	assert(rect);
	assert(point);
	assert(hitTest);

	*hitTest = ohtNone;

	for each (OverlayElement *element in elements)
		if (HitTestElement(element, rect, point, hitTest))
		{
			return element;
		}

	return 0;
}

void Overlay::Paint(HDC dc, const RECT *rect)
{
	assert(dc);
	assert(rect);

	//	The first element is on top, the last one is at the bottom.
	for (OverlayElements::reverse_iterator element = elements.rbegin(); element != elements.rend(); element++)
		(*element)->Paint(dc, rect);

	//	Draw selection frame around the active element.
	if (ActiveElement)
	{
		RECT elementRect;
		ActiveElement->GetRect(rect, &elementRect);
		InflateRect(&elementRect, -1, -1);

		HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
		HGDIOBJ oldBrush = SelectObject(dc, brush);
		HPEN pen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
		COLORREF oldBkColor = SetBkColor(dc, RGB(0xff, 0xff, 0xff));
		int oldRopMode = SetROP2(dc, R2_NOTXORPEN);

		HGDIOBJ oldPen = SelectObject(dc, pen);

		//	Draw selection rectangle.
		MoveToEx(dc, elementRect.left, elementRect.top, 0);
		LineTo(dc, elementRect.right - 1, elementRect.top);
		LineTo(dc, elementRect.right - 1, elementRect.bottom - 1);
		LineTo(dc, elementRect.left, elementRect.bottom - 1);
		LineTo(dc, elementRect.left, elementRect.top);

		SelectObject(dc, oldPen);
		DeleteObject(pen);

		pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		oldPen = SelectObject(dc, pen);

		//	Draw selection edge bumps.
		DrawRect(dc, 
			&NewRect(elementRect.left - 1, elementRect.top - 1, g_SizingBumpSize, g_SizingBumpSize));
		DrawRect(dc, 
			&NewRect(elementRect.right - g_SizingBumpSize + 1, elementRect.top - 1, g_SizingBumpSize, g_SizingBumpSize));
		DrawRect(dc, 
			&NewRect(elementRect.left - 1, elementRect.bottom - g_SizingBumpSize + 1, g_SizingBumpSize, g_SizingBumpSize));
		DrawRect(dc, 
			&NewRect(elementRect.right - g_SizingBumpSize + 1, elementRect.bottom - g_SizingBumpSize + 1, g_SizingBumpSize, g_SizingBumpSize));

		//	Draw inner selection bumps.
		DrawRect(dc, 
			&NewRect(elementRect.left + (RectWidth(elementRect) - g_SizingBumpSize) / 2, elementRect.top - 1, g_SizingBumpSize, g_SizingBumpSize));
		DrawRect(dc, 
			&NewRect(elementRect.right - g_SizingBumpSize + 1, elementRect.top + (RectHeight(elementRect) - g_SizingBumpSize) / 2, g_SizingBumpSize, g_SizingBumpSize));
		DrawRect(dc, 
			&NewRect(elementRect.left + (RectWidth(elementRect) - g_SizingBumpSize) / 2, elementRect.bottom - g_SizingBumpSize + 1, g_SizingBumpSize, g_SizingBumpSize));
		DrawRect(dc, 
			&NewRect(elementRect.left - 1, elementRect.top + (RectHeight(elementRect) - g_SizingBumpSize) / 2, g_SizingBumpSize, g_SizingBumpSize));

		//	Draw drag marker.
		DrawRect(dc, 
			&NewRect(elementRect.left + (RectWidth(elementRect) - g_DragMarkerSize) / 2, elementRect.top + (RectHeight(elementRect) - g_DragMarkerSize) / 2, g_DragMarkerSize, g_DragMarkerSize));

		SetBkColor(dc, oldBkColor);
		SelectObject(dc, oldPen);
		SelectObject(dc, oldBrush);
		SetROP2(dc, oldRopMode);
		DeleteObject(pen);
		DeleteObject(brush);
	}
}

void Overlay::SendToBack(OverlayElement *element)
{
	if (!element)
		return;

	elements.remove(element);
	elements.push_back(element);
}

void Overlay::BringToFront(OverlayElement *element)
{
	if (!element)
		return;

	elements.remove(element);
	elements.push_front(element);
}

void Overlay::Delete(OverlayElement *element)
{
	if (!element)
		return;

	elements.remove(element);
	if (ActiveElement == element)
		ActiveElement = 0;

	delete element;
}

OverlayElement* Overlay::CreateElementOfType(ElementType type)
{
	switch (type)
	{
	case ElementType_Text:
		return new TextElement();
	}

	return 0;
}

bool Overlay::Load(FileStream &in)
{
	try
	{
		int version;
		in >> version;

		in >> start;
		in >> end;

		int b;
		in >> b;
		deleted = static_cast<bool>(b);
		in >> b;
		enableSound = static_cast<bool>(b);
		in >> b;
		enableWebcam = static_cast<bool>(b);

		in >> webcamX;
		in >> webcamY;

		int len;
		in >> len;
		if (len < 0)
			return 0;

		for (int i = 0; i < len; i++)
		{
			short elementType;
			in >> elementType;

			OverlayElement *element = CreateElementOfType(static_cast<ElementType>(elementType));
			assert(element);								//	If null, element type is not supported.

			if (!element)
				return false;

			if (!element->Load(in))
			{
				delete element;
				return false;
			}

			elements.push_back(element);
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool Overlay::Save(FileStream &out)
{
	try
	{
		out << Version;

		out << start;
		out << end;

		out << static_cast<bool>(deleted);
		out << static_cast<bool>(enableSound);
		out << static_cast<bool>(enableWebcam);

		out << webcamX;
		out << webcamY;

		out << (int)elements.size();
		for each (OverlayElement *element in elements)
		{
			out << (short)element->GetElementType();
			if (!element->Save(out))
				return false;
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}
