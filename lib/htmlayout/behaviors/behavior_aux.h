#include "..\htmlayout.h"
#include "..\htmlayout_dom.h"
#include "..\htmlayout_behavior.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "notifications.h"

inline bool equal(const wchar_t* val, const wchar_t* str)
{
  if(!val)
    return false;
  return wcscmp(val,str) == 0;
}

inline int width( const RECT& rc ) { return rc.right - rc.left; }
inline int height( const RECT& rc ) { return rc.bottom - rc.top; }


// some functions used in different behaviors
// implemented in command.cpp
extern void ShowTooltip (HWND hwnd, RECT rc, LPCWSTR toolText);
extern void HideTooltip ();

//#pragma comment(linker, "/include:?select_checkmark_instance@htmlayout@@3Uselect_checkmark@1@A")
#define FORCE_LINKAGE_OF(BEHAVIOR_CLASS) "/include:?" #BEHAVIOR_CLASS "_instance@htmlayout@@3U" #BEHAVIOR_CLASS "@1@A"

