#pragma once
#include "OISControl.h"


class VirtualOIS : public UTS::OISDriver
{
public:
    VirtualOIS(UTS::BaseDevice *dev);
    ~VirtualOIS(void);

     
};


