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
 * (C) 2003-2004, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

#ifndef __htmlayout_behavior_h__
#define __htmlayout_behavior_h__

/*!\file
\brief Behaiviors support (a.k.a windowless controls)
*/
#include <windows.h>

#pragma pack(push,8)

  /** event groups.
       **/
  enum EVENT_GROUPS 
  {
      HANDLE_INITIALIZATION = 0x0000, /** attached/detached */
      HANDLE_MOUSE = 0x0001,          /** mouse events */ 
      HANDLE_KEY = 0x0002,            /** key events */  
      HANDLE_FOCUS = 0x0004,          /** focus events, if this flag is set it also means that element it attached to is focusable */ 
      HANDLE_SCROLL = 0x0008,         /** scroll events */ 
      HANDLE_TIMER = 0x0010,          /** timer event */ 
      HANDLE_SIZE = 0x0020,           /** size changed event */ 
      HANDLE_DRAW = 0x0040,           /** drawing request (event) */
      HANDLE_DATA_ARRIVED = 0x080,    /** requested data () has been delivered */
      HANDLE_BEHAVIOR_EVENT = 0x0100, /** secondary, synthetic events: 
                                        BUTTON_CLICK, HYPERLINK_CLICK, etc., 
                                        a.k.a. notifications from intrinsic behaviors */
      HANDLE_METHOD_CALL = 0x0200,    /** behavior specific methods */
      
      HANDLE_ALL = 0x03FF             /* all of them */
  };

/**Element callback function for all types of events. Similar to WndProc
 * \param tag \b LPVOID, tag assigned by HTMLayoutAttachElementProc function (like GWL_USERDATA)
 * \param he \b HELEMENT, this element handle (like HWND)
 * \param evtg \b UINT, group identifier of the event, value is one of EVENT_GROUPS
 * \param prms \b LPVOID, pointer to group specific parameters structure.
 * \return TRUE if event was handled, FALSE otherwise.
 **/

//typedef 
//  BOOL CALLBACK ElementEventProc(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms );


  enum PHASE_MASK 
  {
      BUBBLING = 0,      // bubbling (emersion) phase
      SINKING  = 0x8000  // capture (immersion) phase, this flag is or'ed with EVENTS codes below
    // see: http://www.w3.org/TR/xml-events/Overview.html#s_intro
  };

  enum MOUSE_BUTTONS 
  {
      MAIN_MOUSE_BUTTON = 1, //aka left button
      PROP_MOUSE_BUTTON = 2, //aka right button
      MIDDLE_MOUSE_BUTTON = 4,
  };

  enum KEYBOARD_STATES 
  {
      CONTROL_KEY_PRESSED = 0x1,
      SHIFT_KEY_PRESSED = 0x2,
      ALT_KEY_PRESSED = 0x4
  };

// parameters of evtg == HANDLE_INITIALIZATION

  enum INITIALIZATION_EVENTS
  {
    BEHAVIOR_DETACH = 0,
    BEHAVIOR_ATTACH = 1
  };

  struct INITIALIZATION_PARAMS
  {
    UINT cmd; // INITIALIZATION_EVENTS
  };
 

// parameters of evtg == HANDLE_MOUSE

  enum MOUSE_EVENTS
  {
      MOUSE_ENTER = 0,
      MOUSE_LEAVE,
      MOUSE_MOVE,
      MOUSE_UP,
      MOUSE_DOWN,
      MOUSE_DCLICK,
      MOUSE_WHEEL, 
      MOUSE_TICK, // mouse pressed ticks
      MOUSE_IDLE, // mouse stay idle for some time
  };

  struct MOUSE_PARAMS
  {
      UINT      cmd;          // MOUSE_EVENTS
      HELEMENT  target;       // target element
      POINT     pos;          // position of cursor, element relative
      POINT     pos_document; // position of cursor, document root relative
      UINT      button_state; // MOUSE_BUTTONS 
      UINT      alt_state;    // KEYBOARD_STATES 
      UINT      cursor_type;  // CURSOR_TYPE to set
      BOOL      is_on_icon;   // mouse is over icon (foreground-image, foreground-repeat:no-repeat)
  };

// parameters of evtg == HANDLE_KEY

  enum KEY_EVENTS
  {
      KEY_DOWN = 0,
      KEY_UP,
      KEY_CHAR
  };

  struct KEY_PARAMS
  {
      UINT      cmd;          // KEY_EVENTS
      HELEMENT  target;       // target element
      UINT      key_code;     // key scan code, or character unicode for KEY_CHAR
      UINT      alt_state;    // KEYBOARD_STATES   
  };

// parameters of evtg == HANDLE_FOCUS

  enum FOCUS_EVENTS
  {
      FOCUS_LOST = 0,
      FOCUS_GOT = 1,
  };

  struct FOCUS_PARAMS
  {
      UINT      cmd;          // FOCUS_EVENTS
      HELEMENT  target;       // target element, for FOCUS_LOST it is a handle of new focus element
      UINT      res;          // and for FOCUS_GOT it is a handle of old focus element, can be NULL
  };

// parameters of evtg == HANDLE_SCROLL

  enum SCROLL_EVENTS
  {
      SCROLL_HOME = 0,
      SCROLL_END,
      SCROLL_STEP_PLUS,
      SCROLL_STEP_MINUS,
      SCROLL_PAGE_PLUS,
      SCROLL_PAGE_MINUS,
      SCROLL_POS
  };

  struct SCROLL_PARAMS
  {
      UINT      cmd;          // SCROLL_EVENTS
      HELEMENT  target;       // target element
      INT       pos;          // scroll position if SCROLL_POS
      BOOL      vertical;     // TRUE if from vertical scrollbar
  };


  enum DRAW_EVENTS
  {
      DRAW_BACKGROUND = 0,
      DRAW_CONTENT = 1,
      DRAW_FOREGROUND = 2,
  };

  struct DRAW_PARAMS
  {
      UINT      cmd;          // DRAW_EVENTS
      HDC       hdc;          // hdc to paint on
      RECT      area;         // element area to paint,  
      UINT      reserved;     //   for DRAW_BACKGROUND/DRAW_FOREGROUND - it is a border box
                              //   for DRAW_CONTENT - it is a content box
  };
  // Use ::GetTextColor(hdc) to get current text color of the element

  enum BEHAVIOR_EVENTS
  {
      BUTTON_CLICK = 0,              // click on button
      BUTTON_PRESS = 1,              // mouse down or key down in button
      BUTTON_STATE_CHANGED = 2,      // checkbox/radio/slider changed its state/value 
      EDIT_VALUE_CHANGING = 3,       // before text change
      EDIT_VALUE_CHANGED = 4,        // after text change
      SELECT_SELECTION_CHANGED = 5,  // selection in <select> changed
      SELECT_STATE_CHANGED = 6,      // node in select expanded/collapsed, heTarget is the node

      POPUP_REQUEST   = 7,           // request to show popup just received, 
                                     //     here DOM of popup element can be modifed.
      POPUP_READY     = 8,           // popup element has been measured and ready to be shown on screen,
                                     //     here you can use functions like ScrollToView.
      POPUP_DISMISSED = 9,           // popup element is closed,
                                     //     here DOM of popup element can be modifed again - e.g. some items can be removed
                                     //     to free memory.

      MENU_ITEM_ACTIVE = 0xA,        // menu item activated by mouse hover or by keyboard
      MENU_ITEM_CLICK = 0xB,         // menu item click 

      
      CONTEXT_MENU_REQUEST = 0x10,   // "right-click", BEHAVIOR_EVENT_PARAMS::he is current popup menu HELEMENT being processed or NULL.
                                     // application can provide its own HELEMENT here (if it is NULL) or modify current menu element.
      
      VISIUAL_STATUS_CHANGED = 0x11, // broadcast notification, sent to all elements of some container being shown or hidden   


      // "grey" event codes  - notfications from behaviors from this SDK 
      HYPERLINK_CLICK = 0x80,        // hyperlink click
      TABLE_HEADER_CLICK,            // click on some cell in table header, 
                                     //     target = the cell, 
                                     //     reason = index of the cell (column number, 0..n)
      TABLE_ROW_CLICK,               // click on data row in the table, target is the row
                                     //     target = the row, 
                                     //     reason = index of the row (fixed_rows..n)
      TABLE_ROW_DBL_CLICK,           // mouse dbl click on data row in the table, target is the row
                                     //     target = the row, 
                                     //     reason = index of the row (fixed_rows..n)

      ELEMENT_COLLAPSED = 0x90,      // element was collapsed, so far only behavior:tabs is sending these two to the panels
      ELEMENT_EXPANDED,              // element was expanded,

      DO_SWITCH_TAB,                 // command to switch tab programmatically, handled by behavior:tabs 
                                     // use it as HTMLayoutPostEvent(tabsElementOrItsChild, DO_SWITCH_TAB, tabElementToShow, 0);


      FIRST_APPLICATION_EVENT_CODE = 0x100 
      // all custom event codes shall be greater
      // than this number. All codes below this will be used
      // solely by application - HTMLayout will not intrepret it 
      // and will do just dispatching.
      // To send event notifications with  these codes use
      // HTMLayoutSend/PostEvent API.

  };

  enum EVENT_REASON
  {
      BY_MOUSE_CLICK,  
      BY_KEY_CLICK, 
      SYNTHESIZED, // synthesized, programmatically generated.
  };

  enum EDIT_CHANGED_REASON
  {
      BY_INS_CHAR,  // single char insertion
      BY_INS_CHARS, // character range insertion, clipboard
      BY_DEL_CHAR,  // single char deletion
      BY_DEL_CHARS, // character range deletion (selection)
  };

  

  struct BEHAVIOR_EVENT_PARAMS
  {
      UINT     cmd;        // BEHAVIOR_EVENTS
      HELEMENT heTarget;   // target element handler
      HELEMENT he;         // source element e.g. in SELECTION_CHANGED it is new selected <option>
      UINT     reason;     // EVENT_REASON or EDIT_CHANGED_REASON - UI action causing change.
                           // In case of custom event notifications this may be any 
                           // application specific value.
  };

  // identifiers of methods currently supported by intrinsic behaviors,
  // see function HTMLayoutCallMethod 

  enum BEHAVIOR_METHOD_IDENTIFIERS
  {
    DO_CLICK = 0,
    GET_TEXT_VALUE = 1,
    SET_TEXT_VALUE,
      // p - TEXT_VALUE_PARAMS
    
    TEXT_EDIT_GET_SELECTION,
      // p - TEXT_EDIT_SELECTION_PARAMS

    TEXT_EDIT_SET_SELECTION,
      // p - TEXT_EDIT_SELECTION_PARAMS

    // Replace selection content or insert text at current caret position.
    // Replaced text will be selected. 
    TEXT_EDIT_REPLACE_SELECTION, 
      // p - TEXT_EDIT_REPLACE_SELECTION_PARAMS

    // Set value of type="vscrollbar"/"hscrollbar"
    SCROLL_BAR_GET_VALUE,
    SCROLL_BAR_SET_VALUE,

    FIRST_APPLICATION_METHOD_ID = 0x100 
  };

  struct METHOD_PARAMS 
  {
    UINT methodID; // see: #BEHAVIOR_METHOD_IDENTIFIERS
  };


  #ifdef __cplusplus 

  struct TEXT_VALUE_PARAMS: METHOD_PARAMS
  {
    LPCWSTR text;
    UINT    length;
    TEXT_VALUE_PARAMS(bool to_set = false) { methodID = to_set? SET_TEXT_VALUE: GET_TEXT_VALUE; }
  };

  struct TEXT_EDIT_SELECTION_PARAMS: METHOD_PARAMS
  {
    UINT selection_start;
    UINT selection_end;
    TEXT_EDIT_SELECTION_PARAMS(bool to_set = false) { methodID = to_set? TEXT_EDIT_SET_SELECTION: TEXT_EDIT_GET_SELECTION; }
  };

  struct TEXT_EDIT_REPLACE_SELECTION_PARAMS: METHOD_PARAMS
  {
    LPCWSTR text;
    UINT    text_length;
    TEXT_EDIT_REPLACE_SELECTION_PARAMS() { methodID = TEXT_EDIT_REPLACE_SELECTION; }
  };

  struct SCROLLBAR_VALUE_PARAMS: METHOD_PARAMS
  {
    INT     value;
    INT     min_value;
    INT     max_value;
    INT     page_value; // page increment
    INT     step_value; // step increment (arrow button click) 
    SCROLLBAR_VALUE_PARAMS(bool to_set = false) { methodID = to_set? SCROLL_BAR_SET_VALUE: SCROLL_BAR_GET_VALUE; }
  };



  #endif

  // see HTMLayoutRequestElementData

  struct DATA_ARRIVED_PARAMS
  {
      HELEMENT  initiator;    // element intiator of HTMLayoutRequestElementData request, 
      LPCBYTE   data;         // data buffer
      UINT      dataSize;     // size of data
      UINT      dataType;     // data type passed "as is" from HTMLayoutRequestElementData
  };



#pragma pack(pop)

#endif

