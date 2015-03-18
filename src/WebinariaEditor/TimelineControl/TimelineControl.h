#pragma once

#include "Timeline.h"
#include "Overlay.h"

[event_source(native), event_receiver(native)]
class TimelineControl
{

private:
	enum InteractionMode
	{
		imNormal,
		imMoveStart,
		imDrag,
		imMoveEnd,
		imDragHandleKnob
	};

private:
	void Repaint(HDC dc);
	void Resize(const RECT *rc);
	void DrawOverlay(HDC dc, const Overlay *overlay);
	void DrawHandle(HDC dc);
	BOOL GetTimelineRect(RECT *rc);
	void GetOverlayRect(const Overlay *overlay, RECT *orc);
	bool OverlayVisible(const Overlay *overlay);
	bool OverlayRectVisible(const RECT *orc);
	Overlay* OverlayFromPoint(const POINT *pt);
	void SaveAnchor(const POINT *pt, const Overlay *overlay);
	void SaveClickAnchor(const POINT *pt);
	void GetKnobRect(RECT *kr);
	int GetCurrentFramePosition();
	int FrameFromPosition(int x);

	bool paintGhost;
	RECT ghostRect;
	RECT effectiveRect;

	bool mouseDown;

private:
	static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	HINSTANCE hInstance;
	WNDCLASS timelineControlClass;
	ATOM timelineControlAtom;
	HWND timelineControl;

private:
	void UpdateMouseCursor();
	void UpdateMouseCursor(InteractionMode mode);
	void GetCursorPosition(POINT *pt);

	void MouseDown(WPARAM wParam, LPARAM lParam);
	void MouseUp(WPARAM wParam, LPARAM lParam);
	void MouseMove(WPARAM wParam, LPARAM lParam);

	InteractionMode SelectInteractionMode(const RECT *overlayRect, const POINT *referencePoint);

	InteractionMode interaction;
	POINT clickAnchor;
	Span overlayAnchor;

private:
	Timeline *timeline;
	double pixelsPerFrame;

	void TimelineChanged();

public:
	__event void RightMouseButtonClicked(Overlay *overlay, const POINT *pt);

public:
	TimelineControl();
	~TimelineControl();

	HWND Create(HINSTANCE instance, HWND parent, const RECT *rc, Timeline *ptimeline);

	void RepaintAll();

	operator HWND() { return timelineControl; }

};
