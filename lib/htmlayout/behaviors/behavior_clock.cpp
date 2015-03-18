#include "stdafx.h"
#include "behavior_aux.h"

#include <math.h>

namespace htmlayout 
{

/*
BEHAVIOR: clock
    goal: Renders analog clock according to current tome.
TYPICAL USE CASE:
    <img style="behavior:clock">
SAMPLE:
*/

struct clock: public behavior
{
    // ctor
    clock(): behavior(HANDLE_TIMER | HANDLE_DRAW | HANDLE_BEHAVIOR_EVENT, "clock") {}

    virtual void attached  (HELEMENT he ) 
    { 
      dom::element el = he;
      if(el.visible())
        HTMLayoutSetTimer( he, 1000 ); // set one second timer      
    } 
   
    virtual void detached  (HELEMENT he ) 
    { 
      HTMLayoutSetTimer( he, 0 ); // remove timer
    } 

    virtual BOOL on_timer  (HELEMENT he ) 
    { 
      dom::element el = he;
      RECT rc = el.get_location(false);
      HWND hw = el.get_element_hwnd(false);
      if ( ::IsWindow(hw) )
        ::InvalidateRect(hw, &rc, FALSE);
      else 
        return FALSE; // stop timer.
      return TRUE; /*keep going*/ 
    }

    virtual BOOL on_event (HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason ) 
    {
      if( type == VISIUAL_STATUS_CHANGED )
      {
        if(reason /* shown */)
          HTMLayoutSetTimer( he, 1000 ); // set one second timer      
        else
          HTMLayoutSetTimer( he, 0 ); // remove timer
      }
      return false;
    }

    virtual BOOL on_draw   (HELEMENT he, UINT draw_type, HDC hdc, const RECT& rc ) 
    { 
      if( draw_type != DRAW_CONTENT )
        return FALSE; /*do default draw*/ 

      // we can get color settings from attributes but for simplicity lets use 
      // default values
            
      SYSTEMTIME st;
      GetLocalTime(&st);
      //Use GetSystemTime(&st); - if you need clocks in UTC or other TZ
      //you may use something like get_attribute("timezone")

      draw_clock_hand(hdc,rc, (st.wHour * 360) / 12, 0);
      draw_clock_hand(hdc,rc, (st.wMinute * 360) / 60, 1);
      draw_clock_hand(hdc,rc, (st.wSecond * 360) / 60, 2);
      
      return TRUE; /*skip default draw as we did it already*/ 
    }

    void draw_clock_hand( HDC hdc, const RECT& rc, int angle_degree, int hand )
    {
       double radians = (2 * 3.1415926 * (angle_degree - 90)) / 360.0 ;
       int radius = min( rc.bottom - rc.top, rc.right - rc.left ) / 2 - 12;
       
       HPEN hp = 0;

       switch(hand)
       {
        case 0: // hours
           radius = (radius * 3) / 4; 
           hp = CreatePen(::GetTextColor(hdc),5);
           break;
        case 1: // minutes
           hp = CreatePen(::GetTextColor(hdc),3);
           break;
        case 2: // seconds
           hp = CreatePen(RGB(0x7f,0,0),1);
           break;
        default:
           assert(false);
       }

       int y = (rc.bottom + rc.top) / 2;
       int x = (rc.right + rc.left) / 2;

       int xe = x + int(cos(radians) * radius);
       int ye = y + int(sin(radians) * radius);

       HGDIOBJ hpBefore = ::SelectObject(hdc,hp);
       ::MoveToEx(hdc,x,y,0);
       ::LineTo(hdc,xe,ye);
       ::SelectObject(hdc,hpBefore);
       ::DeleteObject(hp);

    }

    HPEN CreatePen(COLORREF c, int width)
    {
#ifndef _WIN32_WCE 
       LOGBRUSH logbrush = { BS_SOLID, RGB(0,0,0), 0 };
       logbrush.lbColor = c;
       HPEN hp = ::ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_ROUND, width, &logbrush, 0,0);
       return hp;
#else
       return ::CreatePen( PS_SOLID, width, c );
#endif
    }
   
};

// instantiating and attaching it to the global list
clock clock_instance;

// force it to be included
#pragma comment(linker, FORCE_LINKAGE_OF(clock))

} // htmlayout namespace


