#pragma once

#include "AudioGrabber.h"
#include "ScreenGrabber.h"
#include "WDMGrabber.h"
//#include "CaptureGraphBuilder.h"

namespace WebinariaApplication
{
	namespace WebinariaLogical
	{
		// Enum with recorder states
		enum RecorderStates{RS_RECORDING = 0,
							RS_PAUSE = 1,
							RS_STOP = 2};

		enum RecorderCommand{	RC_NOCOM = -1,
								RC_RECORD = 0,
								RC_STOP = 1,
								RC_PAUSE = 2,
								RC_RESUME = 3,
								RC_CHANGE_DEVICE = 4};

		enum AdditionalFlag {	AF_CAP_AUDIO = 0,
								AF_CAP_CAMERA = 1,
								AF_REMOVE_BACK = 2,
								AF_REMOVE_MOUSE = 3,
								AF_SET_TEXT = 4,
								AF_SET_ARROW = 5,
								AF_ACTIVE_WINDOW = 6,
								AF_CHANGE_SIZE = 7};

		// Class provide interface to recording movie
		class CRecorder
		{
		// Public methods
		public:
			// Default constructor
			CRecorder(void);

			// Virtual destructor
			virtual ~CRecorder(void);

			// Record movie
			static void Record();

			// Pause|Resume record
			static void PauseResume();

			// Start|stop capturing
			static void RecordStop();

			// Stop record
			static void Stop();

			// Change device
			static void ChangeDevice();

			// Get additional flag
			static bool GetAdditionalFlag(AdditionalFlag flag);

			// Set additional flags
			static void SetAdditionalFlag(AdditionalFlag flag, bool state);

			// Get current recorder state
			static RecorderStates GetCurrentRecorderState();

			// Get current recorder command
			static RecorderCommand GetCurrentRecorderCommand();

			// Callback 
			static void ProvideCommand();

			// Set screen area borders
			static void SetBorders(const int & l, const int & t, const int & r, const int & b);

			// Set screen area left border
			static void SetLeft(const int & l);

			// Get screen area left border
			static int GetLeft();

			// Set screen area right border
			static void SetRight(const int & r);

			// Get screen area right border
			static int GetRight();

			// Set screen area top border
			static void SetTop(const int & t);

			// Get screen area top border
			static int GetTop();

			// Set screen area bottom border
			static void SetBottom(const int & b);

			// Get screen area bottom border
			static int GetBottom();

			// Set video frame rate bottom border
			static void SetFrameRate(const double fr);

			// Get video frame rate bottom border
			static double GetFrameRate();
	
		// Protected methods
		protected:
			// Record movie
			static void Record(int);

			// Pause record
			static void Pause(int);

			// Resume record
			static void Resume(int);

			// Stop record
			static void Stop(int);

		// Protected fields
		protected:
			static RecorderStates State;						// Current recorder state
			static RecorderCommand Command;						// Current recorder command 
			static bool	fAudio;									// Flags
			static bool	fCamera;
			static bool	fBack;
			static bool	fMouse;
			static bool	fText;
			static bool	fArrow;	
			static int	Left,
						Right,
						Top,
						Bottom;
			static double FrameRate;								
			static bool fActiveWindow;							// true - if need capture current window
																// false otherwise
			static bool fChangeSize;							// true - just change screen parameters
																// false otherwise
		};
	}

	namespace WL = WebinariaLogical;
}
