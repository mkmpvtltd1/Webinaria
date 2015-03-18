//
#include <strmif.h>
#include "intervals.h"

bool SInterval::In(REFERENCE_TIME Time)
{
    SInterval * Current = this;
    while (Current != NULL)
    {
        if ((Time >= Current->Start) && (Time <= Current->End))
            return true;
        Current = Current->Next;
    }
    return false;
}

SPosInterval * SPosInterval::IntervalByTime(REFERENCE_TIME Time)
{
    SPosInterval * Temp = this;
    while (Temp != NULL)
    {
        if ((Temp->Start <= Time) && (Time<= Temp->End)) 
            return Temp;
        Temp = Temp->Next;
    }
    return NULL;
}
