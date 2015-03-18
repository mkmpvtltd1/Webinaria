#include "StdAfx.h"
#include "Authorization.h"
#include "Folders.h"

using namespace std;

namespace WebinariaApplication
{
	namespace WebinariaUserInterface
	{
		HINSTANCE CAuthorization::HInstance = 0;
		bool CAuthorization::Show = false;
		HANDLE CAuthorization::EnterEvent = CreateEvent(NULL, TRUE, FALSE, L"EnterEvent");
		const wchar_t MCLASSNAME[] = L"WUI::Authorization";

		// Here we import the function from USER32.DLL 
		HMODULE CAuthorization::hUser32 = GetModuleHandle(_T("USER32.DLL")); 
		lpfnSetLayeredWindowAttributes CAuthorization::SetLayeredWindowAttributes = 
		(lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32, "SetLayeredWindowAttributes"); 

		//////////////////////////////////////////////////////////////////////////
		//							Public methods								//
		//////////////////////////////////////////////////////////////////////////
		// Default constructor
		CAuthorization::CAuthorization(HWND hWnd, HINSTANCE hInstance, const wchar_t * Caption):event_handler(HANDLE_BEHAVIOR_EVENT),
																								Caption(NULL),
																								hMainWnd(hWnd)
		{
			this->Caption = new wchar_t[wcslen(Caption) + 1];
			wcscpy(this->Caption, Caption);
			
			// Initialize global window class
			RegisterWindowClass(hInstance);

			// Set window style
			UINT style = WS_CHILDWINDOW | WS_POPUPWINDOW | WS_EX_LAYERED;

			// Create main window
			HWnd = CreateWindowW(MCLASSNAME, Caption, style ,400, 400, 200, 114, hMainWnd, NULL, HInstance, NULL);

			// Save window associated data
			self(HWnd,this);

			// Callback for HTMLayout document elements
			HTMLayoutSetCallback(HWnd,&callback,this);

            TCHAR ui[MAX_PATH];
            Folders::FromRecorderUI(ui, _T("auth.htm"));

			// Loading document elements
			if(HTMLayoutLoadFile(HWnd, ui))
			{
				htmlayout::dom::element r = Root();

				Body			= r.find_first("body");
				WindowCaption	= r.get_element_by_id("caption");
				CloseButton		= r.get_element_by_id("close");

				OKButton		= r.get_element_by_id("OK");
				PasswordEdit	= r.get_element_by_id("Password");
				LoginEdit		= r.get_element_by_id("Login");

				htmlayout::attach_event_handler(r, this);

				SetWindowCaption();
			}
		}

		// Virtual destructor
		CAuthorization::~CAuthorization(void)
		{
			if (Caption != NULL)
				delete[] Caption;
			// Post close message
			::PostMessage(HWnd, WM_CLOSE, 0,0); 
			UnregisterClass(MCLASSNAME,HInstance);
		}
		
		// Register customer window class
		ATOM CAuthorization::RegisterWindowClass(HINSTANCE hInstance)
		{
			// Window instance handle initialize
			HInstance = hInstance;

			WNDCLASSEXW wcex;

			// Set extension window options 
			wcex.cbSize			= sizeof(WNDCLASSEX); 
			wcex.style			= CS_VREDRAW | CS_HREDRAW;
			wcex.lpfnWndProc	= (WNDPROC)AuthDlgWindowProc;
			wcex.cbClsExtra		= 0;
			wcex.cbWndExtra		= 0;
			wcex.hInstance		= hInstance;
			wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_WEBINARIA_ICON);
			wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground	= (HBRUSH)(COLOR_DESKTOP);
			wcex.lpszMenuName	= 0;
			wcex.lpszClassName	= MCLASSNAME;
			wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WEBINARIA_ICON);

			// Register window class
			return RegisterClassExW(&wcex);
		}

		// Get login and password
		bool CAuthorization::GetLoginPassword(wchar_t * login, wchar_t * password)
		{
			if (wcscmp(LoginEdit.text(), L"") != 0 &&
				wcscmp(PasswordEdit.text(), L"") != 0 &&
				LoginEdit.text() != login &&
				PasswordEdit.text() != password)
			{
				wcscpy(login, LoginEdit.text());
				wcscpy(password, PasswordEdit.text());
				
				return true;
			}
			
			return false;
		}
		
		// Show dialog window in popup state
		void CAuthorization::ShowPopupDialog()
		{
			if (!Show)
			{
				ResetEvent(EnterEvent);
				ShowWindow(HWnd, SW_NORMAL);
				Show = true;
			}
		}
		
		// Set window caption
		void CAuthorization::SetWindowCaption( )
		{
			// If text is not null
			if(Caption)
			{
				  // Set window caption
				  ::SetWindowTextW(HWnd, Caption);
				  // If HTMLayout document contains caption
				  if( WindowCaption.is_valid() )
				  {
					// Set HTMLayout document caption text
					WindowCaption.set_text(Caption);
					// Update HTMLayout document caption
					WindowCaption.update(true);
				  }
			}
		}

		// Test HTMLayount controls under mouse pouinter
		int CAuthorization::HitTest(const int & x,
									const int & y ) const
		{
			POINT pt; 
			pt.x = x; 
			pt.y = y;
			// Convert desktop mouse point to window mouse point
			::MapWindowPoints(HWND_DESKTOP,HWnd,&pt,1);

			// If mouse pointer is over window caption
			if( WindowCaption.is_valid() && WindowCaption.is_inside(pt) )
				return HTCAPTION;

			RECT body_rc = Body.get_location(ROOT_RELATIVE | CONTENT_BOX);

			// If mouse pointer is over Body
			if( PtInRect(&body_rc, pt) )
				return HTCLIENT;

			if( pt.y < body_rc.top + 10 ) 
			{
				// If mouse pointer is over LeftTop coner
				if( pt.x < body_rc.left + 10 ) 
					return HTTOPLEFT;
				// If mouse pointer is over RightTop coner
				if( pt.x > body_rc.right - 10 ) 
					return HTTOPRIGHT;
			}
			else 
			if( pt.y > body_rc.bottom - 10 ) 
			{
				// If mouse pointer is over LeftBottom coner
				if( pt.x < body_rc.left + 10 ) 
					return HTBOTTOMLEFT;
				// If mouse pointer is over RightBottom coner
				if( pt.x > body_rc.right - 10 ) 
					return HTBOTTOMRIGHT;
			}

			// If mouse pointer is above top border
			if( pt.y < body_rc.top ) 
				return HTTOP;
			// If mouse pointer is below bottom border
			if( pt.y > body_rc.bottom ) 
				return HTBOTTOM;
			// If mouse pointer is outer of the left border
			if( pt.x < body_rc.left ) 
				return HTLEFT;
			// If mouse pointer is outer of the right border
			if( pt.x > body_rc.right ) 
				return HTRIGHT;

			// If mouse pointer is in the client area
			return HTCLIENT;	
		}

		// Get root DOM element of the HTMLayout document
		HELEMENT CAuthorization::Root() const
		{
			return htmlayout::dom::element::root_element(HWnd);
		}

		// Обрабатывает события, происходящие с элементами документа HTMLayout
		BOOL CAuthorization::on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason )
		{
			// If close button click
			if ((target == CloseButton || target == OKButton) && type == 32769)
			{
				if (target == CloseButton)
				{
					LoginEdit.set_text(L"");
					PasswordEdit.set_text(L"");
				}
				
				ShowWindow(HWnd,SW_HIDE);
				Show = false;
				SetEvent(EnterEvent);
			}

			return TRUE;
		}

		// Handle dialog box message
		LRESULT CALLBACK CAuthorization::AuthDlgWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			LRESULT lResult;
			BOOL    bHandled;
						  
			// HTMLayout +
			// HTMLayout could be created as separate window using CreateWindow API.
			// But in this case we are attaching HTMLayout functionality
			// to the existing window delegating windows message handling to 
			// HTMLayoutProcND function.
			lResult = HTMLayoutProcND(hWnd, message, wParam, lParam, &bHandled);
			if(bHandled)
			  return lResult;
			// HTMLayout -
		
			CAuthorization * me = self(hWnd);

			switch (message) 
			{
			case WM_COMMAND:
				{
					UINT uID; 
					UINT uMsg; 
	
					uID =	LOWORD(wParam);		// Extract element ID
					uMsg = (UINT) lParam;		// Extract message

					switch (uID)
					{
					case 27:
					case IDOK:
						{
							::DestroyWindow(hWnd);
						}
						break;
					}
				}
				break;
			
			case WM_NCHITTEST:
				{
					// Update HTMLayout window
					htmlayout::dom::element r = htmlayout::dom::element::root_element(hWnd);
					r.update(true);
					// Test hit
					if(me)
						return me->HitTest( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
				}
				break;
			
			case WM_ACTIVATE:
					/*Show = (wParam == WA_INACTIVE);*/
				return 0;

			case WM_NCCALCSIZE:   
				// We have no non-client areas
				return 0; 

			case WM_GETMINMAXINFO:
			{
				LRESULT lr = DefWindowProc(hWnd, message, wParam, lParam);
				MINMAXINFO* pmmi = (MINMAXINFO*)lParam;
				pmmi->ptMinTrackSize.x = ::HTMLayoutGetMinWidth(hWnd);
				RECT rc; 
				GetWindowRect(hWnd,&rc);
				pmmi->ptMinTrackSize.y = ::HTMLayoutGetMinHeight(hWnd, rc.right - rc.left);
				return lr;
			}

			case WM_NCPAINT:     
				{
					// Check the current state of the dialog, and then add the WS_EX_LAYERED attribute 
					SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
					// Sets the window to 0 visibility for green colour for window coners
					SetLayeredWindowAttributes(hWnd,RGB(0,255,0), 0, LWA_COLORKEY); 
				}
				return 0; 

			case WM_CLOSE:
				// Destroy window
				::DestroyWindow(hWnd);
				return 0;

			case WM_DESTROY:
				// Delete window class
				me = NULL;
				self(hWnd,0);
				PostQuitMessage(0);
				return 0;
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
}