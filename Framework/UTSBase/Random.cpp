#include "StdAfx.h"
#include "UTSBase.h"

namespace UTS
{
    Random::Random()
    {
        /* Seed the random-number generator with current time so that 
        * the numbers will be different every time we run. 
        */ 
        srand((unsigned)time(nullptr)); 
    }

    Random::~Random()
    {

    }

    double Random::GetRandomDouble()
    {
        return (double)(rand()/(double)RAND_MAX);
    }

    int Random::GetRandomInt(int nMax)
    {
        return (int)(nMax/(float)RAND_MAX * rand());
    }
}
