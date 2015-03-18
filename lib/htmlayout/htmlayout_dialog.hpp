/*
 * Terra Informatica Lightweight Embeddable HTMLayout control
 * http://terrainformatica.com/htmlayout
 * 
 * HTML dialog. 
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * (C) 2003-2004, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

/**\file
 * \brief Implementation of HTML based dialog
 **/

#ifndef __htmlayout_dialog_hpp__
#define __htmlayout_dialog_hpp__

#include "htmlayout_dom.hpp"
#include "htmlayout_behavior.hpp"
#include "htmlayout_controls.hpp"
#include "htmlayout_notifications.hpp"
#include <assert.h>
#include <shellapi.h> // ShellExecute


#include <string> 

#pragma warning(disable:4786) //identifier was truncated...
#pragma warning(disable:4996) //'strcpy' was declared deprecated
#pragma warning(disable:4100) //unreferenced formal parameter 

#pragma once

/**HTMLayout namespace.*/
namespace htmlayout
{

  class dialog: public event_handler,
                public notification_handler<dialog>
  {
  public:
     HWND           parent;
     POINT          position; 
     INT            alignment;
     UINT           style;
     UINT           style_ex;
     named_values*  pvalues;
  
    dialog(HWND hWndParent): pvalues(0),
      event_handler( HANDLE_BEHAVIOR_EVENT | HANDLE_INITIALIZATION )
    {  
      parent = hWndParent;
      position.x = 0;
      position.y = 0;
      alignment = 0; // center of desktop
      style = WS_DLGFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU;
      style_ex = WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE;
    }

    // SHOW function family - show modal dalog and return one of 
    // IDOK, IDCANCEL, etc. codes. 
    // Dialog buttons shall be defined as <button name="OK">, <button name="CANCEL">, etc.

    // show HTML dialog from url
    unsigned int show( LPCWSTR url  );
    
    // show HTML dialog from resource given by resource id
    unsigned int show( UINT html_res_id );
    
    // show HTML dialog from html in memory buffer
    unsigned int show( LPCBYTE html, UINT html_length );


    // INPUT function family - show modal dalog and return one of button codes,
    // values collection is used for initializing and returning values of inputs.

    // show HTML input dialog from url
    unsigned int input( LPCWSTR url, named_values& values  )
    {
      pvalues = &values;
      return show(url);
    }
    
    // show HTML input dialog from resource given by resource id
    unsigned int input( UINT html_res_id, named_values& values )
    {
      pvalues = &values;
      return show(html_res_id);
    }
    
    // show HTML input dialog from html in memory buffer
    unsigned int input( LPCBYTE html, UINT html_length, named_values& values )
    {
      pvalues = &values;
      return show(html, html_length);
    }

  protected:
    virtual void attached( HELEMENT he ); 
    virtual BOOL handle_event ( HELEMENT he, BEHAVIOR_EVENT_PARAMS& params ); 
    virtual BOOL handle_key ( HELEMENT he, KEY_PARAMS& params );


  };

  
// implementations 

    // show HTML dialog from url
    inline unsigned int dialog::show( LPCWSTR url ) 
    { 
      return show( (LPCBYTE) url, 0); 
    }

    // show HTML dialog from resource given by resource id
    inline unsigned int dialog::show( UINT html_res_id )
    {

      PBYTE   html;
      DWORD   html_length;
      if(!load_html_resource(html_res_id, html, html_length ))
      {
        assert(false); // resource not found!
        return 0;
      }
      return show(html, html_length);
    }
    
    // show HTML dialog from html in memory buffer
    inline unsigned int dialog::show( LPCBYTE html, UINT html_length )
    {
      return ::HTMLayoutDialog(
        parent, 
        position, 
        alignment, 
        style, style_ex, 
        &notification_handler<dialog>::callback, 
        &event_handler::element_proc,
        this, html, html_length );
    }

    inline void dialog::attached( HELEMENT he )
    {
        if(pvalues)
        {
          dom::element root = he;
          set_values(root,*pvalues);
        }
    }

    // handle button events.
    // here it does special treatment of "dialog buttons"

    inline BOOL dialog::handle_event (HELEMENT he, BEHAVIOR_EVENT_PARAMS& params ) 
      { 
        dom::element src = params.heTarget;
        if( params.cmd == BUTTON_CLICK )
        {
          const wchar_t* bname = src.get_attribute("name");

          bool positive_answer = false;
    
          UINT id = 0;
          if( wcseqi(bname,L"OK") )            { id =IDOK    ; positive_answer = true; }
          else if( wcseqi(bname,L"CANCEL") )     id =IDCANCEL;
          else if( wcseqi(bname,L"ABORT ") )     id =IDABORT ;
          else if( wcseqi(bname,L"RETRY ") )   { id =IDRETRY ; positive_answer = true; }
          else if( wcseqi(bname,L"IGNORE") )     id =IDIGNORE;
          else if( wcseqi(bname,L"YES") )      { id =IDYES   ; positive_answer = true; }
          else if( wcseqi(bname,L"NO") )         id =IDNO    ;
          else if( wcseqi(bname,L"CLOSE") )      id =IDCLOSE ;
          else if( wcseqi(bname,L"HELP") )       id =IDHELP  ; // ?
          else 
            return FALSE;

          HWND hwndLayout = src.get_element_hwnd(true);
          if( !::IsWindow(hwndLayout) )
            return FALSE;
          HWND hwndDlg = GetParent(hwndLayout);

          if(positive_answer && pvalues)
          {
            dom::element root = src.root();
            pvalues->clear();
            get_values(root,*pvalues);
          }

          EndDialog(hwndDlg, id);
          return TRUE;
        }

#if !defined( _WIN32_WCE ) 
        else if (params.cmd == HYPERLINK_CLICK )
        {
          HWND hwndLayout = src.get_element_hwnd(true);
          const wchar_t* url = src.get_attribute("href");
          if( url )
          {
            #if !defined(UNICODE) 
               ::ShellExecuteA(hwndLayout,"open", w2a(url), NULL,NULL,SW_SHOWNORMAL);
            #else
               ::ShellExecuteW(hwndLayout,L"open", url, NULL,NULL,SW_SHOWNORMAL);
            #endif
          }
        }
#endif
        return FALSE;
      }



    inline BOOL dialog::handle_key (HELEMENT he, KEY_PARAMS& params ) 
    {
       if(params.cmd != KEY_DOWN)
         return FALSE;

       dom::element root = he;

       switch(params.key_code)
       {
         case VK_RETURN:
           {
             dom::element def = root.find_first("[role='default-button']");
             if( def.is_valid() )
             {
               METHOD_PARAMS params; params.methodID = DO_CLICK;
               return def.call_behavior_method(&params)? TRUE:FALSE;
             }
             
           } break;
         case VK_ESCAPE:
           {
             dom::element def = root.find_first("[role='cancel-button']");
             if( def.is_valid() )
             {
               METHOD_PARAMS params; params.methodID = DO_CLICK;
               return def.call_behavior_method(&params)? TRUE:FALSE;
             }
           } break;
       }
       return FALSE;
    }


}

#endif