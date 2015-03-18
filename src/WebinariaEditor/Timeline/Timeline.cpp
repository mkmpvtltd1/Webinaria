#include "StdAfx.h"
#include "Timeline.h"

#include <assert.h>
#include <algorithm>

#include "utils.h"
#include "FileStream.h"
#include "OverlaysEnum.h"

const int g_DefaultSpanLength = 5;							//	Default span length in frames.

class UpdateWatchdog
{

private:
	Timeline *timeline;

public:
	UpdateWatchdog(Timeline *ptimeline)
	{
		timeline = ptimeline;
		timeline->BeginUpdate();
	}

	~UpdateWatchdog()
	{
		timeline->EndUpdate();
	}

};

const int Timeline::Version = 1;
const std::wstring Timeline::Signature = L"Webinaria";

Timeline::Timeline(void)
	: refCount(0)
	, updates(0)
	, length(0)
	, activeOverlay(0)
	, currentFrame(0)
	, fresh(true)
{
}

Timeline::~Timeline(void)
{
	ClearOverlays();
}

void Timeline::ClearOverlays()
{
	for each (Overlay* p in overlays)
		delete p;
	overlays.clear();

	ActiveOverlay = 0;
}

STDMETHODIMP Timeline::QueryInterface(const IID &iid, void **ppv)
{
	if (IID_IUnknown == iid || IID_ITimeline == iid)
	{
		*ppv = static_cast<ITimeline*>(this);
	}
	else
	{
		*ppv = 0;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG __stdcall Timeline::AddRef()
{
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall Timeline::Release()
{
	if (0 == InterlockedDecrement(&refCount))
	{
//BUGBUG		delete this;
//need not, since instance created not via CoCreateInstance
		return 0;
	}

	return refCount;
}

STDMETHODIMP Timeline::GetOverlaysEnum(IOverlaysEnum **ppEnum)
{
	OverlaysEnum *overlaysEnum = new OverlaysEnum(&overlays);

	*ppEnum = dynamic_cast<IOverlaysEnum*>(overlaysEnum);
	(*ppEnum)->AddRef();

	return S_OK;
}

int Timeline::GetLength() const
{
	return length;
}

bool Timeline::GetFresh() const
{
	return fresh;
}

void Timeline::Initialize(int newLength)
{
	BeginUpdate();

	ClearOverlays();
	length = newLength;
	CurrentFrame = 0;

	EndUpdate();

	fresh = true;
}

Overlay* Timeline::GetActiveOverlay() const
{
	return activeOverlay;
}

void Timeline::SetActiveOverlay(Overlay *poverlay)
{
	if (poverlay != activeOverlay)
	{
		activeOverlay = poverlay;
		NotifyActiveOverlayChanged();
	}
}

int Timeline::GetCurrentFrame() const
{
	return currentFrame;
}

void Timeline::SetCurrentFrame(int frame)
{
	SetCurrentFrame(frame, true);
}

void Timeline::SetCurrentFrameDontActivateOverlay(int frame)
{
	SetCurrentFrame(frame, false);
}

void Timeline::SetCurrentFrame(int frame, bool activateOverlayUnderFrame)
{
	assert(Length > 0 && currentFrame >= 0 && currentFrame <= length);

	if (Length < 1 || currentFrame < 0 || currentFrame > length)
		return;

	if (currentFrame != frame)
	{
		currentFrame = frame;
		NotifyCurrentFrameChanged();

		if (activateOverlayUnderFrame)
			ActiveOverlay = GetOverlayForFrame(frame);
	}
}

void Timeline::NotifyChanged()
{
	if (!updates)
		__raise Changed();
}

void Timeline::NotifyActiveOverlayChanged()
{
	if (!updates)
	{
		__raise ActiveOverlayChanged();
		NotifyChanged();
	}
}

void Timeline::NotifyCurrentFrameChanged()
{
	if (!updates)
	{
		__raise CurrentFrameChanged();
		NotifyChanged();
	}
}

void Timeline::BeginUpdate()
{
	updates++;
}

void Timeline::EndUpdate()
{
	if (updates > 0)
		updates--;

	if (!updates)
		NotifyChanged();
}

Overlay* Timeline::Previous(const Overlay *overlay) const
{
	OverlaysContainer::const_iterator i = find(overlays.begin(), overlays.end(), overlay);

	return (overlays.begin() == i || overlays.end() == i) ? 0 : *(--i);
}

Overlay* Timeline::Next(const Overlay *overlay) const
{
	OverlaysContainer::const_iterator i = find(overlays.begin(), overlays.end(), overlay);

	if (overlays.end() == i)
		return 0;

	i++;
	return (overlays.end() == i) ? 0 : *i;
}

Overlay* Timeline::CreateOverlay(int overlaySpanLength/* = 0*/)
{
	Overlay *overlay = 0;

	int spanLength = 
		(overlaySpanLength < g_DefaultSpanLength || overlaySpanLength > Length) ? g_DefaultSpanLength : overlaySpanLength;

	if (overlays.empty() || (*overlays.rbegin())->End + 2 + spanLength < Length)
	{
		//	If there're no overlays, or the last is far from end, then
		//	add the new overlay to the end of the list.
		overlay = new Overlay();

		if (overlays.empty())
		{
			overlay->Start = 1;
			overlay->End = spanLength + 1;
		}
		else
		{
			Overlay *last = (*overlays.rbegin());
			overlay->Start = last->End + 1;
			overlay->End = overlay->Start + spanLength + 1;
		}

		overlays.push_back(overlay);
	}
	else if (!overlays.empty() && (*overlays.begin())->Start >= 2 + spanLength)
	{
		//	If there's a lot of empty space in the very beginning, then
		//	start the list with the new overlay.
		overlay = new Overlay();

		overlay->Start = 1;
		overlay->End = spanLength + 1;

		overlays.push_front(overlay);
	}
	else
	{
		//	Otherwise find a spot to put the new overlay into.
		Overlay *prev = 0, *anchor = 0;
		int maxFreeSpace = 0;

		for each (Overlay *o in overlays)
		{
			if (prev)
			{
				int freeSpace = o->Start - prev->End;
				if (freeSpace >= 2 + spanLength && freeSpace > maxFreeSpace)
				{
					anchor = prev;
					maxFreeSpace = freeSpace;
				}
			}

			prev = o;
		}

		if (anchor)
		{
			OverlaysContainer::iterator i = std::find(overlays.begin(), overlays.end(), anchor);

			overlay = new Overlay();

			overlay->Start = (*i)->End + 1;
			overlay->End = overlay->Start + spanLength + 1;

			i++;
			overlays.insert(i, overlay);
		}
	}

	if (overlay)
		NotifyChanged();

	//	Return value could be NULL, this means there's not enough of empty space in the timeline.
	return overlay;
}

void Timeline::Delete(Overlay *overlay)
{
	if (overlay)
	{
		overlays.remove(overlay);
		if (ActiveOverlay == overlay)
			ActiveOverlay = 0;

		delete overlay;

		NotifyChanged();
	}
}

Overlay* Timeline::GetOverlayForFrame(int frame)
{
	if (frame < 0 || frame > Length)
		return 0;

	for each (Overlay *overlay in overlays)
	{
		if (overlay->Start > frame)
			return 0;
		else if (overlay->Start <= frame && overlay->End >= frame)
		{
			return overlay;
		}
	}

	return 0;
}

void Timeline::Normalize()
{
	//	Traverses all overlays and makes sure they're not overlapping.
}

void Timeline::SwapOverlays(Overlay *left, Overlay *right)
{
	assert(left);
	assert(right);

	int lenLeft = left->Length, lenRight = right->Length;

	right->Start = left->Start;
	right->End = right->Start + lenRight;
	left->Start = right->End + 1;
	left->End = left->Start + lenLeft;
}

OverlayMoveResult Timeline::MoveOverlay(Overlay *overlay, OverlayMoveMode mode, const Span *origin, int delta)
{
	assert(overlay);

	if (ommNone == mode || !delta)
		return omrNone;

	OverlayMoveResult result = omrMoved;
	Overlay *prev = Previous(overlay), *next = Next(overlay);

	//if (prev)
	//	TRACE("P: %d; %d; %d\n", prev->Start, prev->End, prev->End - prev->Start);
	//TRACE("O: %d; %d; %d\n", overlay->Start, overlay->End, overlay->End - overlay->Start);
	//TRACE("delta: %d\n", delta);
	//if (next)
	//	TRACE("N: %d; %d; %d\n", next->Start, next->End, next->End - next->Start);

	/*
		Procedure of overlays swapping is far from perfect now.
		The good news is that it could be enhanced later.
	*/

	int start = origin->first, end = origin->second;
	int leftBorder = (prev) ? prev->End + 1 : 0, rightBorder = (next) ? next->Start - 1 : Length - 1;

	if (delta > 0)
	{
		switch (mode)
		{
		case ommStartOnly:
			start = (start + delta < end) ? start + delta : end - 1;
			break;

		case ommEndOnly:
			end = (end + delta <= rightBorder) ? end + delta : rightBorder;
			break;

		case ommDrag:
			if (next && start + delta > next->End)			//	User has moved the mouse way beyond the end of next frame.
			{
				if (end + delta <= Length)
				{
					//	Move overlay's position in the list.
					overlays.remove(overlay);
					OverlaysContainer::iterator i = find(overlays.begin(), overlays.end(), next);
					i++;
					overlays.insert(i, overlay);

					//	Swap current and previous frames.
					start += delta;
					end += delta;

					result = omrSwapped;
				}
				else
					return omrMoved;
			}
			else
			{
				if (end + delta >= rightBorder)
					delta = rightBorder - end;
				start = start + delta;
				end = end + delta;
			}
			break;
		}
	}
	else
	{
		//	delta < 0.
		switch (mode)
		{
		case ommStartOnly:
			start = (start + delta > leftBorder) ? start + delta : leftBorder;
			break;

		case ommEndOnly:
			end = (end + delta > start) ? end + delta : start + 1;
			break;

		case ommDrag:
			if (prev && end + delta < prev->Start)			//	User has moved the mouse way beyond the start of previous frame.
			{
				if (start + delta >= 0)
				{
					//	Move overlay's position in the list.
					overlays.remove(overlay);
					OverlaysContainer::iterator i = find(overlays.begin(), overlays.end(), prev);
					overlays.insert(i, overlay);

					//	Swap current and previous frames.
					start += delta;
					end += delta;

					result = omrSwapped;
				}
				else
					return omrMoved;
			}
			else
			{
				if (start + delta < leftBorder)
					delta = leftBorder - start;
				start = start + delta;
				end = end + delta;
			}
			break;
		}
	}

	overlay->Start = start;
	overlay->End = end;

	if (overlay->Start == origin->first && overlay->End == origin->second)
		return omrNone;

	NotifyChanged();

	return result;
}

bool Timeline::Load(const TCHAR *fileName)
{
	try
	{
		FileStream in;
		if (!in.Open(fileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING))
			return false;

		UpdateWatchdog updateForSure(this);

		std::wstring signature;
		in >> signature;
		if (Signature != signature)
			return false;

		int version;
		in >> version;

		int len;
		in >> len;
		if (len < 0)
			return 0;
		Initialize(len);

		in >> len;
		if (len < 0)
			return 0;

		for (int i = 0; i < len; i++)
		{
			Overlay *overlay = new Overlay();

			if (!overlay->Load(in))
			{
				delete overlay;
				return false;
			}

			overlays.push_back(overlay);
		}

		CurrentFrame = 0;

		fresh = false;

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool Timeline::Save(const TCHAR *fileName)
{
	try
	{
		FileStream out;
		if (!out.Open(fileName, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS))
			return false;

		out << Signature;
		out << Version;

		out << Length;

		out << (int)overlays.size();
		for each (Overlay *overlay in overlays)
		{
			if (!overlay->Save(out))
				return false;
		}

		fresh = false;

		return true;
	}
	catch (...)
	{
		return false;
	}
}
