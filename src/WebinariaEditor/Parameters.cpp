#include "stdafx.h"
#include "Parameters.h"

using namespace EditorApplication::EditorLogical;
CParameters::CParameters(void)
:AllDeleteInterval()
,WebDeleteInterval()
,AudioDeleteInterval()
,CurrentTextInterval(NULL)
{
	ZeroMemory(&WebShiftInterval, sizeof(WebShiftInterval));
	WebShiftInterval.Start = -1;
	WebShiftInterval.End = -1;
	WebShiftInterval.VertShift = 100;
	ZeroMemory(&TextInterval, sizeof(TextInterval));
	TextInterval.Start = -1;
	TextInterval.End = -1;
	ZeroMemory(&ArrowInterval, sizeof(ArrowInterval));
	ArrowInterval.Start = -1;
	ArrowInterval.End = -1;
}

CParameters::~CParameters(void)
{
	Clear();
}

void CParameters::Clear()
{
	SInterval * Temp;

	// Clear deleted intervals
	while (AllDeleteInterval.Next != NULL)
	{
		Temp = AllDeleteInterval.Next;
		AllDeleteInterval.Next = AllDeleteInterval.Next->Next;
		delete Temp;
		Temp = NULL;
	}

	// Clear web deleted intervals
	while (WebDeleteInterval.Next != NULL)
	{
		Temp = WebDeleteInterval.Next;
		WebDeleteInterval.Next = WebDeleteInterval.Next->Next;
		delete Temp;
		Temp = NULL;
	}
	while (AudioDeleteInterval.Next != NULL)
	{
		Temp = AudioDeleteInterval.Next;
		AudioDeleteInterval.Next = AudioDeleteInterval.Next->Next;
		delete Temp;
		Temp = NULL;
	}

	SPosInterval * PosTemp;

	// Clear web shift intervals
	while (WebShiftInterval.Next != NULL)
	{
		PosTemp = WebShiftInterval.Next;
		WebShiftInterval.Next = WebShiftInterval.Next->Next;
		delete PosTemp;
		PosTemp = NULL;
	}

	STextInterval * TextTemp;

	// Clear text intervals
	while (TextInterval.Next != NULL)
	{
		TextTemp = TextInterval.Next;
		TextInterval.Next = TextInterval.Next->Next;
		delete[] TextTemp->String;
		delete TextTemp;
		TextTemp = NULL;
	}
	CurrentTextInterval = NULL;

	SArrowInterval * ArrowTemp;

	// Clear text intervals
	while (ArrowInterval.Next != NULL)
	{
		ArrowTemp = ArrowInterval.Next;
		ArrowInterval.Next = ArrowInterval.Next->Next;
		delete ArrowTemp;
		ArrowTemp = NULL;
	}
	CurrentArrowInterval = NULL;
}
void __fastcall CParameters::AddInterval(SInterval * pInt, const LONGLONG& Start, const LONGLONG& End)
{
	while (pInt->Next != NULL)
	{
		if (pInt->Next->End == Start) 
		{
			pInt->Next->End = End;
			return;
		}
		else if (pInt->Next->Start == End) 
		{
			pInt->Next->Start = Start;
			return;
		}
		else if (pInt->Next->End > Start) 
			break;
		pInt = pInt->Next;
	}
	SInterval * New = new SInterval;
	New->Next = pInt->Next;
	pInt->Next = New;
	New->Start = Start;
	New->End = End;
}
void __fastcall CParameters::RemoveInterval(SInterval* pInt, const LONGLONG& Start, const LONGLONG& End)
{
	SInterval * Prev = NULL;
	while ((pInt->Next != NULL)&&(pInt->Start < Start))
	{
		Prev = pInt;
		pInt = pInt->Next;
	}

	while (pInt != NULL)
	{
		if ((pInt->Start == Start) && (pInt->End <= End))
		{
			Prev->Next = pInt->Next;
			delete pInt;
			pInt = NULL;
			if (Prev->Next != NULL)
			{
				if (Prev->Next->Start >= End)
					return;
			}
			else
				return;
		}
		else if ((pInt->Start == Start) && (pInt->End > End))
		{
			pInt->Start = End;
			return;
		}
		else if ((pInt->Start < Start) && (pInt->End <= End))
		{
			pInt->End = Start;
			return;
		}
		else if ((pInt->Start < Start) && (pInt->End > End))
		{
			SInterval * New = new SInterval;
			New->Next = pInt->Next;
			pInt->Next = New;
			New->Start = End;
			New->End = pInt->End;
			pInt->End = Start;
			return;
		}	
		Prev = pInt;
		pInt = pInt->Next;
	}

}
void CParameters::AddDeleteAllInterval(const long long & StartFrame, const long long & EndFrame)
{
	SInterval * Temp = &AllDeleteInterval;
	while (Temp->Next != NULL)
	{
		if (Temp->Next->End == StartFrame) 
		{
			Temp->Next->End = EndFrame;
			DeleteDouble(WebDeleteInterval, StartFrame, EndFrame);
			return;
		}
		else if (Temp->Next->Start == EndFrame) 
		{
			Temp->Next->Start = StartFrame;
			DeleteDouble(WebDeleteInterval, StartFrame, EndFrame);
			return;
		}
		else if (Temp->Next->End > StartFrame) 
			break;
		Temp = Temp->Next;
	}
	SInterval * New = new SInterval;
	New->Next = Temp->Next;
	Temp->Next = New;
	New->Start = StartFrame;
	New->End = EndFrame;
	//New->Next = NULL;

	DeleteDouble(WebDeleteInterval, StartFrame, EndFrame);
}

void CParameters::AddDeleteWebInterval(const long long & StartFrame, const long long & EndFrame)
{
	AddInterval(&WebDeleteInterval, StartFrame, EndFrame);
}

void CParameters::RemoveDeleteWebInterval(const long long & StartFrame, const long long & EndFrame)
{
	RemoveInterval(&WebDeleteInterval, StartFrame, EndFrame);
}
void CParameters::AddDeleteAudioInterval(const long long & StartFrame, const long long & EndFrame)
{
	AddInterval(&AudioDeleteInterval, StartFrame, EndFrame);
}
void CParameters::RemoveDeleteAudioInterval(const long long & StartFrame, const long long & EndFrame)
{
	RemoveInterval(&AudioDeleteInterval, StartFrame, EndFrame);
}

void CParameters::AddShiftWebInterval(const long long & StartFrame, const long long & EndFrame,
									  const unsigned int & HorShift, const unsigned int & VertShift)
{
	SPosInterval * Temp = &WebShiftInterval;
	while (Temp->Next != NULL)
	{
		if (Temp->Next->End == StartFrame) 
		{
			Temp->Next->End = EndFrame;
			return;
		}
		else if (Temp->Next->Start == EndFrame) 
		{
			Temp->Next->Start = StartFrame;
			return;
		}
		else if (Temp->Next->End > StartFrame) 
			break;
		Temp = Temp->Next;
	}
	SPosInterval * New = new SPosInterval;
	New->Next = Temp->Next;
	Temp->Next = New;
	New->Start = StartFrame;
	New->End = EndFrame;
	New->HorShift = HorShift;
	New->VertShift = VertShift;
	//New->Next = NULL;
}

void CParameters::AddTextInterval(const long long & StartFrame, const long long & EndFrame,
								  const wchar_t * String,
								  const LOGFONT & lf,
								  const int & Left, const int & Top,
								  const unsigned int & Color)
{
	STextInterval * New = new STextInterval;
	New->Next = TextInterval.Next;
	TextInterval.Next = New;
	New->Start = StartFrame;
	New->End = EndFrame;
	int iLen = wcslen(String) + 1;
	New ->String = new wchar_t[iLen];
	wcscpy_s(New->String, iLen, String);
	New->lf = lf;
	New->Left = Left;
	New->Top = Top;
	New->Color = Color;
	if (NULL != CurrentTextInterval)
				CurrentTextInterval->IsSelected = false;
	CurrentTextInterval = New;
}

void CParameters::DeleteCurrentTextInterval()
{
	if (!CurrentTextInterval)
		return;
	STextInterval * Current = &TextInterval;
	// Clear text intervals
	while ((NULL != Current->Next) && (CurrentTextInterval != Current->Next))
	{
		Current = Current->Next;
	}
	if (CurrentTextInterval == Current->Next)
	{
		Current->Next = Current->Next->Next;
		delete CurrentTextInterval;
		CurrentTextInterval = NULL;
	}
}

void CParameters::AddArrowInterval(	const long long & StartFrame, const long long & EndFrame,
									const int & x1, const int & y1,
									const int & x2, const int & y2,
									const unsigned int & Color,
									const unsigned int & Width)
{
	SArrowInterval * New = new SArrowInterval;
	New->Next = ArrowInterval.Next;
	ArrowInterval.Next = New;
	New->Start = StartFrame;
	New->End = EndFrame;
	New->x1 = x1;
	New->y1 = y1;
	New->x2 = x2;
	New->y2 = y2;
	New->Color = Color;
	New->Width = Width;
	if (NULL != CurrentArrowInterval)
				CurrentArrowInterval->IsSelected = false;
	CurrentArrowInterval = New;
}

void CParameters::DeleteCurrentArrowInterval()
{
	if (!CurrentArrowInterval)
		return;
	SArrowInterval * Current = &ArrowInterval;
	// Clear text intervals
	while ((NULL != Current->Next) && (CurrentArrowInterval != Current->Next))
	{
		Current = Current->Next;
	}
	if (CurrentArrowInterval == Current->Next)
	{
		Current->Next = Current->Next->Next;
		delete CurrentArrowInterval;
		CurrentArrowInterval = NULL;
	}
}

STextInterval * CParameters::HitText(const unsigned int & X,
									 const unsigned int & Y,
									 const float & Reduction)
{
	STextInterval * CurrentText = TextInterval.Next;
	HDC hdc = GetDC( NULL );
	HDC mhdc = CreateCompatibleDC( hdc );
	if( !mhdc )
		return NULL;
	ReleaseDC( NULL, hdc );

	while (NULL != CurrentText)
	{
		HFONT hFont = CreateFontIndirect(&CurrentText->lf);
		HFONT hOldFont = (HFONT)SelectObject(mhdc, hFont);
		SIZE sz;
		GetTextExtentPoint32(mhdc, CurrentText->String, wcslen(CurrentText->String), &sz);
		long NX = (long)(((float)X) / Reduction);
		long NY = (long)(((float)Y) / Reduction);
		if ((CurrentText->Left <= NX)&&
			(CurrentText->Left + sz.cx >= NX)&&
			(CurrentText->Top <= NY)&&
			(CurrentText->Top + sz.cy >= NY))
			return CurrentText;
		SelectObject(mhdc, hOldFont);
		DeleteObject(hFont);
		CurrentText = CurrentText->Next;
	}
	DeleteDC(mhdc);
	return NULL;
}

SArrowInterval * CParameters::HitArrow(	const unsigned int & X,
										const unsigned int & Y,
										const float & Reduction)
{
	SArrowInterval * CurrentArrow = ArrowInterval.Next;

	while (NULL != CurrentArrow)
	{
		long NX = (long)(((float)X) / Reduction);
		long NY = (long)(((float)Y) / Reduction);
		if ((((CurrentArrow->x1 <= NX)&&(CurrentArrow->x2 >= NX))||
			 ((CurrentArrow->x1 >= NX)&&(CurrentArrow->x2 <= NX)))&&
			(((CurrentArrow->y1 <= NY)&&(CurrentArrow->y2 >= NY))||
			 ((CurrentArrow->y1 >= NY)&&(CurrentArrow->y2 <= NY))))
			return CurrentArrow;
		CurrentArrow = CurrentArrow->Next;
	}
	return NULL;
}

//	0 - enabled
//  1 - disabled
//	2 - different for interval
int CParameters::GetWebState(const long long & StartTrackFrameNumber, const long long & EndTrackFrameNumber)
{
	long long StartFrame = GetRealFrameNumber(StartTrackFrameNumber);
	long long EndFrame = GetRealFrameNumber(EndTrackFrameNumber);
	SInterval * Temp = &WebDeleteInterval;
	while (Temp->Next != NULL)
	{
		if ((Temp->Next->Start <= StartFrame) && (Temp->Next->End >= EndFrame)) 
			return 1;
		else 
			if (((Temp->Next->Start <= StartFrame) && (Temp->Next->End > StartFrame) && (Temp->Next->End < EndFrame))|| 
				((Temp->Next->Start > StartFrame) && (Temp->Next->End >= EndFrame) && (Temp->Next->Start < EndFrame)))
			return 2;
		Temp = Temp->Next;
	}				
	return 0;		
}
int CParameters::GetAudioState(const long long & StartTrackFrameNumber, const long long & EndTrackFrameNumber)
{
	long long StartFrame = GetRealFrameNumber(StartTrackFrameNumber);
	long long EndFrame = GetRealFrameNumber(EndTrackFrameNumber);
	SInterval * Temp = &AudioDeleteInterval;
	while (Temp->Next != NULL)
	{
		if ((Temp->Next->Start <= StartFrame) && (Temp->Next->End >= EndFrame)) 
			return 1;
		else 
			if (((Temp->Next->Start <= StartFrame) && (Temp->Next->End > StartFrame) && (Temp->Next->End < EndFrame))|| 
				((Temp->Next->Start > StartFrame) && (Temp->Next->End >= EndFrame) && (Temp->Next->Start < EndFrame)))
			return 2;
		Temp = Temp->Next;
	}				
	return 0;		
}

long long CParameters::GetRealFrameNumber(const long long & TrackFrameNumber) const
{
	SInterval * Temp = const_cast<SInterval*>(&AllDeleteInterval);
	long long NonDeletedSum = 0;
	long long MainPos = 0;
	if (Temp->Next == NULL)
		return TrackFrameNumber;
	while (Temp->Next != NULL)
	{
		NonDeletedSum += Temp->Next->Start - Temp->End;
		if (NonDeletedSum > TrackFrameNumber) 
		{
			MainPos += TrackFrameNumber - NonDeletedSum + Temp->Next->Start - Temp->End;
			break;
		}
		MainPos = Temp->Next->End;
		Temp = Temp->Next;
	}
	if (NonDeletedSum < TrackFrameNumber) 
		MainPos += TrackFrameNumber - NonDeletedSum;
	return MainPos;
}

void CParameters::DeleteDouble(SInterval & inter, const long long & StartFrame, const long long & EndFrame)
{
	SInterval * Temp = &inter;
	while (Temp->Next != NULL)
	{
		if (Temp->Next->Start >= StartFrame) 
		{
			if (Temp->Next->End <= EndFrame) 
			{
				while (Temp->Next != NULL)
				{
					if (Temp->Next->End <= EndFrame)
					{
						SInterval * Del = Temp->Next;
						Temp->Next = Temp->Next->Next;
						delete Del;
						Del = NULL;
					}
					else
					{
						if (Temp->Next->Start < EndFrame) 
							Temp->Next->Start = EndFrame;
						break;
					}
				}
				break;
			}
			else
			{
				Temp->Next->Start = EndFrame;
				break;
			}
		}
		Temp = Temp->Next;
	}
}

const SInterval * CParameters::GetAllDeletedInterval() const
{
	return &AllDeleteInterval;
}

const SInterval * CParameters::GetWebDeletedInterval()  const
{
	return &WebDeleteInterval;
}
const SInterval * CParameters::GetAudioDeletedInterval()  const
{
	return &AudioDeleteInterval;
}
const SPosInterval * CParameters::GetWebPosInterval()  const
{
	return &WebShiftInterval;
}

const SPosInterval * CParameters::GetWebPosInterval(const long long & Pos)  const
{
	long long RealPos = GetRealFrameNumber(Pos);
	SPosInterval * Temp = WebShiftInterval.Next;
	while (Temp != NULL)
	{
		if ((Temp->Start <= RealPos) && (RealPos<= Temp->End)) 
			return Temp;
		Temp = Temp->Next;
	}
	return &WebShiftInterval;
}

const STextInterval * CParameters::GetTextInterval()  const
{
	return &TextInterval;
}

const SArrowInterval * CParameters::GetArrowInterval()  const
{
	return &ArrowInterval;
}
