#include "StdAfx.h"

#include <math.h>
#include "WebcamControl.h"
#include "utils.h"

const int WM_WEBCAMCONTROLNOTIFY = RegisterWindowMessage(_T("WebcamControl.{D8C71EA6-7BB1-4d60-87D7-2D92F12EB5E5}"));

const int g_CameraIconWidth = 32;
const int g_CameraIconHeight = 32;

WebcamControl::WebcamControl(HINSTANCE instance, HWND parent, UINT id)
	: m_HorzShift(0)
	, m_VertShift(100)
	, m_HorzShiftMin(0)
	, m_HorzShiftMax(0)
	, m_VertShiftMin(0)
	, m_VertShiftMax(100.0)
	, m_Dragging(false)
{
	ZeroMemory(&m_CameraBounds, sizeof(m_CameraBounds));
	ZeroMemory(&m_Camera, sizeof(m_Camera));
	ZeroMemory(&m_MainBounds, sizeof(m_MainBounds));
	ZeroMemory(&m_MainStreamGeometry, sizeof(m_MainStreamGeometry));
	ZeroMemory(&m_CameraStreamGeometry, sizeof(m_CameraStreamGeometry));

	m_Dialog = 
		CreateDialogParam(instance, MAKEINTRESOURCE(IDD_WEBCAM_CONTROL), parent, s_DialogProc, (LPARAM)this);

	if (m_Dialog)
	{
		SetWindowLong(m_Dialog, GWL_ID, id);
		//	Use imagelist to hold webcam image for simplified masking support.
		m_Images = ImageList_LoadBitmap(instance, MAKEINTRESOURCE(IDB_CAMERA), g_CameraIconWidth, 1, CLR_DEFAULT);
	}
}

WebcamControl::~WebcamControl()
{
	if (m_Dialog)
		DestroyWindow(m_Dialog);

	if (m_Images)
		ImageList_Destroy(m_Images);
}

BOOL CALLBACK WebcamControl::s_DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_INITDIALOG == message)
		SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);

	WebcamControl *dlg = (WebcamControl *)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	if (dlg)
		return dlg->DialogProc(hwndDlg, message, wParam, lParam);

	return FALSE;
}

void WebcamControl::SetGeometry(SIZE main, SIZE camera)
{
	memmove(&m_MainStreamGeometry, &main, sizeof(SIZE));
	memmove(&m_CameraStreamGeometry, &camera, sizeof(SIZE));

	if (m_Dialog)
		RecalculateBounds(m_Dialog, true);
}

void WebcamControl::RecalculateBounds(HWND wnd, bool redraw/* = false*/)
{
	RECT client;
	GetClientRect(wnd, &client);

	if (m_MainStreamGeometry.cx < 1 || m_MainStreamGeometry.cy < 1 || m_CameraStreamGeometry.cx < 1 || m_CameraStreamGeometry.cy < 1 ||
		m_CameraStreamGeometry.cx > m_MainStreamGeometry.cx || m_CameraStreamGeometry.cy > m_MainStreamGeometry.cy)
	{
		//	Geometry isn't set or insane (camera stream larger than main stream), 
		//	so set default rects.
		m_MainStreamGeometry.cx = RectWidth(client);
		m_MainStreamGeometry.cy = RectHeight(client);
		m_CameraStreamGeometry.cx = g_CameraIconWidth;
		m_CameraStreamGeometry.cy = g_CameraIconHeight;
	}

	//	Calculate shift constraints.
	m_VertShiftMin = 100.0 - ((float)(m_MainStreamGeometry.cy - m_CameraStreamGeometry.cy) * 100.0) / (float)m_MainStreamGeometry.cy;
	m_HorzShiftMax = ((float)(m_MainStreamGeometry.cx - m_CameraStreamGeometry.cx) * 100.0) / (float)m_MainStreamGeometry.cx;

	//	Zoom factor for main bounds (will be calculated below using longest 
	//	side of the main rect).
	float zm = 1;

	//	Calculate visible bounds of main stream.
	memmove(&m_MainBounds, &client, sizeof(m_MainBounds));
	InflateRect(&m_MainBounds, -1, -1);
	//	Keep aspect ratio.
	float km = (float)m_MainStreamGeometry.cx / (float)m_MainStreamGeometry.cy;
	if (km > 1.0)
	{
		//	Width is longer.
		m_MainBounds.bottom = m_MainBounds.top + (float)RectWidth(m_MainBounds) / km;
		zm = (float)RectWidth(m_MainBounds) / (float)m_MainStreamGeometry.cx;
	}
	else
	{
		//	Height is longer.
		m_MainBounds.right = m_MainBounds.left + (float)RectHeight(m_MainBounds) * km;
		zm = (float)RectHeight(m_MainBounds) / (float)m_MainStreamGeometry.cy;
	}
	//	Center main stream within our client bounds.
	int cx = (RectWidth(client) - RectWidth(m_MainBounds)) / 2;
	int cy = (RectHeight(client) - RectHeight(m_MainBounds)) / 2;
	OffsetRect(&m_MainBounds, cx, cy);

	//	Calculate visible bounds of camera stream.
	ZeroMemory(&m_Camera, sizeof(m_Camera));
	float kc = (float)m_CameraStreamGeometry.cx / (float)m_CameraStreamGeometry.cy;
	if (kc > 1.0)
	{
		//	Width is longer.
		m_Camera.right = m_Camera.left + (float)m_CameraStreamGeometry.cx * zm;
		m_Camera.bottom = m_Camera.top + (float)RectWidth(m_Camera) / kc;
	}
	else
	{
		//	Height is longer.
		m_Camera.bottom = m_Camera.top + (float)m_CameraStreamGeometry.cy * zm;
		m_Camera.right = m_Camera.left + (float)RectHeight(m_Camera) * kc;
	}

	//	Calculate camera movement bounds.
	memmove(&m_CameraBounds, &m_MainBounds, sizeof(m_CameraBounds));
	InflateRect(&m_CameraBounds, -(RectWidth(m_Camera) / 2), -(RectHeight(m_Camera) / 2));

	InternalSetCamera(m_HorzShift, m_VertShift, false);

	if (redraw)
		InvalidateRect(wnd, 0, FALSE);
}

void WebcamControl::Repaint(HWND wnd, HDC dc)
{
	RECT rc;
	GetClientRect(wnd, &rc);

	//	Control background.
	FillRect(dc, &rc, GetSysColorBrush(COLOR_BTNFACE));

	//	Main stream bounds.
	DrawEdge(dc, &m_MainBounds, EDGE_ETCHED, BF_RECT);

	//	Camera stream bounds.
	DrawEdge(dc, &m_Camera, EDGE_ETCHED, BF_RECT);

#ifdef DEBUG
	DrawEdge(dc, &m_CameraBounds, EDGE_ETCHED, BF_RECT);
#endif

	//	Draw camera.
	if (m_Images)
	{
		ImageList_Draw(m_Images, 0, dc, m_Camera.left + (RectWidth(m_Camera) - g_CameraIconWidth) / 2, 
			m_Camera.top + (RectHeight(m_Camera) - g_CameraIconHeight) / 2, ILD_TRANSPARENT);
	}
}

void WebcamControl::GetClientMousePos(POINT *ppt)
{
	GetCursorPos(ppt);
	ScreenToClient(m_Dialog, ppt);
}

void WebcamControl::MouseDown()
{
	//	Determine if the mouse is in camera bounds.
	POINT pt;
	GetClientMousePos(&pt);
	if (!PtInRect(&m_MainBounds, pt))
		return;

	//	Capture camera in camera bounds.
	m_Dragging = true;
	SetCapture(m_Dialog);

	POINT leftTop, rightBottom;
	leftTop.x = m_CameraBounds.left;
	leftTop.y = m_CameraBounds.top;
	rightBottom.x = m_CameraBounds.right;
	rightBottom.y = m_CameraBounds.bottom;

	ClientToScreen(m_Dialog, &leftTop);
	ClientToScreen(m_Dialog, &rightBottom);

	RECT rc;
	SetRect(&rc, leftTop.x, leftTop.y, rightBottom.x, rightBottom.y);

	ClipCursor(&rc);

	MoveCamera();
}

void WebcamControl::MouseUp()
{
	m_Dragging = false;
	ClipCursor(0);
	ReleaseCapture();

	//	Notify parent about position change.
	NMHDR nmhdr = { 0 };
	nmhdr.hwndFrom = m_Dialog;
	nmhdr.idFrom = GetDlgCtrlID(m_Dialog);
	nmhdr.code = WebcamPositionUpdated;
	SendMessage(GetParent(m_Dialog), WM_NOTIFY, nmhdr.idFrom, (LPARAM)&nmhdr);
}

void WebcamControl::MouseMove()
{
	if (m_Dragging)
		MoveCamera();
}

void WebcamControl::MoveCamera()
{
	//	Only dragging withing the camera bounds count.
	POINT pt;
	GetClientMousePos(&pt);
	if (!PtInRect(&m_CameraBounds, pt))
		return;

#ifdef DEBUG
	TCHAR dbg[256];
	_stprintf_s(dbg, _countof(dbg), _T("%d; %d; %d; %d\n"), pt.y, RectHeight(m_Camera), m_MainBounds.top, RectHeight(m_MainBounds));
	OutputDebugString(dbg);
#endif

	//	Update camera position.
	int horzShift = 
		(((float)pt.x - (float)RectWidth(m_Camera) / 2.0 - m_MainBounds.left) * 100.0) / (float)RectWidth(m_MainBounds);
	int vertShift = 
		100.0 * (1.0 - ((float)pt.y - (float)RectHeight(m_Camera) / 2.0 - m_MainBounds.top) / (float)RectHeight(m_MainBounds));
	SetCamera(horzShift, vertShift);

#ifdef DEBUG
	_stprintf_s(dbg, _countof(dbg), _T("%d; %f; %d\n"), vertShift, m_VertShiftMin, m_VertShift);
	OutputDebugString(dbg);
#endif
}

float WebcamControl::between(float x, float mn, float mx)
{
	if (x > mx)
		return mx;

	if (x < mn)
		return ceil(mn);

	return x;
}

void WebcamControl::InternalSetCamera(int horzShift, int vertShift, bool repaint)
{
	m_HorzShift = between(horzShift, m_HorzShiftMin, m_HorzShiftMax);
	m_VertShift = between(vertShift, m_VertShiftMin, m_VertShiftMax);

	int x = RectWidth(m_MainBounds) * ((float)m_HorzShift / 100.0);
	int y = RectHeight(m_MainBounds) * ((100.0 - (float)m_VertShift) / 100.0);
	int cx = RectWidth(m_Camera);
	int cy = RectHeight(m_Camera);

	//	Update camera rectangle.
	m_Camera.left = m_MainBounds.left + x;
	m_Camera.right = m_Camera.left + cx;
	m_Camera.top = m_MainBounds.top + y;
	m_Camera.bottom = m_Camera.top + cy;

	if (repaint)
		InvalidateRect(m_Dialog, 0, FALSE);
}

void WebcamControl::SetCamera(int horzShift, int vertShift)
{
	InternalSetCamera(horzShift, vertShift, true);
}

BOOL CALLBACK WebcamControl::DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		RecalculateBounds(hwndDlg);
		break;

	case WM_LBUTTONDOWN:
		MouseDown();
		break;

	case WM_LBUTTONUP:
		MouseUp();
		break;

	case WM_MOUSEMOVE:
		MouseMove();
		break;

	case WM_SIZE:
	case WM_SIZING:
		RecalculateBounds(hwndDlg, true);
		break;

	case WM_PAINT:
		if (GetUpdateRect(hwndDlg, 0, FALSE))
		{
			PAINTSTRUCT ps = { 0 };
			HDC dc = BeginPaint(hwndDlg, &ps);
			if (dc)
			{
				Repaint(hwndDlg, dc);
				EndPaint(hwndDlg, &ps);
			}
		}
		break;

	case WM_ERASEBKGND:
		return FALSE;
	}

	return FALSE;
}
