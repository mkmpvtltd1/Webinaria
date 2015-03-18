#include "StdAfx.h"
#include "TimelineControl.h"

#include <memory.h>
#include <assert.h>
#include <math.h>

#include "utils.h"

const TCHAR *g_TimelineControlClassName = _T("WebinariaTimelineControlClass");
const double g_MinimalOverlayWidth = 5.0;

const int g_HandleKnobSize = 12;
const int g_HandleWidth = 4;

const int g_OverlayEdgeTreshold = 10;

TimelineControl::TimelineControl()
	: timelineControlAtom(0)
	, hInstance(0)
	, timelineControl(0)
	, timeline(0)
	, pixelsPerFrame(0.0)
	, interaction(imNormal)
	, overlayAnchor(0, 0)
	, paintGhost(false)
	, mouseDown(false)
{
	memset(&timelineControlClass, 0, sizeof(timelineControlClass));
	memset(&clickAnchor, 0, sizeof(clickAnchor));
	memset(&ghostRect, 0, sizeof(ghostRect));
	memset(&effectiveRect, 0, sizeof(effectiveRect));
}

TimelineControl::~TimelineControl()
{
	if (0 != timeline)
		__unhook(&Timeline::Changed, timeline, &TimelineControl::TimelineChanged);

	if (0 != timelineControl)
		DestroyWindow(timelineControl);

	if (0 != timelineControlAtom)
		UnregisterClass(g_TimelineControlClassName, hInstance);
}

HWND TimelineControl::Create(HINSTANCE instance, HWND parent, const RECT *rc, Timeline *ptimeline)
{
	assert(0 != ptimeline);

	timeline = ptimeline;
	__hook(&Timeline::Changed, timeline, &TimelineControl::TimelineChanged);

	hInstance = instance;

	//	Setup window class.
	timelineControlClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	timelineControlClass.lpfnWndProc = (WNDPROC)s_WndProc;
	timelineControlClass.hInstance = hInstance;
	timelineControlClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	timelineControlClass.hCursor = LoadCursor(0, IDC_ARROW);
	timelineControlClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	timelineControlClass.lpszClassName = g_TimelineControlClassName;

	//	Register window class.
	timelineControlAtom = RegisterClass(&timelineControlClass);
	if (0 == timelineControlAtom)
		return false;

	//	Create window.
	timelineControl = CreateWindow(g_TimelineControlClassName, _T("Timeline"), WS_CHILD | WS_VISIBLE, rc->left, rc->top, 
		RectWidth(*rc), RectHeight(*rc), parent, 0, 0, this);

	Resize(rc);

	return timelineControl;
}

void TimelineControl::Resize(const RECT *rc)
{
	//	Calculate effective rect.
	effectiveRect.left = g_HandleKnobSize / 2;
	effectiveRect.right = effectiveRect.left + RectWidth(*rc) - g_HandleKnobSize;
	effectiveRect.top = g_HandleKnobSize + 2;
	effectiveRect.bottom = RectHeight(*rc) - 5;
}

LRESULT CALLBACK TimelineControl::s_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCCREATE:
		return TRUE;										//	Go on with window.

	case WM_CREATE:
		{
			//	Save pointer to timeline control instance.
			CREATESTRUCT *cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
		}
		return 0;

	default:
		{
			LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (0 != ptr)
				return reinterpret_cast<TimelineControl*>(ptr)->WndProc(hwnd, message, wParam, lParam);
		}
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK TimelineControl::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		if (GetUpdateRect(hwnd, 0, FALSE))
		{
			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint(hwnd, &ps);
			if (dc)
			{
				Repaint(dc);
				EndPaint(hwnd, &ps);
			}
		}
		return 0;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		MouseDown(wParam, lParam);
		return 0;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		MouseUp(wParam, lParam);
		return 0;

	case WM_MOUSEMOVE:
		MouseMove(wParam, lParam);
		return 0;

	case WM_SIZE:
	case WM_SIZING:
		{
			RECT rc;
			GetClientRect(hwnd, &rc);
			Resize(&rc);
			RepaintAll();
			return (WM_SIZING == message);
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL TimelineControl::GetTimelineRect(RECT *rc)
{
	memmove(rc, &effectiveRect, sizeof(*rc));
	InflateRect(rc, -2, -2);

	return TRUE;
}

void TimelineControl::GetOverlayRect(const Overlay *overlay, RECT *orc)
{
	RECT rc;

	double ow = 
		max((double)overlay->Length * pixelsPerFrame, g_MinimalOverlayWidth);

	GetTimelineRect(&rc);

	orc->left = rc.left + (int)ceil(overlay->Start * pixelsPerFrame);
	if (orc->left < rc.left)
		orc->left = rc.left;
	orc->top = rc.top;
	orc->bottom = rc.bottom;
	orc->right = orc->left + (int)ceil(ow);
	if (orc->right > rc.right)
		orc->right = rc.right;
}

bool TimelineControl::OverlayRectVisible(const RECT *orc)
{
	RECT rc;
	GetTimelineRect(&rc);

	return (orc->left >= rc.left && orc->right <= rc.right);
}

bool TimelineControl::OverlayVisible(const Overlay *overlay)
{
	RECT orc;
	GetOverlayRect(overlay, &orc);

	return OverlayRectVisible(&orc);
}

Overlay* TimelineControl::OverlayFromPoint(const POINT *pt)
{
	for (OverlaysContainer::const_iterator i = timeline->FirstOverlay(); i != timeline->LastOverlay(); i++)
	{
		RECT rc;
		GetOverlayRect(*i, &rc);

		if (rc.left <= pt->x && rc.right >= pt->x && rc.top <= pt->y && rc.bottom >= pt->y)
			return *i;

		if (rc.left > pt->x)
			break;
	}

	return 0;
}

void TimelineControl::DrawOverlay(HDC dc, const Overlay *overlay)
{
	RECT orc;
	GetOverlayRect(overlay, &orc);

	if (!OverlayRectVisible(&orc))							//	Drop overlay, which is invisible.
		return;

	if (overlay->Deleted)
	{
		HBRUSH deletedBrush = CreateSolidBrush(RGB(0xff, 0, 0));
		FillRect(dc, &orc, deletedBrush);
		DeleteObject(deletedBrush);
	}
	else
		FillRect(dc, &orc, (HBRUSH)(COLOR_HIGHLIGHT + 1));
	DrawEdge(dc, &orc, EDGE_RAISED, BF_RECT);
	if (overlay == timeline->ActiveOverlay)
	{
		RECT frc;
		memmove(&frc, &orc, sizeof(frc));
		//	InflateRect(&frc, -2, -2);
		DrawFocusRect(dc, &frc);
	}
}

void TimelineControl::DrawHandle(HDC dc)
{
	RECT rc;
	GetClientRect(timelineControl, &rc);

	//	Knob.
	RECT kr;
	GetKnobRect(&kr);
	FillRect(dc, &kr, (HBRUSH)(COLOR_BTNFACE + 1));
	DrawEdge(dc, &kr, EDGE_RAISED, BF_RECT);

	//	Handle.
	RECT hr;
	hr.left = kr.left + (RectWidth(kr) - g_HandleWidth) / 2;
	hr.right = hr.left + g_HandleWidth;
	hr.top = kr.bottom;
	hr.bottom = hr.top + RectHeight(effectiveRect) + 3;
	FillRect(dc, &hr, (HBRUSH)(COLOR_BTNFACE + 1));
	DrawEdge(dc, &hr, EDGE_RAISED, BF_RECT);
}

void TimelineControl::Repaint(HDC dc)
{
	RECT rc;
	GetClientRect(timelineControl, &rc);

	//	Fill background.
	FillRect(dc, &rc, (HBRUSH)(COLOR_BTNFACE + 1));

	//	Draw outer edges.
	DrawEdge(dc, &effectiveRect, EDGE_SUNKEN, BF_RECT);

	//	Draw all overlays.
	for (OverlaysContainer::iterator overlay = timeline->FirstOverlay(); overlay != timeline->LastOverlay(); overlay++)
		DrawOverlay(dc, *overlay);

	if (paintGhost)
	{
		//	Draw ghost overlay rectangle.
		int oldRop = SetROP2(dc, R2_NOTXORPEN);
		Rectangle(dc, ghostRect.left, ghostRect.top, ghostRect.right, ghostRect.bottom);
		SetROP2(dc, oldRop);
	}

	//	Draw 10th frame indicators.
	HPEN pen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
	HGDIOBJ oldPen = SelectObject(dc, pen);

	for (int i = 0; i < timeline->Length; i += 10)
		if (i && !(i % 10))
		{
			MoveToEx(dc, effectiveRect.left + i * pixelsPerFrame, effectiveRect.top - 2, 0);
			LineTo(dc, effectiveRect.left + i * pixelsPerFrame, effectiveRect.bottom + 2);
		}

	SelectObject(dc, oldPen);
	DeleteObject(pen);

	DrawHandle(dc);
}

void TimelineControl::RepaintAll()
{
	InvalidateRect(timelineControl, 0, TRUE);
}

int TimelineControl::GetCurrentFramePosition()
{
	RECT rc;
	GetTimelineRect(&rc);

	return min(rc.left + (int)ceil((double)timeline->CurrentFrame * pixelsPerFrame), rc.right);
}

void TimelineControl::GetKnobRect(RECT *kr)
{
	RECT rc;
	GetClientRect(timelineControl, &rc);

	kr->left = GetCurrentFramePosition() - g_HandleKnobSize / 2;
	kr->top = rc.top;
	kr->bottom = kr->top + g_HandleKnobSize;
	kr->right = kr->left + g_HandleKnobSize;
}

void TimelineControl::TimelineChanged()
{
	RECT rc;
	GetTimelineRect(&rc);

	if (timeline->Length > 0)
	{
		pixelsPerFrame = (double)RectWidth(rc) / (double)timeline->Length;
	}
	else
	{
		pixelsPerFrame = 0.0;
	}

	RepaintAll();
}

void TimelineControl::UpdateMouseCursor()
{
	UpdateMouseCursor(interaction);
}

void TimelineControl::UpdateMouseCursor(InteractionMode mode)
{
	switch (mode)
	{
	case imMoveStart:
	case imMoveEnd:
	case imDragHandleKnob:
		SetCursor(LoadCursor(0, IDC_SIZEWE));
		break;

	case imDrag:
		SetCursor(LoadCursor(0, IDC_SIZEALL));
		break;

	default:
		SetCursor(LoadCursor(0, IDC_ARROW));
		break;
	}
}

void TimelineControl::GetCursorPosition(POINT *pt)
{
	::GetCursorPosition(timelineControl, pt);
}

TimelineControl::InteractionMode TimelineControl::SelectInteractionMode(const RECT *overlayRect, const POINT *referencePoint)
{
	if (referencePoint->x >= overlayRect->left && referencePoint->x <= overlayRect->left + 2)
		return imMoveStart;

	if (referencePoint->x <= overlayRect->right && referencePoint->x >= overlayRect->right - 2)
		return imMoveEnd;

	if (referencePoint->x >= overlayRect->left && referencePoint->x <= overlayRect->right)
		return imDrag;

	return imNormal;
}

void TimelineControl::SaveAnchor(const POINT *pt, const Overlay *overlay)
{
	SaveClickAnchor(pt);
	//	Save original overlay rect.
	overlayAnchor = Span(overlay->Start, overlay->End);
}

void TimelineControl::SaveClickAnchor(const POINT *pt)
{
	//	Save original cursor position.
	memmove(&clickAnchor, pt, sizeof(clickAnchor));
}

void TimelineControl::MouseDown(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	GetCursorPosition(&pt);

	Overlay *overlay = OverlayFromPoint(&pt);
	bool canChangeOverlay = true;

	if (0 != (wParam & MK_LBUTTON))
	{
		bool dragging = false;

		if (overlay)
		{
			//	Start overlay dragging.
			RECT rc;
			GetOverlayRect(overlay, &rc);

			SaveAnchor(&pt, overlay);
			interaction = SelectInteractionMode(&rc, &clickAnchor);

			dragging = true;
			mouseDown = true;
		}
		else
		{
			//	Start curent frame knob dragging.
			RECT kr;
			GetKnobRect(&kr);

			if (PtInRect(&kr, pt))
			{
				SaveClickAnchor(&pt);
				interaction = imDragHandleKnob;

				dragging = true;
				canChangeOverlay = false;
			}
			else
			{
				//	Move current frame pointer to click position.
				int pos = FrameFromPosition(pt.x);

				if (pos < 0 || pos > timeline->Length)
					pos = timeline->CurrentFrame;

				timeline->CurrentFrame = pos;
			}
		}

		if (dragging)
		{
			RECT controlRect;
			GetWindowRect(timelineControl, &controlRect);

			SetCapture(timelineControl);
			ClipCursor(&controlRect);						//	Ensure that mouse won't drag overlay out of control boundaries.

			UpdateMouseCursor();
		}
	}

	if (canChangeOverlay)
		timeline->ActiveOverlay = overlay;

	if (0 != (wParam & MK_RBUTTON))
	{
		POINT mouse;
		GetCursorPos(&mouse);

		__raise RightMouseButtonClicked(overlay, &mouse);
	}
}

void TimelineControl::MouseUp(WPARAM wParam, LPARAM lParam)
{
	ClipCursor(0);
	ReleaseCapture();

	interaction = imNormal;
	UpdateMouseCursor();

	paintGhost = false;
	RepaintAll();

	if (mouseDown)
	{
		mouseDown = false;

		if (timeline->ActiveOverlay)
		{
			POINT pt;
			GetCursorPosition(&pt);

			RECT rc;
			GetOverlayRect(timeline->ActiveOverlay, &rc);

			if (pt.x >= rc.left && pt.x <= rc.left + g_OverlayEdgeTreshold)
				timeline->CurrentFrame = timeline->ActiveOverlay->Start;
			else if (pt.x >= rc.right - g_OverlayEdgeTreshold && pt.x <= rc.right)
				timeline->CurrentFrame = timeline->ActiveOverlay->End;
			else
			{
				int middleClick = rc.left + RectWidth(rc) / 2;
				if (pt.x >= middleClick - g_OverlayEdgeTreshold && pt.x <= middleClick + g_OverlayEdgeTreshold)
				{
					timeline->CurrentFrame = timeline->ActiveOverlay->Start + 
						(timeline->ActiveOverlay->End - timeline->ActiveOverlay->Start) / 2;
				}
			}
		}
	}
}

int TimelineControl::FrameFromPosition(int x)
{
	RECT trc;
	GetTimelineRect(&trc);

	return (x - trc.left) / pixelsPerFrame;
}

void TimelineControl::MouseMove(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	GetCursorPosition(&pt);

	mouseDown = false;

	if (imDragHandleKnob == interaction)
	{
		int pos = FrameFromPosition(pt.x);
		bool saveAnchor = true;

		if (pos < 0 || pos > timeline->Length)
		{
			pos = timeline->CurrentFrame;
			saveAnchor = false;
		}

		timeline->CurrentFrame = pos;
		if (saveAnchor)
			SaveClickAnchor(&pt);

		UpdateMouseCursor(interaction);
		return;
	}

	int delta = (int)ceil((double)(pt.x - clickAnchor.x) / pixelsPerFrame);
	Overlay *overlay = 
		(imNormal == interaction) ? OverlayFromPoint(&pt) : timeline->ActiveOverlay;

	if (!overlay)
	{
		UpdateMouseCursor(imNormal);
		return;
	}

	RECT rc;
	GetOverlayRect(overlay, &rc);

	if (imNormal == interaction)
	{
		InteractionMode mode = SelectInteractionMode(&rc, &pt);
		if (imDrag == mode)
			mode = imNormal;

		UpdateMouseCursor(mode);
		return;
	}

	OverlayMoveMode mode = ommNone;

	switch (interaction)
	{
	case imMoveStart:
		mode = ommStartOnly;
		break;

	case imMoveEnd:
		mode = ommEndOnly;
		break;

	case imDrag:
		mode = ommDrag;
		break;
	}

	paintGhost = false;

	OverlayMoveResult moveResult = timeline->MoveOverlay(overlay, mode, &overlayAnchor, delta);
	switch (moveResult)
	{
	case omrSwapped:
		timeline->MoveOverlay(overlay, mode, &overlayAnchor, delta);
		SaveAnchor(&pt, overlay);
		break;

	case omrNone:
		if (imDrag == interaction && delta)
		{
			//	Center ghost rect arond the mouse cursor.
			int grw = RectWidth(rc);
			ghostRect.left = pt.x - (grw / 2);
			ghostRect.right = ghostRect.left + grw;
			ghostRect.top = rc.top;
			ghostRect.bottom = rc.bottom;

			paintGhost = true;
			RepaintAll();
		}
		break;
	}

	UpdateMouseCursor(interaction);
}
