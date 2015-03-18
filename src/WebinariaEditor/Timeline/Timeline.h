#pragma once

#include <list>
#include <string>

#include "iRGBFilters.h"
#include "Overlay.h"

typedef std::list<Overlay*> OverlaysContainer;

enum OverlayMoveMode
{
	ommNone,
	ommStartOnly,
	ommEndOnly,
	ommDrag
};

enum OverlayMoveResult
{
	omrNone,
	omrMoved,
	omrSwapped
};

/*
	Timeline contains chained overlays.
	Overlays cannot overlap, and in order to simplify the logic, the "next" overlay 
	will become "current" only when "current" overlay ends, even if the start of the "next" overlay
	is in the middle of the "current" one.

	Example (overlays are defined by start and end frame).
	(5; 10) <-> (7; 14) <-> (20; 23)
	means that timeline contains of 3 overlays; first starts at 5th and lasts till 10th; 
	then second will be shown only at 11th (despite it was set to start at 7) and will last till 14th;
	then third one will be shown at 20th and will be visible till 23rd.

	There's a room for improvement and it's possible (and makes sense) to make overlays to overlap, 
	but not in the current Webinaria version.
*/
[event_source(native)]
class Timeline : ITimeline
{

private:
	LONG refCount;

private:
	static const std::wstring Signature;					//	Data record version in file.
	static const int Version;								//	Data record version in file.

private:
	bool fresh;
	OverlaysContainer overlays;
	int length;
	int currentFrame;
	int updates;
	Overlay *activeOverlay;

	void SwapOverlays(Overlay *left, Overlay *right);

	void NotifyChanged();
	void NotifyActiveOverlayChanged();
	void NotifyCurrentFrameChanged();

	void SetCurrentFrame(int frame, bool activateOverlayUnderFrame);

	void ClearOverlays();

public:
	int GetLength() const;
	Overlay* GetActiveOverlay() const;
	void SetActiveOverlay(Overlay *poverlay);
	int GetCurrentFrame() const;
	void SetCurrentFrame(int frame);
	void SetCurrentFrameDontActivateOverlay(int frame);
	bool GetFresh() const;

public:
	Timeline(void);
	~Timeline(void);

	void Initialize(int newLength);

	bool Load(const TCHAR *fileName);
	bool Save(const TCHAR *fileName);

	void BeginUpdate();
	void EndUpdate();

public:
	OverlaysContainer::iterator FirstOverlay() { return overlays.begin(); }
	OverlaysContainer::iterator LastOverlay() { return overlays.end(); }
	Overlay* Previous(const Overlay *overlay) const;
	Overlay* Next(const Overlay *overlay) const;

	Overlay* CreateOverlay(int overlaySpanLength = 0);
	void Delete(Overlay *overlay);
	Overlay* GetOverlayForFrame(int frame);

	OverlayMoveResult MoveOverlay(Overlay *overlay, OverlayMoveMode mode, const Span *origin, int delta);

	void Normalize();

public:
	__declspec(property(get = GetLength)) int Length;
	__declspec(property(get = GetActiveOverlay, put = SetActiveOverlay)) Overlay* ActiveOverlay;
	__declspec(property(get = GetCurrentFrame, put = SetCurrentFrame)) int CurrentFrame;
	__declspec(property(get = GetFresh)) bool Fresh;

public:
	__event void Changed();
	__event void ActiveOverlayChanged();
	__event void CurrentFrameChanged();

public:
	//	ITimeline methods.
	STDMETHOD(GetOverlaysEnum)(IOverlaysEnum **ppEnum);

	//	IUnknown methods.
    STDMETHOD(QueryInterface)(const IID &iid, void **ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();

};
