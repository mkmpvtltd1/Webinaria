#include "stdafx.h"
#include "behavior_aux.h"

#include <math.h>

namespace htmlayout 
{

/*
BEHAVIOR: path
    goal: Renders content of the element with path ellipsis.
TYPICAL USE CASE:
    <td style="behavior:path; overflow-x:hidden; white-space:nowrap;text-overflow:ellipsis; ">
SAMPLE:
*/

struct path: public behavior
{
    // ctor
    path(): behavior(HANDLE_DRAW, "path") {}

    virtual BOOL on_draw   (HELEMENT he, UINT draw_type, HDC hdc, const RECT& rc ) 
    { 
      if( draw_type != DRAW_CONTENT )
        return FALSE; /*do default draw*/ 

      dom::element el = he;
      
      UINT pta = GetTextAlign(hdc);
      SetTextAlign(hdc, TA_LEFT | TA_TOP |TA_NOUPDATECP); 
      DrawTextW(hdc, el.text(), -1, const_cast<RECT*>(&rc), DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_PATH_ELLIPSIS);
      SetTextAlign(hdc, pta); 
      return TRUE; /*skip default draw as we did it already*/ 
    }
  
};

// instantiating and attaching it to the global list
path path_instance;

// force it to be included
#pragma comment(linker, FORCE_LINKAGE_OF(path))

} // htmlayout namespace


