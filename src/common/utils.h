// utils.h 
// there are set of common small defines and helpers
#pragma once

#include <evcode.h>

#define N_ELEMENTS(x) (sizeof(x)/sizeof(x[0]))
#define EC_PROGRESS		(EC_USER+101)

#define g_DefaultWebcamX 0
#define g_DefaultWebcamY 100

// TruncatePath
// Truncates fill path from "c:\\some\folder\somefile.ext" to "c:\\some\folder\"
// Len is size in TCHARs
size_t TruncatePath(TCHAR* tcPath, size_t Len);

// GetAppFolder
// Returns folder where OurApplication.exe located. 
// Len is size in TCHARs
size_t GetAppFolder(TCHAR* tcPath, size_t Len);
bool __fastcall FileExists(const TCHAR* tcName);


inline HRESULT CreateFilter(REFGUID clsidFilter, IBaseFilter** ppBaseFilter)
{
	return CoCreateInstance( clsidFilter, NULL, CLSCTX_INPROC, IID_IBaseFilter, (VOID**) ppBaseFilter);
}
HRESULT __fastcall CreateAndInsertFilter(	IGraphBuilder *pFilterGraph,
											REFGUID clsidFilter, 
                                            IBaseFilter** ppBaseFilter,
                                            LPCWSTR lpcwstrName);

HRESULT __fastcall GetUnconnectedPin(	IBaseFilter *pFilter,   // Pointer to the filter.
										PIN_DIRECTION PinDir,   // Direction of the pin to find.
										IPin **ppPin);          // Receives a pointer to the pin.

HRESULT __fastcall JoinFilterToChain( /*in*/IGraphBuilder *pFilterGraph,
									 /*in*/IBaseFilter* Filter, 
									 /*in, out*/IPin **LastPin);
HRESULT __fastcall JoinFilterToChain(/*in*/IGraphBuilder *pFilterGraph,
									 /*in*/IBaseFilter* Filter, 
									 /*in*/LPCWSTR FilterName, 
									 /*in, out*/IPin **LastPin);
void __fastcall NukeDownstream(IBaseFilter * pf, IGraphBuilder * pGB);
HRESULT __fastcall RemoveAndDeleteFilter(IGraphBuilder *pFilterGraph, IBaseFilter **ppFilter);
HRESULT __fastcall DemolishGraphFilters(IGraphBuilder *pFilterGraph);
HRESULT __fastcall GetFrameSize(IPin* pPin, LONG* plWidth, LONG* plHeight);
enum PinType {ptUnknown = 0, ptVideo, ptAudio};

PinType __fastcall GetPinType(IPin* pPin);
#ifdef DEBUG
void __fastcall AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void __fastcall RemoveGraphFromRot(DWORD pdwRegister);
#else
#define AddGraphToRot(x,y)
#define RemoveGraphFromRot(x)
#endif

int RectWidth(const RECT &rect);
int RectHeight(const RECT &rect);
RECT NewRect(int x, int y, int width, int height);
void DrawRect(HDC dc, const RECT *rc);

void GetCursorPosition(HWND hwnd, POINT *pt);

void CenterWindow(HWND wnd);

TCHAR* _tcstrim(TCHAR *p);
