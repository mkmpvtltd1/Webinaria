#pragma once

#include "intervals.h"

namespace EditorApplication
{
	namespace EditorLogical
	{
		class CParameters
		{
		protected:
			void __fastcall AddInterval(SInterval * pInt, const LONGLONG& Start, const LONGLONG& End);
			void __fastcall RemoveInterval(SInterval * pInt, const LONGLONG& Start, const LONGLONG& End);
		public:
			CParameters(void);

			void Clear();
		
			virtual ~CParameters(void);

			void AddDeleteAllInterval(const long long & StartFrame, const long long & EndFrame);

			void AddDeleteWebInterval(const long long & StartFrame, const long long & EndFrame);
			void RemoveDeleteWebInterval(const long long & StartFrame, const long long & EndFrame);

			void AddDeleteAudioInterval(const long long & StartFrame, const long long & EndFrame);
			void RemoveDeleteAudioInterval(const long long & StartFrame, const long long & EndFrame);

			void AddShiftWebInterval(const long long & StartFrame, const long long & EndFrame,
									 const unsigned int & HorShift, const unsigned int & VertShift);

			void AddTextInterval(const long long & StartFrame, const long long & EndFrame,
								 const wchar_t * String,
								 const LOGFONT & lf,
								 const int & Left, const int & Top,
								 const unsigned int & Color);

			void DeleteCurrentTextInterval();

			void AddArrowInterval(	const long long & StartFrame, const long long & EndFrame,
									const int & x1, const int & y1,
									const int & x2, const int & y2,
									const unsigned int & Color,
									const unsigned int & Width);

			void DeleteCurrentArrowInterval();

			STextInterval * HitText(const unsigned int & X,
									const unsigned int & Y,
									const float & Reduction);
			SArrowInterval * HitArrow(	const unsigned int & X,
										const unsigned int & Y,
										const float & Reduction);
			
			long long GetRealFrameNumber(const long long & TrackFrameNumber) const;			
			
			int GetWebState(const long long & StartTrackFrameNumber, const long long & EndTrackFrameNumber);
			int GetAudioState(const long long & StartTrackFrameNumber, const long long & EndTrackFrameNumber);
							
			const SInterval * GetAllDeletedInterval()  const;
			const SInterval * GetWebDeletedInterval()  const;
			const SInterval * GetAudioDeletedInterval()  const;

			const SPosInterval * GetWebPosInterval()  const;

			const SPosInterval * GetWebPosInterval(const long long & Pos)  const;

			const STextInterval * GetTextInterval()  const;

			const SArrowInterval * GetArrowInterval()  const;

		protected:

			void DeleteDouble(SInterval & inter, const long long & StartFrame, const long long & EndFrame);

		public:

			SInterval		AllDeleteInterval;
			SInterval		WebDeleteInterval;
			SInterval		AudioDeleteInterval;
			SPosInterval	WebShiftInterval;
			STextInterval	TextInterval;
			SArrowInterval	ArrowInterval;

			STextInterval*  CurrentTextInterval;
			SArrowInterval* CurrentArrowInterval;
		};
	}
}