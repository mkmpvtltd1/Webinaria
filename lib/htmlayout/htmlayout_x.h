#ifndef __htmlayoutex_h__
#define __htmlayoutex_h__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>

#include "htmlayout.h"

// handle to extended DIB object
typedef void *    HDIBEX;

typedef enum tagHLRESULT
{
  HLR_OK = 0,
  HLR_INVALID_HANDLE,
  HLR_INVALID_FORMAT,
  HLR_FILE_NOT_FOUND,
  HLR_INVALID_PARAMETER,
  HLR_INVALID_STATE, // attempt to do operation on empty document
} HLRESULT;

#ifdef __cplusplus
inline COLORREF SYSCOLOR(int SysColorId) { return SysColorId | 0xFF000000; }
#else
#define SYSCOLOR( SysColorId ) ( SysColorId | 0xFF000000 )
#endif

// setHdibMargins stretch flags
#define DIBEX_STRETCH_TOP           0x1
#define DIBEX_STRETCH_LEFT          0x2
#define DIBEX_STRETCH_CENTER        0x4
#define DIBEX_STRETCH_RIGHT         0x8
#define DIBEX_STRETCH_BOTTOM        0x10

#ifdef __cplusplus

typedef struct tagHTMLAYOUT_HDIB_API
{
  virtual HLRESULT __stdcall loadHdibFromFile(LPCSTR path, HDIBEX* pOutHDib) = 0;
  virtual HLRESULT __stdcall loadHdibFromMemory(LPCBYTE dataptr, DWORD datasize, HDIBEX* pOutHDib) = 0;
  virtual HLRESULT __stdcall setHdibMargins(HDIBEX hdib, int marginLeft, int marginTop, int marginRight, int marginBottom, int stretchFlags) = 0;
  virtual HLRESULT __stdcall destroyHdib(HDIBEX hdib) = 0;
  virtual HLRESULT __stdcall getHdibInfo(HDIBEX hdib, LPBITMAPINFO pBbitmapInfo) = 0;
  virtual HLRESULT __stdcall getHdibBits(HDIBEX hdib,LPBYTE *ppBytes, LPDWORD pNumOfBytes) = 0;
  virtual HLRESULT __stdcall renderHdib(HDC dstDC, int dstX, int dstY, int dstWidth, int dstHeight, HDIBEX hdib) = 0;
  virtual HLRESULT __stdcall renderHdib(HDC dstDC, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, HDIBEX hdib) = 0;
  virtual HLRESULT __stdcall renderHdib(HDC dstDC, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight, HDIBEX hdib) = 0;
  virtual HLRESULT __stdcall setColorSchema(HDIBEX hdib,COLORREF DkShadow,COLORREF Shadow,COLORREF Face,COLORREF Light, COLORREF HighLight) = 0;
} HTMLAYOUT_HDIB_API;

#endif //__cplusplus

#define WM_HL_GET_INTERFACE (WM_USER + 0xaff)

#define HLINTERFACE_HDIB_API 0xAFED
#define HLINTERFACE_PRINTING_API (0xAFED + 1) 

#ifdef __cplusplus 
class  dibex 
{
  HTMLAYOUT_HDIB_API* papi;
  HDIBEX hdibex;

public:
  dibex():hdibex(0) 
  {
    papi = (HTMLAYOUT_HDIB_API*) HTMLayoutProc(0, WM_HL_GET_INTERFACE, HLINTERFACE_HDIB_API, 17246);
    assert(papi);
  }
  ~dibex() { destroy(); }
  
  void destroy() 
  { 
    HLRESULT hr;
	  hr = papi->destroyHdib(hdibex);
    assert(hr == HLR_OK); hr;
    hdibex = 0;
  }
  bool load(LPCSTR path)
  {
    HLRESULT hr = papi->loadHdibFromFile(path,&hdibex);
    assert(hr == HLR_OK);
    return hr == HLR_OK;
  }
  bool load(LPCBYTE dataptr, DWORD datasize)
  {
    HLRESULT hr = papi->loadHdibFromMemory(dataptr,datasize,&hdibex);
    assert(hr == HLR_OK);
    return hr == HLR_OK;
  }
  bool set_margins(int left, int top, int right, int bottom, int stretch_flags)
  {
    HLRESULT hr = papi->setHdibMargins(hdibex, left, top, right, bottom, stretch_flags);
    assert(hr == HLR_OK);
    return hr == HLR_OK;
  }
  int  width()
  {
    BITMAPINFO bmi;
    HLRESULT hr;
	  hr = papi->getHdibInfo(hdibex, &bmi);
    assert(hr == HLR_OK); hr;
    return bmi.bmiHeader.biWidth;
  }
  int  height()
  {
    BITMAPINFO bmi;
    HLRESULT hr;
	  hr = papi->getHdibInfo(hdibex, &bmi);
    assert(hr == HLR_OK); hr;
    return bmi.bmiHeader.biHeight;
  }
  void render(HDC dstDC, const RECT& dst)
  {
    HLRESULT hr;
	  hr = papi->renderHdib(dstDC, 
      dst.left, 
      dst.top, 
      dst.right - dst.left + 1,
      dst.bottom - dst.top + 1, hdibex);      
    assert(hr == HLR_OK); hr;
  }
  void render(HDC dstDC, const POINT& dst)
  {
    BITMAPINFO bmi;
    HLRESULT hr = papi->getHdibInfo(hdibex, &bmi);
    assert(hr == HLR_OK);
    hr = papi->renderHdib(dstDC, 
      dst.x, 
      dst.y, 
      bmi.bmiHeader.biWidth,
      bmi.bmiHeader.biHeight, hdibex);      
    assert(hr == HLR_OK); hr;
  }
  void render(HDC dstDC, const RECT& dst, const POINT& src)
  {
    HLRESULT hr;
	  hr = papi->renderHdib(dstDC, 
      dst.left, 
      dst.top, 
      dst.right - dst.left + 1,
      dst.bottom - dst.top + 1, src.x, src.y, hdibex);      
    assert(hr == HLR_OK); hr;
  }
  void render(HDC dstDC, const RECT& dst, const RECT& src)
  {
    HLRESULT hr;
	  hr = papi->renderHdib(dstDC, 
      dst.left, 
      dst.top, 
      dst.right - dst.left + 1,
      dst.bottom - dst.top + 1, 
      src.left, src.right, 
      src.right - src.left + 1,
      src.bottom - src.top + 1, 
      hdibex);      
    assert(hr == HLR_OK); hr;
  }

  void set_color_schema(COLORREF DkShadow,COLORREF Shadow,COLORREF Face,COLORREF Light, COLORREF HighLight)
  {
    HLRESULT hr;
	  hr = papi->setColorSchema(hdibex,DkShadow,Shadow,Face,Light,HighLight);
    assert(hr == HLR_OK); hr;
  }

};

#endif //__cplusplus 

// handle to extended PRINTEX object
typedef void *    HPRINTEX;

#ifdef __cplusplus

//################################################################
//#
//# ATTENTION! HTMLAYOUT_PRINTING_API is working but OBSOLETE 
//# Please use htmprint.h instead.  
//#
//################################################################


typedef struct tagHTMLAYOUT_PRINTING_API
{
  virtual HLRESULT __stdcall createInstance(HPRINTEX* pOutHPrint) = 0;
  virtual HLRESULT __stdcall destroyInstance(HPRINTEX hPrint) = 0;

  virtual HLRESULT __stdcall loadHtmlFromMemory(HPRINTEX hPrint, LPCSTR baseURI, LPCBYTE dataptr, DWORD datasize) = 0;
  virtual HLRESULT __stdcall loadHtmlFromFile(HPRINTEX hPrint, LPCSTR path) = 0;

  virtual HLRESULT __stdcall getDocumentMinWidth(HPRINTEX hPrint, LPDWORD minWidth) = 0;
  virtual HLRESULT __stdcall getDocumentHeight(HPRINTEX hPrint, LPDWORD height) = 0;

  virtual HLRESULT __stdcall measure(HPRINTEX hPrint, HDC hdc,
          int scaledWidth,    // number of screen pixels in viewportWidth
          int viewportWidth,  // width of rendering area in device (physical) units  
          int viewportHeight,  // height of rendering area in device (physical) units  
          int* pOutNumberOfPages) = 0;

  virtual HLRESULT __stdcall render(HPRINTEX hPrint, HDC hdc, int viewportX, int viewportY, int pageNo) = 0;
  virtual HLRESULT __stdcall setDataReady(HPRINTEX hPrint, LPCSTR url, LPCBYTE data, DWORD dataSize) = 0;

  virtual HLRESULT __stdcall setOption(HPRINTEX hPrint, DWORD optId, DWORD_PTR optValue) = 0;  

} HTMLAYOUT_PRINTING_API,*LPHTMLAYOUT_PRINTING_API;

#endif //__cplusplus

// setOption optId values
typedef enum tagPRINTEX_OPTIONS 
{
  SET_CALLBACK = 0,   // optValue = DWORD  - LPHTMLAYOUT_PRINTING_CALLBACK
} PRINTEX_OPTIONS;

#ifdef __cplusplus

// HTMENGINE PRINTING CALLBACK interface
typedef struct tagHTMLAYOUT_PRINTING_CALLBACK
{
  virtual BOOL __stdcall loadData(HPRINTEX hPrint, LPCSTR url) = 0;
  // return FALSE if you dont want to load any images
  // return TRUE if you want image to be provessed. You may call set_data_ready() to supply your own
  // image content
  virtual VOID __stdcall hyperlinkArea(HPRINTEX hPrint, RECT* area, LPCSTR url) = 0;
  // HtmLayout will call this callback each time when it renderes hyperlinked area
} HTMLAYOUT_PRINTING_CALLBACK, *LPHTMLAYOUT_PRINTING_CALLBACK;

#endif //__cplusplus

#ifdef __cplusplus 

// C++ wrapper 


class  printex: public HTMLAYOUT_PRINTING_CALLBACK
{
  HTMLAYOUT_PRINTING_API* papi;
  HPRINTEX                hprintex;

  // HTMLAYOUT_PRINTING_CALLBACK methods
  virtual BOOL __stdcall loadData(HPRINTEX /*hPrint*/, LPCSTR url)
  {
    return loadUrlData(url);
  }
  virtual VOID __stdcall hyperlinkArea(HPRINTEX hPrint, RECT* area, LPCSTR url)
  {
	(void)hPrint;
    registerHyperlinkArea(area, url);
  }

public:
  printex():hprintex(0) 
  {
    papi = (HTMLAYOUT_PRINTING_API*) HTMLayoutProc(0, WM_HL_GET_INTERFACE, HLINTERFACE_PRINTING_API, 3172);
    assert(papi);
    HLRESULT hr = papi->createInstance(&hprintex);
    assert(hr == HLR_OK);
    if(hr == HLR_OK)
    {
      // connect callback interface
      papi->setOption(hprintex,SET_CALLBACK,DWORD_PTR(this));
    }
  }
  ~printex() { destroy(); }
  
  void destroy() 
  { 
    HLRESULT hr;
	hr = papi->destroyInstance(hprintex);
    assert(hr == HLR_OK);
    hprintex = 0;
  }
  bool load(LPCSTR path)
  {
    HLRESULT hr = papi->loadHtmlFromFile(hprintex,path);
    assert(hr == HLR_OK);
    return hr == HLR_OK;
  }
  bool load(LPBYTE dataptr, DWORD datasize, LPCSTR baseURI)
  {
    HLRESULT hr = papi->loadHtmlFromMemory(hprintex, baseURI, dataptr,datasize);
    assert(hr == HLR_OK);
    return hr == HLR_OK;
  }

  int  measure(HDC hdc,
          int scaledWidth,    // number of screen pixels in viewportWidth
          int viewportWidth,  // width of rendering area in device (physical) units  
          int viewportHeight) // height of rendering area in device (physical) units  
          //return number of pages
  {
    int numPages = 0;
    HLRESULT hr = papi->measure(hprintex, hdc, scaledWidth, viewportWidth, viewportHeight, &numPages);
    assert(hr == HLR_OK);
    return (hr == HLR_OK)?numPages:0;
  }

  bool  render(HDC hdc, int viewportX, int viewportY, int pageNo)
  {
    HLRESULT hr = papi->render(hprintex, hdc,viewportX, viewportY, pageNo);
    assert(hr == HLR_OK);
    return hr == HLR_OK;
  }

  bool  setDataReady(LPCSTR url, LPCBYTE data, DWORD dataSize)
  {
    HLRESULT hr = papi->setDataReady(hprintex, url, data, dataSize);
    assert(hr == HLR_OK);
    return hr == HLR_OK;
  }
  
  // Get current document measured height for width
  // given in measure scaledWidth/viewportWidth parameters.
  // ATTN: You need call first measure to get valid result.
  // retunrn value is in screen pixels.

  DWORD getDocumentHeight()
  {
    DWORD v;
    HLRESULT hr = papi->getDocumentHeight(hprintex, &v);
    assert(hr == HLR_OK); 
    return (hr == HLR_OK)?v:0;
  }

  // Get current document measured minimum (intrinsic) width.
  // ATTN: You need call first measure to get valid result.
  // return value is in screen pixels.

  DWORD getDocumentMinWidth()
  {
    DWORD v;
    HLRESULT hr = papi->getDocumentMinWidth(hprintex, &v);
    assert(hr == HLR_OK); 
    return (hr == HLR_OK)?v:0;
  }
  
  // override this if you need other image loading policy
  virtual bool loadUrlData(LPCSTR url) 
  {
    url;
    return true; // proceed with default image loader
    
    /* other options are: */

    /* discard image loading at all:
    return false;
    */

    /* to load data from your own namespace simply call: 
    set_data_ready(url,data,dataSize);
    return true;
    */
  }

  // override this if you want some special processing for hyperlinks. 
  virtual void registerHyperlinkArea(const RECT* area, LPCSTR url)
  {
    area;url;
    //e.g. PDF output.
  }


};
#endif //__cplusplus 

#endif