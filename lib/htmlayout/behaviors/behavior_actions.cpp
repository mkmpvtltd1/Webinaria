#include "stdafx.h"
#include "behavior_aux.h"

namespace htmlayout 
{

/*

BEHAVIOR: actions
  
  Simplistic action interpretter.

  on button clicks exectues actions described by 'action' attribute.

TYPICAL USE CASE:
   < div style="behavior:actions" >
        ....
        <button action="alert(Hello world!)" >
        .... 
   </div>
*/

struct actions: public behavior
{

    // ctor
    actions(): behavior(HANDLE_BEHAVIOR_EVENT, "actions") {}

    virtual BOOL on_event (HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason ) 
    { 
      if( type != BUTTON_CLICK )
        return FALSE;
      
      dom::element btn = target;
      const wchar_t* action = btn.get_attribute("action");
      if( !action )
        return FALSE;  // it is not a button we know about
      if(interpret_action( action) )
        return TRUE;
      return FALSE;
    }

    // Just an idea: here should go some simple interpretter.
    // If you know one - let me know.
    // For a while it is just dumb thing like this:
    bool interpret_action( const wchar_t* action )
    {
      if( wcsncmp(action, L"alert:", 6) == 0)
      {
        ::MessageBoxW(NULL,action+6,L"alert!",MB_OK);
        return true;
      }
      return false;
    }
};

// instantiating and attaching it to the global list
actions actions_instance;
// force it to be included
#pragma comment(linker, FORCE_LINKAGE_OF(actions))


} // htmlayout namespace

