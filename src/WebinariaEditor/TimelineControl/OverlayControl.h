#pragma once

#include "Overlay.h"

[event_source(native)]
class OverlayControl
{

private:
	void ResetCursor();
	void UpdateCursor(OverlayHitTest hitTest);
	void RepaintAll(HWND hwnd);

	Overlay* activeOverlay;
	POINT clickAnchor;
	OverlayHitTest hitTestAnchor;
	RECT elementRectAnchor;

public:
	void MouseDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void MouseUp(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void MouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void Paint(HWND hwnd, HDC dc);

	Overlay* GetActiveOverlay() const;
	void SetActiveOverlay(Overlay *overlay);

	void ActivateElement(HWND hwnd, OverlayElement *element);

public:
	__declspec(property(get = GetActiveOverlay, put = SetActiveOverlay)) Overlay* ActiveOverlay;

public:
	__event void ActiveOverlayElementChanged();
	__event void RightMouseButtonClicked(OverlayElement *element, const POINT *pt);

public:
	OverlayControl();
	~OverlayControl();

};
