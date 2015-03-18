/** \mainpage Terra Informatica Lightweight Embeddable HTMLayout control.
 *
 * \section legal_sec In legalese
 *
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 *
 * <a href="http://terrainformatica.com/htmlayout">HTMLayout Home</a>
 *
 * (C) 2003-2006, Terra Informatica Software, Inc. and Andrew Fedoniouk
 *
 * \section structure_sec Structure of the documentation
 *
 * See <a href="files.html">Files</a> section.
 **/

#ifndef __htmlayout_h__
#define __htmlayout_h__

/**\file htmlayout.h
 * Main include file.
 **/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else 
#define EXTERN_C extern
#endif /* __cplusplus **/

#ifndef HTMLAYOUT_STATIC_LIB
  #ifdef  HTMLAYOUT_EXPORTS
    #define HLAPI __declspec(dllexport) __stdcall
  #else
    #define HLAPI __declspec(dllimport) __stdcall
  #endif
#else
  #define HLAPI __stdcall
  void HLAPI HTMLayoutInit( HINSTANCE hModule, bool start);
#endif 

#ifndef _LPCBYTE_DEFINED
#define _LPCBYTE_DEFINED
typedef const BYTE *LPCBYTE;
#endif

#pragma pack(push,4)

#include "htmlayout_dom.h"
#include "htmlayout_behavior.h"

/**Get name of HTMLayout window class.
 *
 * \return \b LPCSTR, name of HTMLayout window class.
 *
 * Use this function if you wish to create ansi version of HTMLayout.
 * The returned name can be used in CreateWindow(Ex)A function.
 * You can use #HTMLayoutClassNameT macro.
 ***/
EXTERN_C LPCSTR  HLAPI HTMLayoutClassNameA();

/**Get name of HTMLayout window class.
 *
 * \return \b LPCWSTR, name of HTMLayout window class.
 *
 * Use this function if you wish to create unicode version of HTMLayout. 
 * The returned name can be used in CreateWindow(Ex)W function. 
 * You can use #HTMLayoutClassNameT macro.
 **/
EXTERN_C LPCWSTR HLAPI HTMLayoutClassNameW();

/**Returns name of HTMLayout window class.
 *
 * \return \b LPCTSTR, name of HTMLayout window class.
 *
 * This macro is used to select between #HTMLayoutClassNameW() and 
 * #HTMLayoutClassNameA() functions depending on whether UNICODE macro symbol 
 * is defined. 
 **/
#ifdef UNICODE
#define HTMLayoutClassNameT  HTMLayoutClassNameW
#else
#define HTMLayoutClassNameT  HTMLayoutClassNameA
#endif // !UNICODE

/**This notification is sent on parsing the document and while handling 
 * &lt;INPUT&gt;, &lt;TEXTAREA&gt;, &lt;SELECT&gt; and &lt;WIDGET&gt; tags.
 *
 * \param lParam #LPNMHL_CREATE_CONTROL.
 * \return HWND of the control or #HWND_TRY_DEFAULT or #HWND_DISCARD_CREATION.
 *
 * This notification is sent when HtmLayout is about to create a child control. 
 * The application can override the creation of "standard HTML controls" or 
 * implement the creation of other application specific controls. To do this assign 
 * HWND of newly created control to #NMHL_CREATE_CONTROL::outControlHwnd member of 
 * #NMHL_CREATE_CONTROL. 
 **/
#define HLN_CREATE_CONTROL  0xAFF + 0x01 

/**Notifies that HtmLayout is about to download a referred resource. 
 *
 * \param lParam #LPNMHL_LOAD_DATA.
 * \return #LOAD_OK or #LOAD_DISCARD 
 *
 * This notification gives application a chance to override built-in loader and 
 * implement loading of resources in its own way (for example images may be loaded from 
 * database or other resource). To do this set #NMHL_LOAD_DATA::outData and 
 * #NMHL_LOAD_DATA::outDataSize members of NMHL_LOAD_DATA. HTMLayout does not 
 * store pointer to this data. You can call #HTMLayoutDataReady() function instead 
 * of filling these fields. This allows you to free your outData buffer 
 * immediately.
**/
#define HLN_LOAD_DATA       0xAFF + 0x02

/**This notification is sent when control creation process has completed. 
 *
 * \param lParam #LPNMHL_CREATE_CONTROL. 
 *
 * While processing this notification application can modify control settings 
 * or store information about the created control.
 * For example it can use SetWindowLong() function, to set control's id:
 * \code
 * SetWindowLong(((LPNMHL_CREATECONTROL)lParam)->outControlHwnd, GWL_ID, controlId)
 * \endcode
 **/
#define HLN_CONTROL_CREATED 0xAFF + 0x03 

/**This notification indicates that external data (for example image) download process 
 * completed.
 *
 * \param lParam #LPNMHL_DATA_LOADED
 *
 * This notifiaction is sent for each external resource used by document when 
 * this resource has been completely downloaded. HTMLayout will send this 
 * notification asynchronously.
 **/
#define HLN_DATA_LOADED     0xAFF + 0x04 

/**This notification is sent when all external data (for example image) has been downloaded.
 *
 * This notification is sent when all external resources required by document 
 * have been completely downloaded. HTMLayout will send this notification 
 * asynchronously.
 **/
#define HLN_DOCUMENT_COMPLETE 0xAFF + 0x05 


/**This notification instructs host application to update its UI.
 *
 * This notification is sent on changes in HTMLayout formatting registers. 
 * If application indicates their state on toolbars or using other controls it 
 * should update them.
 **/
#define HLN_UPDATE_UI       0xAFF + 0x06


/**This notification is sent when HTMLayout destroys its controls.
 *
 * \param lParam #LPNMHL_DESTROY_CONTROL
 * Before loading new document HTMLayout destroys all controls belonging to 
 * previous document. Host application can reject deletion of the control by 
 * setting NMHL_DESTROY_CONTROL::inoutControlHwnd to zero.
 *
 * Handling of this notification makes sence when you need for example reload
 * HTML without destroying set of controls on HTML form. 
 * But you still need to handle NMHL_CREATE_CONTROL notification to bind
 * existing controls with their HTML "places"
 *
 * \attention HTMLayouts sends this notifiation only while loading new or 
 * empty document.
 **/
#define HLN_DESTROY_CONTROL 0xAFF + 0x07

/**This notification is sent on parsing the document and while processing 
 * elements having non empty style.behavior attribute value.
 *
 * \param lParam #LPNMHL_ATTACH_BEHAVIOR
 * 
 * Application has to provide implementation of #htmlayout::behavior interface. 
 * Set #NMHL_ATTACH_BEHAVIOR::impl to address of this implementation.
 **/
#define HLN_ATTACH_BEHAVIOR 0xAFF + 0x08

/**This notification is sent when dialog window created but document is not loaded
 *
 *
 * \param lParam is a pointer to statndard NMHDR
 * 
 * ATTN: Will be sent ONLY by HTMLayoutDialog API function.
 **/
#define HLN_DIALOG_CREATED 0xAFF + 0x10

/**This notification is sent when dialog window is about to be closed
 * - happens while handling WM_CLOSE in dialog
 *
 * \param lParam #LPNMHL_DIALOG_CLOSE_RQ
 * 
 * ATTN: Will be sent ONLY by HTMLayoutDialog API function.
 **/
#define HLN_DIALOG_CLOSE_RQ 0xAFF + 0x11


/**Notification callback function.
 *
 * \param uMsg \b UINT, WM_NOTIFY.
 * \param wParam \b WPARAM, control Id.
 * \param lParam \b LPARAM, depends on notification.
 * \param vParam \b LPVOID, value of cbParam parameter passed to 
 * #HTMLayoutSetCallback function.
 * \return \b LRESULT, depends on notification.
 * 
 * If set, HTMLayout will send all HLN_*** notifications to this function. 
 * To set notification callback use #HTMLayoutSetCallback() function.
 * By default (if callback is not set) HTMLayout will send HLN_*** notifications 
 * as WM_NOTIFY messages to its parent window.
 **/
typedef LRESULT CALLBACK HTMLAYOUT_NOTIFY(UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID vParam);
typedef HTMLAYOUT_NOTIFY* LPHTMLAYOUT_NOTIFY;

/**Create "default HTML control", used by #HLN_CREATE_CONTROL notification as 
 * value for #NMHL_CREATE_CONTROL::outControlHwnd.
 **/
#define HWND_TRY_DEFAULT      ((HWND)0)

/**Do not create any controls, used by #HLN_CREATE_CONTROL notification as 
 * value for #NMHL_CREATE_CONTROL::outControlHwnd.
 **/
#define HWND_DISCARD_CREATION ((HWND)1) 

/**This structure is used by #HLN_CREATE_CONTROL and #HLN_CONTROL_CREATED 
 * notifications.
 *
 * - #HLN_CREATE_CONTROL
 * \copydoc HLN_CREATE_CONTROL
 * 
 * - #HLN_CONTROL_CREATED
 *   \copydoc HLN_CONTROL_CREATED
 *
 **/
typedef struct tagNMHL_CREATE_CONTROL
{
    NMHDR     hdr;              /**< Default WM_NOTIFY header */

    HELEMENT  helement;         /**< [in] DOM element.*/
    HWND      inHwndParent;     /**< [in] HWND of the HTMLayout window.*/

    HWND      outControlHwnd;   /**< [out] HWND of control created or #HWND_TRY_DEFAULT or HWND_DISCARD_CREATION.*/
    DWORD     reserved1;
    DWORD     reserved2;

} NMHL_CREATE_CONTROL, FAR *LPNMHL_CREATE_CONTROL;

/**This structure is used by #HLN_DESTROY_CONTROL notification.
 * \copydoc HLN_DESTROY_CONTROL 
 **/
typedef struct tagNMHL_DESTROY_CONTROL
{
	/**Default WM_NOTIFY header.
	 **/
    NMHDR     hdr;              
	/**[in] DOM element.
	 **/
    HELEMENT  helement; 
    HWND      inoutControlHwnd; 
	/**<[in/out] HWND of child to be destroyed.*/
    DWORD     reserved1;

} NMHL_DESTROY_CONTROL, FAR *LPNMHL_DESTROY_CONTROL;

/**Use default loader or outData/outDataSize if they are set, 
 * used as return value for #HLN_CREATE_CONTROL notification.
 **/
#define LOAD_OK  0

/**Do not load resource at all, 
 * Used as return value for #HLN_CREATE_CONTROL notification.
 **/
#define LOAD_DISCARD      1

/**This structure is used by #HLN_LOAD_DATA notification.
 *\copydoc HLN_LOAD_DATA **/
typedef struct tagNMHL_LOAD_DATA
{
    NMHDR    hdr;              /**< Default WM_NOTIFY header.*/
    
    LPCWSTR  uri;              /**< [in] Zero terminated string, fully qualified uri, for example "http://server/folder/file.ext".*/
    
    LPVOID   outData;          /**< [out] pointer to loaded data .*/
    DWORD    outDataSize;      /**< [out] loaded data size.*/
    UINT     dataType;         /**< [in] HTMLayoutResourceType */

    HELEMENT principal;        /**< [in] element requested download, in case of context_menu:url( menu-url )
                                         it is an element for which context menu was requested */
    HELEMENT initiator;        /**< N/A */

} NMHL_LOAD_DATA, FAR *LPNMHL_LOAD_DATA;

/**This structure is used by #HLN_DATA_LOADED notification.
 *\copydoc HLN_DATA_LOADED 
 **/
typedef struct tagNMHL_DATA_LOADED
{
    NMHDR    hdr;              /**< Default WM_NOTIFY header.*/
    
    LPCWSTR  uri;              /**< [in] zero terminated string, fully qualified uri, for example "http://server/folder/file.ext".*/
    LPCBYTE  data;             /**< [in] pointer to loaded data.*/
    DWORD    dataSize;         /**< [in] loaded data size (in bytes).*/
    UINT     dataType;         /**< [in] HTMLayoutResourceType */

} NMHL_DATA_LOADED, FAR *LPNMHL_DATA_LOADED;

struct HTMLAYOUT_BEHAVIOR;

/**This structure is used by #HLN_ATTACH_BEHAVIOR notification.
 *\copydoc HLN_ATTACH_BEHAVIOR **/
typedef struct tagNMHL_ATTACH_BEHAVIOR
{
    NMHDR    hdr;              /**< Default WM_NOTIFY header.*/
    
    HELEMENT element;          /**< [in] target DOM element handle*/
    LPCSTR   behaviorName;     /**< [in] zero terminated string, string appears as value of CSS behavior:"???" attribute.*/
    
    ElementEventProc* elementProc;    /**< [out] pointer to ElementEventProc function.*/
    LPVOID            elementTag;     /**< [out] tag value, passed as is into pointer ElementEventProc function.*/
    UINT              elementEvents;  /**< [out] EVENT_GROUPS bit flags, event groups elementProc subscribed to. */

} NMHL_ATTACH_BEHAVIOR, FAR *LPNMHL_ATTACH_BEHAVIOR;


/**This structure is used by #HLN_DIALOG_CLOSE_RQ notification.
 *\copydoc HLN_LOAD_DATA **/
typedef struct tagNMHL_DIALOG_CLOSE_RQ
{
    NMHDR   hdr;              /**< Default WM_NOTIFY header.*/
    BOOL    outCancel;        /**< [out] set it to non-zero for canceling close request.*/

} NMHL_DIALOG_CLOSE_RQ, FAR *LPNMHL_DIALOG_CLOSE_RQ;



/**This function is used in response to HLN_LOAD_DATA request. 
 *
 * \param[in] hwnd \b HWND, HTMLayout window handle.
 * \param[in] uri \b LPCWSTR, URI of the data requested by HTMLayout.
 * \param[in] data \b LPBYTE, pointer to data buffer.
 * \param[in] dataLength \b DWORD, length of the data in bytes.
 * \return \b BOOL, TRUE if HTMLayout accepts the data or \c FALSE if error occured 
 * (for example this function was called outside of #HLN_LOAD_DATA request).
 *
 * \warning If used, call of this function MUST be done ONLY while handling 
 * HLN_LOAD_DATA request and in the same thread. For asynchronous resource loading
 * use HTMLayoutDataReadyAsync
 **/
EXTERN_C BOOL HLAPI HTMLayoutDataReady(HWND hwnd,LPCWSTR uri,LPBYTE data, DWORD dataLength);

/** Resource data type.
 *  Used by HTMLayoutDataReadyAsync() function.
 **/
enum HTMLayoutResourceType 
{ 
  HLRT_DATA_HTML = 0, 
  HLRT_DATA_IMAGE = 1, 
  HLRT_DATA_STYLE = 2, 
  HLRT_DATA_CURSOR = 3,
  HLRT_DATA_SCRIPT = 4,
};

/**Use this function outside of HLN_LOAD_DATA request. This function is needed when you
 * you have your own http client implemented in your application.
 *
 * \param[in] hwnd \b HWND, HTMLayout window handle.
 * \param[in] uri \b LPCWSTR, URI of the data requested by HTMLayout.
 * \param[in] data \b LPBYTE, pointer to data buffer.
 * \param[in] dataLength \b DWORD, length of the data in bytes.
 * \param[in] dataType \b UINT, type of resource to load. See HTMLayoutResourceType.
 * \return \b BOOL, TRUE if HTMLayout accepts the data or \c FALSE if error occured 
 **/

EXTERN_C BOOL HLAPI HTMLayoutDataReadyAsync(HWND hwnd,LPCWSTR uri,LPBYTE data, DWORD dataLength, UINT dataType /*HTMLayoutResourceType*/ );


/**HTMLayout Window Proc.*/
EXTERN_C LRESULT HLAPI HTMLayoutProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**HTMLayout Window Proc without call of DefWindowProc.*/
EXTERN_C LRESULT HLAPI HTMLayoutProcND(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* pbHandled);


/**HTMLayout operational modes. Used by HTMLayoutSetMode() function.*/
enum HTMLayoutModes 
{
  HLM_LAYOUT_ONLY = 0,         /**< layout manager and renderer.*/
  HLM_SHOW_SELECTION = 1,      /**< layout manager and renderer + text selection and WM_COPY.*/
};

/**Callback function used with HTMLayoutEnumResources().
 *
 * \param[in] resourceUri \b LPCWSTR, uri used to download resource.
 * \param[in] resourceType \b LPCSTR, type of the resources. 
 * \param[in] imageData \b LPCBYTE, address of resource data.
 * \param[in] imageDataSize \b DWORD, resource data size.
 * \return \b BOOL, \c TRUE - continue enumeration, \c FALSE - stop
 *
 * This function recieves information on all resources loaded with the currend document.
 **/
typedef BOOL CALLBACK HTMLAYOUT_CALLBACK_RES(LPCWSTR resourceUri, LPCSTR resourceType, LPCBYTE imageData, DWORD imageDataSize);

/**Returns minimal width of the document.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \return \b UINT, Minimal width at which HTMLayout can render the document.
 **/
EXTERN_C UINT HLAPI     HTMLayoutGetMinWidth(HWND hWndHTMLayout);

/**\brief Returns minimal height of the document.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \param[in] width \b UINT, desired width of the document.
 * \return \b UINT, Height of the document rendered with given width.
 **/
EXTERN_C UINT HLAPI     HTMLayoutGetMinHeight(HWND hWndHTMLayout, UINT width);

/**Load HTML file.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle. 
 * \param[in] filename \b LPCWSTR, File name of an HTML file.
 * \return \b BOOL, \c TRUE if the text was parsed and loaded successfully, \c FALSE otherwise.
 **/
EXTERN_C BOOL HLAPI     HTMLayoutLoadFile(HWND hWndHTMLayout, LPCWSTR filename);

/**Load HTML from in memory buffer.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \param[in] html \b LPCBYTE, Address of HTML to load.
 * \param[in] htmlSize \b UINT, Length of the array pointed by html parameter.
 * \return \b BOOL, \c TRUE if the text was parsed and loaded successfully, FALSE otherwise.
 **/
EXTERN_C BOOL HLAPI     HTMLayoutLoadHtml(HWND hWndHTMLayout, LPCBYTE html, UINT htmlSize);

/**Load HTML from in memory buffer with base.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \param[in] html \b LPCBYTE, Address of HTML to load.
 * \param[in] htmlSize \b UINT, Length of the array pointed by html parameter.
 * \param[in] baseUrl \b LPCWSTR, base URL. All relative links will be resolved against 
 *                                this URL.
 * \return \b BOOL, \c TRUE if the text was parsed and loaded successfully, FALSE otherwise.
 **/
EXTERN_C BOOL HLAPI     HTMLayoutLoadHtmlEx(HWND hWndHTMLayout, LPCBYTE html, UINT htmlSize, LPCWSTR baseUrl);


/**Sets the HTMLayout \link #HTMLayoutModes operational mode \endlink.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \param[in] HTMLayoutMode \b int, desired \link #HTMLayoutModes operational mode \endlink.
 **/
EXTERN_C VOID HLAPI     HTMLayoutSetMode(HWND hWndHTMLayout, int HTMLayoutMode);

/**Set \link #HTMLAYOUT_NOTIFY() notification callback function \endlink.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \param[in] cb \b HTMLAYOUT_NOTIFY*, \link #HTMLAYOUT_NOTIFY() callback function \endlink.
 * \param[in] cbParam \b LPVOID, parameter that will be passed to \link #HTMLAYOUT_NOTIFY() callback function \endlink as vParam paramter.
 **/
EXTERN_C VOID HLAPI     HTMLayoutSetCallback(HWND hWndHTMLayout, LPHTMLAYOUT_NOTIFY cb, LPVOID cbParam);

/**Query whether user have selected some HTML text.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 **/
EXTERN_C BOOL HLAPI     HTMLayoutSelectionExist(HWND hWndHTMLayout);

/**Get selected HTML
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \param[out] pSize \b LPUINT*, variable to recieve size of the returned buffer.
 * \return LPCBYTE, pointer to buffer containing selected HTML. It is HTMLayout's responsibility to free this buffer.
 **/
EXTERN_C LPCBYTE HLAPI  HTMLayoutGetSelectedHTML(HWND hWndHTMLayout, LPUINT pSize);

/**Copies selection to clipboard.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \return \b BOOL, TRUE if selected HTML text has been copied to clipboard, FALSE otherwise.
 **/
EXTERN_C BOOL HLAPI     HTMLayoutClipboardCopy(HWND hWndHTMLayout);

/**Enumerates resources loaded with current document.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \param[in] cb \b #HTMLAYOUT_CALLBACK_RES*, callback function.
 **/
EXTERN_C UINT HLAPI     HTMLayoutEnumResources(HWND hWndHTMLayout,HTMLAYOUT_CALLBACK_RES* cb);

/**Set Master style sheet.
  See: http://www.terrainformatica.com/htmlayout/master_ss.css.txt
  Or resource section of the library for "master-css" HTML resource.
 *
 * \param[in] utf8 \b LPCBYTE, start of CSS buffer.
 * \param[in] numBytes \b UINT, number of bytes in utf8.
 **/

EXTERN_C BOOL HLAPI     HTMLayoutSetMasterCSS(LPCBYTE utf8, UINT numBytes);

/**Set (reset) style sheet of current document.
 Will reset styles for all elements according to given CSS (utf8)
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \param[in] utf8 \b LPCBYTE, start of CSS buffer.
 * \param[in] numBytes \b UINT, number of bytes in utf8.
 **/

EXTERN_C BOOL HLAPI     HTMLayoutSetCSS(HWND hWndHTMLayout, LPCBYTE utf8, UINT numBytes, LPCWSTR baseUrl, LPCWSTR mediaType);

/**Set media type of this htmlayout instance.
 *
 * \param[in] hWndHTMLayout \b HWND, HTMLayout window handle.
 * \param[in] mediaType \b LPCWSTR, media type name.
 *
 * For example media type can be "handheld", "projection", "screen", "screen-hires", etc.
 * By default htmlayout window has "screen" media type.
 * 
 * Media type name is used while loading and parsing style sheets in the engine so
 * you should call this function *before* loading document in it.
 *
 **/

EXTERN_C BOOL HLAPI     HTMLayoutSetMediaType(HWND hWndHTMLayout, LPCWSTR mediaType);


/**Show HTML based dialog
 *
 * \param[in] hWndParent \b HWND, parent window, can be NULL.
 * \param[in] position \b POINT, anchor point.
 * \param[in] alignment \b UINT, alignment of the window, defines corner of 
 *            the dialog window 'position' points to:
 *            Values: 0 - center of the dialog in the center of the screen
 *                    1-9 - see NUMPAD digits for what 'position' defines.  
 *                   -1..-9 correspondent corner of the dilaog will be placed to the correspondent corner of window rectangle
 *                          of hWndParent.
 *                          Example: -9 dialog will be aligned to top/right corner of the parent window. 
 * \param[in] style \b UINT, style of the dialog window. WS_***. 
 * \param[in] styleEx \b UINT, extended style of the dialog window, WS_EX_***. 
 * \param[in] notificationCallback \b LPHTMLAYOUT_NOTIFY, notifaction callback function, see HTMLayoutSetCallback.
 * \param[in] eventsCallback \b LPELEMENT_EVENT_PROC, event callback function, see HTMLayoutAttachEventHandler.
 * \param[in] callbackParam \b LPVOID, notifaction callback function, see HTMLayoutSetCallback.
 * \param[in] html \b either file name or pointer to buffer containing HTML
 * \param[in] htmlLength \b UINT, number of bytes in 'html'. 
 *                       If it is zero then 'html' points to LPCWSTR string - URL of the document to load.
 *                       If it is not zero than it shall contain number of bytes in 'html' buffer. 
 *                    
 **/
EXTERN_C INT_PTR HLAPI HTMLayoutDialog(
                HWND                  hWndParent, 
                POINT                 position, 
                INT                   alignment, 
                UINT                  style, 
                UINT                  styleEx,
                LPHTMLAYOUT_NOTIFY    notificationCallback,
                LPELEMENT_EVENT_PROC  eventsCallback,
                LPVOID                callbackParam,
                LPCBYTE               html,
                UINT                  htmlLength);


/** HTMLayoutSetupDebugOutput - setup debug output function.
 *
 *  This output function will be used for reprting problems 
 *  found while loading html and css documents.
 *
 **/

typedef VOID (CALLBACK* DEBUG_OUTPUT_PROC)(LPVOID param, INT character);

EXTERN_C VOID HLAPI HTMLayoutSetupDebugOutput(
                LPVOID                param,    // param to be passed "as is" to the pfOutput
                DEBUG_OUTPUT_PROC     pfOutput  // output function, output stream alike thing.
                );

#pragma pack(pop)

#ifndef LIBRARY_BUILD

#if defined(__cplusplus) && !defined( PLAIN_API_ONLY )

  #include "htmlayout_behavior.hpp"
  #include "htmlayout_dom.hpp"
  #include "htmlayout_dialog.hpp"
  #include "behaviors/notifications.h"
#if !defined(_WIN32_WCE) && !defined(SCRIPTING)
  // these two guys are using STL - not enough portable, e.g. not for eVC.
  #include "htmlayout_value.hpp"
  #include "htmlayout_controls.hpp"
#endif

  namespace htmlayout 
  {
    struct debug_output
    {
      debug_output()
      {
        ::HTMLayoutSetupDebugOutput(0, _output_debug);
      }
      static VOID CALLBACK _output_debug(LPVOID, INT ch)
      {
        WCHAR m[2] = {0,0};
        m[0] = ch;
        OutputDebugStringW(m);
      }
    };

#if !defined(_WIN32_WCE)
    struct debug_output_console
    {
      
      debug_output_console()
      {
        ::HTMLayoutSetupDebugOutput(0, _output_debug);
      }
      static VOID CALLBACK _output_debug(LPVOID, INT ch)
      {
        static bool initialized = false;
        if(!initialized)
        {
          AllocConsole();
          freopen("conin$", "r", stdin);
          freopen("conout$", "w", stdout);
          freopen("conout$", "w", stderr);
          initialized = true;
        }
        putwchar(ch);
      }

      void printf( const char* fmt, ... )
      {
        char buffer [ 2049 ];
        va_list args;
        va_start ( args, fmt );
        int len = _vsnprintf( buffer, 2048, fmt, args );
        va_end ( args );
        buffer [ 2048 ] = 0;
        for( const char *p = buffer; *p; ++p)
          _output_debug(0, *p);
      }


    };
#endif




  }

#endif

#endif // LIBRARY_BUILD

#endif


