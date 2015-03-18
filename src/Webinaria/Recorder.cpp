#include "StdAfx.h"
#include "Recorder.h"
namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		RecorderStates CRecorder::State = RS_STOP;			// Current recorder state
		RecorderCommand CRecorder::Command = RC_NOCOM;		// Current recorder command
		bool	CRecorder::fAudio = true;					// Flags
		bool	CRecorder::fCamera = false;
		bool	CRecorder::fBack = true;
		bool	CRecorder::fMouse = false;
		bool	CRecorder::fText = false;
		bool	CRecorder::fArrow = false;	
		int		CRecorder::Left = 0;
		int		CRecorder::Right = 640;
		int		CRecorder::Top = 0;
		int		CRecorder::Bottom = 480;
		double	CRecorder::FrameRate = 15.0;
		bool	CRecorder::fActiveWindow = true;			// Capturing static part of screen
		bool	CRecorder::fChangeSize = false;				// Change screen capturing area sizes

		//////////////////////////////////////////////////////////////////////////
		//								Public methods							//
		//////////////////////////////////////////////////////////////////////////

		CRecorder::CRecorder(void)
		{
		}

		CRecorder::~CRecorder(void)
		{
		}

		// Record movie
		void CRecorder::Record()
		{
			int i = 0;
			Record(i);
		}

		// Pause record
		void CRecorder::PauseResume()
		{
			int i = 0;
			if (State == RS_PAUSE)
				Resume(i);
			if (State == RS_RECORDING)
				Pause(i);
		}

		// Start|stop capturing
		void CRecorder::RecordStop()
		{
			int i = 0;
			if (State == RS_RECORDING)
				Stop(i);
			if (State == RS_STOP)
				Record(i);
		}

		// Stop record
		void CRecorder::Stop()
		{
			int i = 0;
			Stop(i);
		}

		// Change device
		void CRecorder::ChangeDevice()
		{
			Command = RC_CHANGE_DEVICE;
		}

		// Get current recorder state
		RecorderStates CRecorder::GetCurrentRecorderState()
		{
			return State;
		}

		// Get current recorder command
		RecorderCommand CRecorder::GetCurrentRecorderCommand()
		{
			return Command;
		}

		// Callback 
		void CRecorder::ProvideCommand()
		{
			switch (Command)
			{
			case RC_RECORD:
			case RC_RESUME:
				State = RS_RECORDING;
				break;
			case RC_STOP:
				State = RS_STOP;
				break;
			case RC_PAUSE:
				State = RS_PAUSE;
				break;
			case RC_NOCOM:
			case RC_CHANGE_DEVICE:
			default:
				break;
			}
			Command = RC_NOCOM;
		}

		// Get additional flag
		bool CRecorder::GetAdditionalFlag(AdditionalFlag flag)
		{
			switch (flag)
			{
			case AF_CAP_AUDIO:
				return fAudio;
				break;
			case AF_CAP_CAMERA:
				return fCamera;
				break;
			case AF_REMOVE_BACK:
				return fBack;
				break;
			case AF_REMOVE_MOUSE:
				return fMouse;
				break;
			case AF_SET_TEXT:
				return fText;
				break;
			case AF_SET_ARROW:
				return fArrow;
				break;
			case AF_ACTIVE_WINDOW:
				return fActiveWindow;
				break;
			case AF_CHANGE_SIZE:
				return fChangeSize;
				break;
			default:
				return false;
			}
		}

		// Set additional flags
		void CRecorder::SetAdditionalFlag(AdditionalFlag flag, bool state)
		{
			switch (flag)
			{
			case AF_CAP_AUDIO:
				fAudio = state;
				break;
			case AF_CAP_CAMERA:
				fCamera = state;
				break;
			case AF_REMOVE_BACK:
				fBack = state;
				break;
			case AF_REMOVE_MOUSE:
				fMouse = state;
				break;
			case AF_SET_TEXT:
				fText = state;
				break;
			case AF_SET_ARROW:
				fArrow = state;
				break;
			case AF_ACTIVE_WINDOW:
				fActiveWindow = state;
				break;
			case AF_CHANGE_SIZE:
				fChangeSize = state;
				break;
			}
		}

		// Set screen area borders
		void CRecorder::SetBorders(const int & l, const int & t, const int & r, const int & b)
		{
			RECT rc;
			GetWindowRect(GetDesktopWindow(), &rc);
			if (l < rc.left)
				Left = rc.left;
			else if (l > rc.right)
				Left = rc.right;
			else
				Left = l;
			if (r < rc.left)
				Right = rc.left;
			else if (r > rc.right)
				Right = rc.right;
			else
				Right = r;
			if (t < rc.top)
				Top = rc.top;
			else if (t > rc.bottom)
				Top = rc.bottom;
			else
				Top = t;
			if (b < rc.top)
				Bottom = rc.top;
			else if (b > rc.bottom)
				Bottom = rc.bottom;
			else
				Bottom = b;

			if ((Right - Left)%2 == 1)
				Right--;
			if ((Bottom - Top)%2 == 1)
				Bottom--;
			// If need multimonitor support
			/*POINT pt;
			pt.x = l;
			pt.y = t;
			HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(NULL, &mi);
			if (l < mi.rcWork.left)
				Left = mi.rcWork.left;
			if (l > mi.rcWork.right)
				Left = mi.rcWork.right;
			if (r < mi.rcWork.left)
				Right = mi.rcWork.left;
			if (r > mi.rcWork.right)
				Right = mi.rcWork.right;
			if (t < mi.rcWork.top)
				Top = mi.rcWork.top;
			if (t > mi.rcWork.bottom)
				Top = mi.rcWork.bottom;
			if (b < mi.rcWork.top)
				Bottom = mi.rcWork.top;
			if (b > mi.rcWork.bottom)
				Bottom = mi.rcWork.bottom;*/
		}

		// Set screen area left border
		void CRecorder::SetLeft(const int & l)
		{
			RECT rc;
			GetWindowRect(GetDesktopWindow(), &rc);
			if (l < rc.left)
				Left = rc.left;
			else if (l > rc.right)
				Left = rc.right;
			else
				Left = l;
			if ((Right - Left)%2 == 1)
				Right--;
			/*POINT pt;
			pt.x = l;
			pt.y = 1;
			HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi;
			GetMonitorInfo(hMonitor, &mi);
			if (l < mi.rcWork.left)
				Left = mi.rcWork.left;
			if (l > mi.rcWork.right)
				Left = mi.rcWork.right;*/

		}

		// Get screen area left border
		int CRecorder::GetLeft()
		{
			return Left;
		}

		// Set screen area right border
		void CRecorder::SetRight(const int & r)
		{
			RECT rc;
			GetWindowRect(GetDesktopWindow(), &rc);
			if (r < rc.left)
				Right = rc.left;
			else if (r > rc.right)
				Right = rc.right;
			else
				Right = r;
			if ((Right - Left)%2 == 1)
				Right--;
			/*POINT pt;
			pt.x = r;
			pt.y = 1;
			HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi;
			GetMonitorInfo(hMonitor, &mi);
			if (r < mi.rcWork.left)
				Right = mi.rcWork.left;
			if (r > mi.rcWork.right)
				Right = mi.rcWork.right;*/
		}

		// Get screen area right border
		int CRecorder::GetRight()
		{
			return Right;
		}

		// Set screen area top border
		void CRecorder::SetTop(const int & t)
		{
			RECT rc;
			GetWindowRect(GetDesktopWindow(), &rc);
			if (t < rc.top)
				Top = rc.top;
			else if (t > rc.bottom)
				Top = rc.bottom;
			else
				Top = t;
			if ((Bottom - Top)%2 == 1)
				Bottom--;
			/*POINT pt;
			pt.x = 1;
			pt.y = t;
			HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi;
			GetMonitorInfo(hMonitor, &mi);
			if (t < mi.rcWork.top)
				Top = mi.rcWork.top;
			if (t > mi.rcWork.bottom)
				Top = mi.rcWork.bottom;*/
		}

		// Get screen area top border
		int CRecorder::GetTop()
		{
			return Top;
		}

		// Set screen area bottom border
		void CRecorder::SetBottom(const int & b)
		{
			RECT rc;
			GetWindowRect(GetDesktopWindow(), &rc);
			if (b < rc.top)
				Bottom = rc.top;
			else if (b > rc.bottom)
				Bottom = rc.bottom;
			else
				Bottom = b;
			if ((Bottom - Top)%2 == 1)
				Bottom--;
			/*POINT pt;
			pt.x = 1;
			pt.y = b;
			HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi;
			GetMonitorInfo(hMonitor, &mi);
			if (b < mi.rcWork.top)
				Bottom = mi.rcWork.top;
			if (b > mi.rcWork.bottom)
				Bottom = mi.rcWork.bottom;*/
		}

		// Get screen area bottom border
		int CRecorder::GetBottom()
		{
			return Bottom;
		}

		// Set video frame rate bottom border
		void CRecorder::SetFrameRate(const double fr)
		{
			FrameRate = fr;
		}

		// Get video frame rate bottom border
		double CRecorder::GetFrameRate()
		{
			return FrameRate;
		}

		//////////////////////////////////////////////////////////////////////////
		//							Protected methods							//
		//////////////////////////////////////////////////////////////////////////

		// Record movie
		void CRecorder::Record(int i)
		{
			Command = RC_RECORD;
		}
		
		// Pause record
		void CRecorder::Pause(int i)
		{
			Command = RC_PAUSE;
		}

		// Resume record
		void CRecorder::Resume(int i)
		{
			Command = RC_RESUME;
		}

		// Stop record
		void CRecorder::Stop(int i)
		{
			Command = RC_STOP;
		}
	}
}