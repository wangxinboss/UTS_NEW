#include "stdafx.h"
#include "UTSBase.h"

namespace UTS
{
    FPSCounter::FPSCounter()
    {
        dFPS = 0.0;
        fps_first = TRUE;
    }

    FPSCounter::~FPSCounter()
    {

    }

    double FPSCounter::GetFPS(void)
    {
        if (fps_first)
        {
            display_count = 0;
            QueryPerformanceCounter(&start_time);
            fps_first = FALSE;
        }
        else
        {
            QueryPerformanceCounter(&end_time);
            QueryPerformanceFrequency(&freq);
            double gap = ((double)(end_time.QuadPart - start_time.QuadPart)) / ((double)freq.QuadPart);

            if (gap >= 2)    // up-date per 2 second
            {
                fps_first = TRUE;
                dFPS = display_count / gap;
            }
        }
        display_count++;
        return dFPS;
    }
}
