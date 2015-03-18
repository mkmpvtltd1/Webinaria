#include "StdAfx.h"
#include "OverlaysEnum.h"

#include "TimelineErrorCodes.h"

OverlaysEnum::OverlaysEnum(OverlaysContainer *pOverlays)
	: refCount(0)
{
	//	Create a copy of overlays.
	for each (Overlay *p in *pOverlays)
		overlays.push_back(p);

	Rewind();
}

OverlaysEnum::~OverlaysEnum()
{
}

STDMETHODIMP OverlaysEnum::QueryInterface(const IID &iid, void **ppv)
{
	if (IID_IUnknown == iid || IID_IOverlaysEnum == iid)
	{
		*ppv = static_cast<IOverlaysEnum*>(this);
	}
	else
	{
		*ppv = 0;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG __stdcall OverlaysEnum::AddRef()
{
	return InterlockedIncrement(&refCount);
}

ULONG __stdcall OverlaysEnum::Release()
{
	if (0 == InterlockedDecrement(&refCount))
	{
		delete this;
		return 0;
	}

	return refCount;
}

STDMETHODIMP OverlaysEnum::Rewind()
{
	current = overlays.begin();

	return S_OK;
}

STDMETHODIMP OverlaysEnum::GetNext(IFrameOverlay **ppOverlay)
{
	if (overlays.end() == current)
	{
		*ppOverlay = 0;
		return E_NOMOREOVERLAYS;
	}

	*ppOverlay = static_cast<IFrameOverlay*>(*current);
	current++;

	return S_OK;
}
