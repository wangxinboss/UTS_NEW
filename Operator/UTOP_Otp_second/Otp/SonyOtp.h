#pragma once
#include "baseotp.h"

class SonyOtp :
    public BaseOtp
{
public:
    SonyOtp(BaseDevice *dev);
    ~SonyOtp(void);

protected:
    int get_wb_gain(WB_RATIO *ratio, WB_RATIO *ratio_target, WB_GAIN *gain);
    int get_wb_cali_data(WB_GAIN *gain, void *out);

};

