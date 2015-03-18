#pragma once

#include <list>

#include "Timeline.h"
#include "Overlay.h"
#include "iRGBFilters.h"

class OverlaysEnum : public IOverlaysEnum
{

private:
	LONG refCount;

private:
	//	Container will hold a copy of timeline's overlays, so no need to 
	//	destroy overlays in the destructor.
	std::list<Overlay*> overlays;							//	No matter what OverlaysContainer is, we're using list<T> here.
	std::list<Overlay*>::iterator current;

public:
	OverlaysEnum(OverlaysContainer *pOverlays);
	~OverlaysEnum();

public:
	//	IOverlaysEnum methods.
	STDMETHOD(Rewind)();
	STDMETHOD(GetNext)(IFrameOverlay **ppOverlay);

	//	IUnknown methods.
    STDMETHOD(QueryInterface)(const IID &iid, void **ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();
};
