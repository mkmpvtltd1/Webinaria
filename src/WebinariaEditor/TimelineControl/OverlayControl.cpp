#include "StdAfx.h"
#include "OverlayControl.h"

#include "OverlayElement.h"
#include "utils.h"

OverlayControl::OverlayControl()
	: activeOverlay(0)
	, hitTestAnchor(ohtNone)
{
	memset(&clickAnchor, 0, sizeof(clickAnchor));
}

OverlayControl::~OverlayControl()
{
}

Overlay* OverlayControl::GetActiveOverlay() const
{
	return activeOverlay;
}

void OverlayControl::SetActiveOverlay(Overlay *overlay)
{
	activeOverlay = overlay;
}

void OverlayControl::ResetCursor()
{
	UpdateCursor(ohtNone);
}

void OverlayControl::UpdateCursor(OverlayHitTest hitTest)
{
	HCURSOR cursor;

	switch (hitTest)
	{
	case ohtNone:
		cursor = LoadCursor(0, IDC_ARROW);
		break;

	case ohtInside:
		cursor = LoadCursor(0, IDC_SIZEALL);
		break;

	case ohtLeftBorder:
	case ohtRightBorder:
		cursor = LoadCursor(0, IDC_SIZEWE);
		break;

	case ohtTopBorder:
	case ohtBottomBorder:
		cursor = LoadCursor(0, IDC_SIZENS);
		break;

	case ohtTopLeftCorner:
	case ohtBottomRightCorner:
		cursor = LoadCursor(0, IDC_SIZENWSE);
		break;

	case ohtTopRightCorner:
	case ohtBottomLeftCorner:
		cursor = LoadCursor(0, IDC_SIZENESW);
		break;

	default:
		cursor = LoadCursor(0, IDC_ARROW);
		break;
	}

	SetCursor(cursor);
}

void OverlayControl::ActivateElement(HWND hwnd, OverlayElement *element)
{
	if (ActiveOverlay->ActiveElement != element)
	{
		ActiveOverlay->ActiveElement = element;
		RepaintAll(hwnd);								//	Repaint window, if selection changed (block activated or selection cleared).

		ActiveOverlayElementChanged();
	}
}

void OverlayControl::RepaintAll(HWND hwnd)
{
	InvalidateRect(hwnd, 0, FALSE);
}

void OverlayControl::Paint(HWND hwnd, HDC dc)
{
	if (!ActiveOverlay)
		return;

	RECT rect;
	GetClientRect(hwnd, &rect);
    
	ActiveOverlay->Paint(dc, &rect);
}

void OverlayControl::MouseDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (!ActiveOverlay)
		return;

	//	Left or right button clicked -- select the element under cursor.
	RECT clipRect, clientRect;
	GetWindowRect(hwnd, &clipRect);
	GetClientRect(hwnd, &clientRect);

	//	Save original mouse position.
	GetCursorPosition(hwnd, &clickAnchor);

	//	Element could be activated only if no element active AND user doesn't click 
	//	within the active element.
	if (!ActiveOverlay->ActiveElement || 
		!ActiveOverlay->HitTestElement(ActiveOverlay->ActiveElement, &clientRect, &clickAnchor, &hitTestAnchor))
	{
		//	Find overlay element under cursor.
		OverlayElement *element = ActiveOverlay->FindElementFromPoint(&clientRect, &clickAnchor, &hitTestAnchor);
		ActivateElement(hwnd, element);
	}

	if (!ActiveOverlay->ActiveElement)
	{
		ResetCursor();
	}

	if (0 != (wParam & MK_LBUTTON))
	{
		if (ActiveOverlay->ActiveElement)
		{
			ActiveOverlay->ActiveElement->GetRect(&clientRect, &elementRectAnchor);

			//	Show that we're about to drag the object.
			UpdateCursor(hitTestAnchor);

			//	Capture the mouse.
			SetCapture(hwnd);
			ClipCursor(&clipRect);
		}

		return;
	}

	if (0 != (wParam & MK_RBUTTON))
	{
		//	Show popup menu.
		POINT pt;
		GetCursorPos(&pt);

		__raise RightMouseButtonClicked(ActiveOverlay->ActiveElement, &pt);

		return;
	}
}

void OverlayControl::MouseUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (!ActiveOverlay)
		return;

	//	Release mouse.
	ClipCursor(0);
	ReleaseCapture();
	ResetCursor();
}

void OverlayControl::MouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (!ActiveOverlay || !ActiveOverlay->ActiveElement)
	{
		ResetCursor();
		return;
	}

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	POINT pt;
	GetCursorPosition(hwnd, &pt);

	if (0 != (wParam & MK_LBUTTON))
	{
		RECT elementRect;
		memmove(&elementRect, &elementRectAnchor, sizeof(elementRect));

		int dx = pt.x - clickAnchor.x, dy = pt.y - clickAnchor.y;

		bool updateAnchors = false;

		//	Move/drag element.
		if (0 != (hitTestAnchor & ohtLeftBorder))
		{
			if (ohtInside != hitTestAnchor && dx > 0 && RectWidth(elementRect) - dx < OverlayElement::MinimalSize)
				dx = RectWidth(elementRect) - OverlayElement::MinimalSize;
			elementRect.left += dx;
		}
		if (0 != (hitTestAnchor & ohtRightBorder))
		{
			if (ohtInside != hitTestAnchor && dx < 0 && RectWidth(elementRect) + dx < OverlayElement::MinimalSize)
				dx = OverlayElement::MinimalSize - RectWidth(elementRect);
			elementRect.right += dx;
		}
		if (0 != (hitTestAnchor & ohtTopBorder))
		{
			if (ohtInside != hitTestAnchor && dy > 0 && RectHeight(elementRect) - dy < OverlayElement::MinimalSize)
				dy = RectHeight(elementRect) - OverlayElement::MinimalSize;
			elementRect.top += dy;
		}
		if (0 != (hitTestAnchor & ohtBottomBorder))
		{
			if (ohtInside != hitTestAnchor && dy < 0 && RectHeight(elementRect) + dy < OverlayElement::MinimalSize)
				dy = OverlayElement::MinimalSize - RectHeight(elementRect);
			elementRect.bottom += dy;
		}

		//	Check for sanity.
		if (elementRect.left < clientRect.left)
		{
			updateAnchors = true;
			OffsetRect(&elementRect, clientRect.left - elementRect.left, 0);
		}
		if (elementRect.right > clientRect.right)
		{
			updateAnchors = true;
			OffsetRect(&elementRect, clientRect.right - elementRect.right, 0);
		}
		if (elementRect.top < clientRect.top)
		{
			updateAnchors = true;
			OffsetRect(&elementRect, 0, clientRect.top - elementRect.top);
		}
		if (elementRect.bottom > clientRect.bottom)
		{
			updateAnchors = true;
			OffsetRect(&elementRect, 0, clientRect.bottom - elementRect.bottom);
		}

		if (updateAnchors)
		{
			memmove(&clickAnchor, &pt, sizeof(clickAnchor));
			memmove(&elementRectAnchor, &elementRect, sizeof(elementRectAnchor));
		}

		//	Update element coordinates.
		ActiveOverlay->ActiveElement->SetRect(&clientRect, &elementRect);

		//	Drag object under cursor.
		RepaintAll(hwnd);
	}
	else
	{
		//	No mouse keys pressed -- change cursor accordingly to what's 
		//	visible on the screen.
		OverlayHitTest hitTest = ohtNone;
		OverlayElement *element = ActiveOverlay->ActiveElement;

		if (ActiveOverlay->ActiveElement && 
			!ActiveOverlay->HitTestElement(ActiveOverlay->ActiveElement, &clientRect, &pt, &hitTest))
		{
			element = ActiveOverlay->FindElementFromPoint(&clientRect, &pt, &hitTest);
		}

		if (0 != element && element == ActiveOverlay->ActiveElement && ohtInside != hitTest)
			UpdateCursor(hitTest);
		else
			ResetCursor();
	}
}
