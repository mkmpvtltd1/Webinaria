/*
 * Terra Informatica Lightweight Embeddable HTMLayout control
 * http://terrainformatica.com/htmlayout
 * 
 * Behaviors support (a.k.a windowless controls)
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * 
 * (C) 2003-2005, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

#ifndef __htmlayout_behavior_hpp__
#define __htmlayout_behavior_hpp__

/*!\file
\brief Behaiviors support (a.k.a windowless controls), C++ wrapper
*/
#include <windows.h>
#include <assert.h>
#include "htmlayout.h"

#pragma warning(disable:4786) //identifier was truncated...
#pragma warning(disable:4996) //'strcpy' was declared deprecated
#pragma warning(disable:4100) //unreferenced formal parameter 

namespace htmlayout 
{

  // event handler which can be attached to any DOM element.
  // event handler can be attached to the element as a "behavior" (see below)
  // or by htmlayout::dom::element::attach( event_handler* eh )

  struct event_handler
  {

    event_handler(UINT subsriptions) // EVENT_GROUPS flags
      :subscribed_to(subsriptions)
    {
    }

    virtual void detached  (HELEMENT /*he*/ ) { } 
    virtual void attached  (HELEMENT /*he*/ ) { } 
    virtual event_handler* attach (HELEMENT /*he*/ ) { return this; } 

    // handlers with extended interface
    // by default they are calling old set of handlers (for compatibility with legacy code)
    
    virtual BOOL handle_mouse  (HELEMENT he, MOUSE_PARAMS& params ) 
      { 
        return on_mouse( he, params.target, params.cmd, params.pos, params.button_state, params.alt_state ); 
      }
    virtual BOOL handle_key    (HELEMENT he, KEY_PARAMS& params ) 
      { 
        return on_key( he, params.target, params.cmd, params.key_code, params.alt_state );
      }
    virtual BOOL handle_focus  (HELEMENT he, FOCUS_PARAMS& params ) 
      { 
        return on_focus( he, params.target, params.cmd ); 
      }
    virtual BOOL handle_timer  (HELEMENT he ) 
      { 
        return on_timer( he );
      }
    virtual void handle_size  (HELEMENT he ) 
      { 
        on_size( he );
      }

    virtual BOOL handle_draw   (HELEMENT he, DRAW_PARAMS& params ) 
      { 
        return on_draw(he, params.cmd, params.hdc, params.area );
      }

    virtual BOOL handle_method_call (HELEMENT he, METHOD_PARAMS& params ) 
      { 
        return on_method_call(he, params.methodID, &params );
      }

    // notification events from builtin behaviors - synthesized events: BUTTON_CLICK, VALUE_CHANGED
    // see enum BEHAVIOR_EVENTS
    virtual BOOL handle_event (HELEMENT he, BEHAVIOR_EVENT_PARAMS& params ) 
      { 
        return on_event(he, params.heTarget, (BEHAVIOR_EVENTS)params.cmd, params.reason );
      }

    // notification event: data requested by HTMLayoutRequestData just delivered
    virtual BOOL handle_data_arrived (HELEMENT he, DATA_ARRIVED_PARAMS& params ) 
      { 
        return on_data_arrived(he, params.initiator, params.data, params.dataSize, params.dataType ); 
      }

    //
    // alternative set of event handlers (aka old set).
    //
    virtual BOOL on_mouse  (HELEMENT he, HELEMENT target, UINT event_type, POINT pt, UINT mouseButtons, UINT keyboardStates ) { return FALSE; }
    virtual BOOL on_key    (HELEMENT he, HELEMENT target, UINT event_type, UINT code, UINT keyboardStates ) { return FALSE; }
    virtual BOOL on_focus  (HELEMENT he, HELEMENT target, UINT event_type ) { return FALSE; }
    virtual BOOL on_timer  (HELEMENT he ) { return FALSE; /*stop this timer*/ }
    virtual BOOL on_draw   (HELEMENT he, UINT draw_type, HDC hdc, const RECT& rc ) { return FALSE; /*do default draw*/ }
    virtual void on_size   (HELEMENT he ) { }

    virtual BOOL on_method_call (HELEMENT he, UINT methodID, METHOD_PARAMS* params ) { return FALSE; /*not handled*/ }

    // notification events from builtin behaviors - synthesized events: BUTTON_CLICK, VALUE_CHANGED
    // see enum BEHAVIOR_EVENTS
    virtual BOOL on_event (HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason ) { return FALSE; }

    // notification event: data requested by HTMLayoutRequestData just delivered
    virtual BOOL on_data_arrived (HELEMENT he, HELEMENT initiator, LPCBYTE data, UINT dataSize, UINT dataType ) { return FALSE; }


    // ElementWventProc implementeation
    static BOOL CALLBACK  element_proc(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms )
    {
      event_handler* pThis = static_cast<event_handler*>(tag);
      if( pThis ) switch( evtg )
        {
          case HANDLE_INITIALIZATION:
            {
              INITIALIZATION_PARAMS *p = (INITIALIZATION_PARAMS *)prms;
              if(p->cmd == BEHAVIOR_DETACH)
                pThis->detached(he);
              else if(p->cmd == BEHAVIOR_ATTACH)
                pThis->attached(he);
              return TRUE;
            }
          case HANDLE_MOUSE: {  MOUSE_PARAMS *p = (MOUSE_PARAMS *)prms; return pThis->handle_mouse( he, *p );  }
          case HANDLE_KEY:   {  KEY_PARAMS *p = (KEY_PARAMS *)prms; return pThis->handle_key( he, *p ); }
          case HANDLE_FOCUS: {  FOCUS_PARAMS *p = (FOCUS_PARAMS *)prms; return pThis->handle_focus( he, *p ); }
          case HANDLE_DRAW:  {  DRAW_PARAMS *p = (DRAW_PARAMS *)prms; return pThis->handle_draw(he, *p ); }
          case HANDLE_TIMER: {  return pThis->on_timer(he); }
          case HANDLE_BEHAVIOR_EVENT:   { BEHAVIOR_EVENT_PARAMS *p = (BEHAVIOR_EVENT_PARAMS *)prms; return pThis->handle_event(he, *p ); }
          case HANDLE_METHOD_CALL:      { METHOD_PARAMS *p = (METHOD_PARAMS *)prms; return pThis->handle_method_call(he, *p ); }
          case HANDLE_DATA_ARRIVED:     { DATA_ARRIVED_PARAMS *p = (DATA_ARRIVED_PARAMS *)prms; return pThis->handle_data_arrived(he, *p ); }
          case HANDLE_SIZE:  {  pThis->handle_size(he); return FALSE; }

          case HANDLE_SCROLL:

            return FALSE; // for a while they are empty

          default:
            assert(false);
        }
      return FALSE;
    }

    UINT             subscribed_to;
  };

  // "manually" attach event_handler proc to the DOM element 
  inline void attach_event_handler(HELEMENT he, event_handler* p_event_handler, UINT subscription = HANDLE_ALL )
  {
    HTMLayoutAttachEventHandlerEx(he, &event_handler::element_proc, p_event_handler, subscription);
  }

  inline void detach_event_handler(HELEMENT he, event_handler* p_event_handler )
  {
    HTMLayoutDetachEventHandler(he, &event_handler::element_proc, p_event_handler);
  }

  


  //
  // "behavior" is a named event_handler
  // behaviors organized into one global list to be processed
  // automaticly while handling HLN_ATTACH_BEHAVIOR notification
  //

  struct behavior: event_handler 
  {

    behavior(UINT subsriptions, const char* external_name)
      :next(0),name(external_name), event_handler(subsriptions)
    {
      // add this implementation to the list (singleton)
      next = root();
      root(this);
    }

    // behavior list support
    behavior*        next;
    const char*      name; // name must be a pointer to a static string

    // returns behavior implementation by name.
    static event_handler* find(const char* name, HELEMENT he)
    {
      for(behavior* t = root(); t; t = t->next)
        if(strcmp(t->name,name)==0)
        {
          return t->attach(he);
        }
      return 0; // not found
    }
    // implementation of static list of behaviors  
    static behavior* root(behavior* to_set = 0)
    {
      static behavior* _root = 0;
      if(to_set) _root = to_set;
      return _root;
    }

    // standard implementation of HLN_ATTACH_BEHAVIOR notification
    static bool handle( LPNMHL_ATTACH_BEHAVIOR lpab )
    {
      htmlayout::event_handler *pb = htmlayout::behavior::find(lpab->behaviorName, lpab->element);
      if(pb) 
      {
        lpab->elementTag  = pb;
        lpab->elementProc = htmlayout::event_handler::element_proc;
        lpab->elementEvents = pb->subscribed_to;
        return true;
      }
      return false;
    }

  };


} //namespace htmlayout



#endif

