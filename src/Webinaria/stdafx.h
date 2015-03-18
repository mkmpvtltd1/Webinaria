// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

#define _ATL_APARTMENT_THREADED
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define POINTER_64 __ptr64

#ifdef  DEBUG
#define REGISTER_FILTERGRAPH
#endif

#include <windows.h>
#include <windowsx.h>
#include <wininet.h>
#include <tchar.h>
#include <streams.h>
#include <strsafe.h>
//////////////////////
#include <dbt.h>
#include <mmreg.h>
#include <msacm.h>
#include <fcntl.h>
#include <io.h>
#include <commdlg.h>
//////////////////////
#include "resource.h"

// Use for support "clever" pointers to COM
#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>

// Standard additional macroses
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
#define ABS(x) (((x) > 0) ? (x) : -(x))

#pragma comment(lib, "HTMLayout.lib")

// Use for transparent some parts of window
#ifndef WS_EX_LAYERED 
	#define WS_EX_LAYERED   0x00080000
	#define LWA_COLORKEY	0x00000001
	#define LWA_ALPHA		0x00000002
#endif // ndef WS_EX_LAYERED 

// Window messages for filters notify
#define WM_FGNOTIFY WM_USER+1

#if (WINVER < 0x0500)

typedef struct _DEV_BROADCAST_DEVICEINTERFACE_A {
	DWORD       dbcc_size;
	DWORD       dbcc_devicetype;
	DWORD       dbcc_reserved;
	GUID        dbcc_classguid;
	char        dbcc_name[1];
} DEV_BROADCAST_DEVICEINTERFACE_A, *PDEV_BROADCAST_DEVICEINTERFACE_A;

typedef struct _DEV_BROADCAST_DEVICEINTERFACE_W {
	DWORD       dbcc_size;
	DWORD       dbcc_devicetype;
	DWORD       dbcc_reserved;
	GUID        dbcc_classguid;
	wchar_t     dbcc_name[1];
} DEV_BROADCAST_DEVICEINTERFACE_W, *PDEV_BROADCAST_DEVICEINTERFACE_W;

#ifdef UNICODE
typedef DEV_BROADCAST_DEVICEINTERFACE_W   DEV_BROADCAST_DEVICEINTERFACE;
typedef PDEV_BROADCAST_DEVICEINTERFACE_W  PDEV_BROADCAST_DEVICEINTERFACE;
#else
typedef DEV_BROADCAST_DEVICEINTERFACE_A   DEV_BROADCAST_DEVICEINTERFACE;
typedef PDEV_BROADCAST_DEVICEINTERFACE_A  PDEV_BROADCAST_DEVICEINTERFACE;
#endif // UNICODE
#endif // WINVER

// Preparation for the function we want to import from USER32.DLL 
typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
