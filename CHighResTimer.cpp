#include "stdafx.h"
#include "CHighResTimer.h"

double HighResTimer::frequency;

HighResTimer::HighResTimer()
{

}

HighResTimer::~HighResTimer()
{

}

int HighResTimer::Init()
{
    static bool init = false;

    if(init) //Only allow call to QueryPerformanceFrequency once globally
        return 1;

    LARGE_INTEGER qpf;

    if(!QueryPerformanceFrequency(&qpf))
        return 0;
	
    frequency = (qpf.QuadPart / 1000.0); //milliseconds

    init = true;
    return 1;
}

void HighResTimer::Reset()
{
    QueryPerformanceCounter(&start);
}

double HighResTimer::ElapsedMillis()
{
    QueryPerformanceCounter(&stop);

    return (stop.QuadPart - start.QuadPart) / frequency;
}

__int64 HighResTimer::ElapsedMicro()
{
    QueryPerformanceCounter(&stop);

    return (stop.QuadPart - start.QuadPart) / (frequency / 1000.0);
}
