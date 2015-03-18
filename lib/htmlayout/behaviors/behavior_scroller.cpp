
#include "stdafx.h"
#include "behavior_aux.h"

namespace htmlayout 
{
 
/** behavior:grid, browser of tabular data (records).
 *  Supports multiple selection mode. Use "multiple" attribute.
 *
 *   model is like this:
 *   <table fixedrows=1 ... style="behavior:grid"> 
 *     <tr>...
 *     <tr>...
 *   </table> 
 *
 *  LOGICAL EVENTS: 
 *    TABLE_HEADER_CLICK, click on some cell in table header, target is the cell
 *    TABLE_ROW_CLICK,    click on data row in the table, target is the row
 *
 *  See: html_samples/grid/scrollable-table.htm
 *  Authors: Andrew Fedoniouk, initial implementation.
 *           Andrey Kubyshev, multiselection mode.
 *
 **/
    
struct scroller: public behavior
{
    // ctor
    scroller(const char* name = "scroller"): behavior(HANDLE_MOUSE, name) {}
    
    virtual void attached  (HELEMENT he ) 
    { 
    } 

    virtual BOOL on_mouse(HELEMENT he, HELEMENT target, UINT event_type, POINT pt, UINT mouseButtons, UINT keyboardStates )
    {
      if( (event_type != (MOUSE_MOVE | SINKING)) || (mouseButtons != 0))
        return false; // only mouse move in unpressed state.

      POINT scroll_pos;
      RECT  view_rect; SIZE content_size;

      dom::element el = he;

      el.get_scroll_info(scroll_pos, view_rect, content_size);

      if( pt.x < view_rect.left || pt.x > view_rect.right || 
          pt.y < view_rect.top || pt.x > view_rect.bottom )
        return false;


      int vh = view_rect.bottom - view_rect.top;
      if( vh >= content_size.cy  )
        return false;

      int dh = content_size.cy - vh;

      int py = ( pt.y * dh) / vh;
      if( py != scroll_pos.y )
      {
        scroll_pos.y = py;
        el.set_scroll_pos(scroll_pos);
      }
      return false; 
    }


};

scroller          scroller_instance;

// force it to be included
#pragma comment(linker, FORCE_LINKAGE_OF(scroller))


}