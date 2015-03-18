#include "stdafx.h"
#include "behavior_aux.h"

#include <commctrl.h> // for tooltip support

namespace htmlayout 
{

/*
BEHAVIOR: popup
    goal: 
          
          
TYPICAL USE CASE:
    
SAMPLE:
*/

struct dropdown: public behavior
{
    // ctor
    dropdown(): behavior(HANDLE_MOUSE, "dropdown") {}
    
    virtual BOOL on_mouse(HELEMENT he, HELEMENT target, UINT event_type, POINT pt, UINT mouseButtons, UINT keyboardStates )
    {
      dom::element el = he;
      
      switch( event_type )
      {
        case MOUSE_DOWN: 
          {
            dom::element popup_el = el.find_first(".popup,popup"); // either class or element <popup>
            if( popup_el.is_valid() && el.is_valid() )
            {
              el.set_attribute("popup-shown",L""); // set popup-shown attribute 
                     // to indicate that popup has been shown
                     // this may be used by CSS to indicate UI state
              el.update(true); // render state
              ::HTMLayoutShowPopup(popup_el,el,2); // show it below
              popup_el.set_state(0,STATE_FOCUS);
              el.set_attribute("popup-shown",0);   // remove popup-shown attribute 
            }
            return true;
          }
          break;
      }
      return false;
    }
  
};

struct popup: public behavior
{
    // ctor
    popup(): behavior(HANDLE_MOUSE | HANDLE_BEHAVIOR_EVENT | HANDLE_FOCUS, "popup") {}

    virtual BOOL on_mouse(HELEMENT he, HELEMENT target, UINT event_type, POINT pt, UINT mouseButtons, UINT keyboardStates )
    {
      switch( event_type )
      {
        case MOUSE_DOWN:
          return true;
        case MOUSE_UP:
          // notification could be done by command behavior
          // ... 
          // and close window
          ::HTMLayoutHidePopup(he);
          return true;
      }
      return false; 
    }
 
    virtual BOOL on_event (HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason ) 
    { 
      switch( type )
      {
      case BUTTON_CLICK:
      case HYPERLINK_CLICK:
         ::HTMLayoutHidePopup(he);
         break;
      }
      return false;
    }

    bool belongs_to( dom::element parent, dom::element child )
    {
      if( !child.is_valid()) 
        return false;
      if( parent == child )
        return true;
      return belongs_to( parent, child.parent() );
    }
   
    virtual BOOL on_focus  (HELEMENT he, HELEMENT target, UINT event_type ) 
    { 
      if( (event_type == FOCUS_LOST) || (event_type == (FOCUS_LOST | SINKING)) )
      {
        if( !belongs_to( he, target ) )
          ::HTMLayoutHidePopup(he);
      }
      return TRUE; 
    }

};


// instantiating and attaching it to the global list
dropdown dropdown_instance;
popup popup_instance;

// force it to be included
#pragma comment(linker, FORCE_LINKAGE_OF(dropdown))
#pragma comment(linker, FORCE_LINKAGE_OF(popup))


} // htmlayout namespace
