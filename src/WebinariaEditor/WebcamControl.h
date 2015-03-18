#pragma once

extern const int WM_WEBCAMCONTROLNOTIFY;

enum WebcamControlNotificationCode
{
	WebcamPositionUpdated = 1
};

class WebcamControl
{

private:
	int m_HorzShift, m_VertShift;
	float m_HorzShiftMin, m_HorzShiftMax, m_VertShiftMin, m_VertShiftMax;
	RECT m_MainBounds, m_CameraBounds, m_Camera;
	SIZE m_MainStreamGeometry, m_CameraStreamGeometry;

	bool m_Dragging;

	void RecalculateBounds(HWND wnd, bool redraw = false);
	float between(float x, float mn, float mx);

private:
	static BOOL CALLBACK s_DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

	HWND m_Dialog;
	HIMAGELIST m_Images;

	void Repaint(HWND wnd, HDC dc);

	void MouseDown();
	void MouseUp();
	void MouseMove();
	void GetClientMousePos(POINT *ppt);
	void MoveCamera();

	void InternalSetCamera(int horzShift, int vertShift, bool repaint);

public:
	WebcamControl(HINSTANCE instance, HWND parent, UINT id);
	~WebcamControl();

	void SetGeometry(SIZE main, SIZE camera);

	operator HWND() { return m_Dialog; }

	int HorzShift() const { return m_HorzShift; }
	int VertShift() const { return m_VertShift; }

	void SetCamera(int horzShift, int vertShift);

};
