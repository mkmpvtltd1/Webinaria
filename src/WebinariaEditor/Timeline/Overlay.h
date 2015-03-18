#pragma once

#include <utility>
#include <list>

#include "FileStream.h"
#include "iRGBFilters.h"
#include "OverlayElement.h"

typedef std::pair<int, int> Span;
typedef std::list<OverlayElement *> OverlayElements;

enum OverlayHitTest
{
	ohtNone = 0,
	ohtTopBorder = 1,
	ohtBottomBorder = 2,
	ohtLeftBorder = 4,
	ohtRightBorder = 8,
	ohtTopLeftCorner = ohtLeftBorder | ohtTopBorder,
	ohtTopRightCorner = ohtRightBorder | ohtTopBorder,
	ohtBottomLeftCorner = ohtLeftBorder | ohtBottomBorder,
	ohtBottomRightCorner = ohtRightBorder | ohtBottomBorder,
	ohtInside = ohtLeftBorder | ohtRightBorder | ohtTopBorder | ohtBottomBorder,
};

class Overlay : public IFrameOverlay
{

private:
	static const int Version;								//	Data record version in file.

private:
	LONG refCount;

private:
	int start, end;
	bool deleted;
	bool enableSound, enableWebcam;
	int webcamX, webcamY;
	/*
		Elements, contained in overlay.
		First element lies on the foreground, the last one is on the background.
	*/
	OverlayElements elements;
	OverlayElement *activeElement;

	OverlayElement* CreateElementOfType(ElementType type);

public:
	void Paint(HDC dc, const RECT *rect);
	OverlayElement* FindElementFromPoint(const RECT *rect, const POINT *point, OverlayHitTest *hitTest);
	bool HitTestElement(const OverlayElement* element, const RECT *rect, const POINT *point, OverlayHitTest *hitTest);

	void SendToBack(OverlayElement *element);
	void BringToFront(OverlayElement *element);
	void Delete(OverlayElement *element);

public:
	template<class ElementType> ElementType* CreateElement(const RECT *overlayRect)
	{
		ElementType* element = new ElementType();
		element->ConfigureDefault(overlayRect);
		elements.push_front(element);

		return element;
	}

public:
	int GetStart() const;
	void SetStart(int newStart);
	int GetEnd() const;
	void SetEnd(int newEnd);
	int GetLength() const;
	OverlayElement* GetActiveElement();
	void SetActiveElement(OverlayElement *element);
	bool GetDeleted() const;
	void SetDeleted(bool value);
	bool GetEnableSound() const;
	void SetEnableSound(bool value);
	bool GetEnableWebcam() const;
	void SetEnableWebcam(bool value);
	int GetWebcamX() const;
	void SetWebcamX(int value);
	int GetWebcamY() const;
	void SetWebcamY(int value);

public:
	__declspec(property(get = GetStart, put = SetStart)) int Start;
	__declspec(property(get = GetEnd, put = SetEnd)) int End;
	__declspec(property(get = GetLength)) int Length;
	__declspec(property(get = GetActiveElement, put = SetActiveElement)) OverlayElement* ActiveElement;
	__declspec(property(get = GetDeleted, put = SetDeleted)) bool Deleted;
	__declspec(property(get = GetEnableSound, put = SetEnableSound)) bool EnableSound;
	__declspec(property(get = GetEnableWebcam, put = SetEnableWebcam)) bool EnableWebcam;
	__declspec(property(get = GetWebcamX, put = SetWebcamX)) int WebcamX;
	__declspec(property(get = GetWebcamY, put = SetWebcamY)) int WebcamY;

public:
	Overlay();
	~Overlay();

	bool Load(FileStream &in);
	bool Save(FileStream &out);

public:
	//	IFrameOverlay methods.
    virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Draw( 
        /* [in] */ FRAME_INFO *pFrameInfo)
	{
        if(!pFrameInfo) return E_POINTER;

        Paint(pFrameInfo->hdc, &pFrameInfo->rect);
		return S_OK;
	}
    
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetBoundaries( 
            /* [out] */ LONG *plBegin,
            /* [out] */ LONG *plEnd)
	{
        if(!plBegin || !plEnd) return E_POINTER;
        *plBegin = start;
        *plEnd   = end;
		return S_OK;;
	}

	//	IUnknown methods.
    STDMETHOD(QueryInterface)(const IID &iid, void **ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();

};
