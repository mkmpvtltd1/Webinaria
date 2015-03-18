#pragma once

struct SInterval
{
    REFERENCE_TIME Start;
	REFERENCE_TIME End;
    SInterval * Next;
    bool In(REFERENCE_TIME Time);
	SInterval():Start(-1), End(-1), Next(NULL)
	{
	}
	void Reset(void)
	{
		Start = End= -1;
		Next = NULL;
	}
};

struct SPosInterval
{
    unsigned int HorShift;
    unsigned int VertShift;
    REFERENCE_TIME Start;
	REFERENCE_TIME End;
    SPosInterval * Next;
	SPosInterval()
		:HorShift(0)
		,VertShift(0)
		,Start(-1)
		,End(-1)
		,Next(NULL)
	{
	}
    SPosInterval * IntervalByTime(REFERENCE_TIME Time);
};

struct STextInterval
{
    int				Left;
	int				Top;
    unsigned int	Color;
    bool			IsSelected;
    REFERENCE_TIME	Start;
	REFERENCE_TIME	End;
    STextInterval * Next;
    wchar_t *		String;
    LOGFONT			lf;
};

struct SArrowInterval
{
    unsigned int Color;
    unsigned int Width;
    bool IsSelected;
    int x1, y1, x2, y2;
    REFERENCE_TIME Start, End;
    SArrowInterval * Next;
};
