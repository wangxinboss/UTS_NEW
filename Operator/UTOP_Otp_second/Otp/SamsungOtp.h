#pragma once
#include "baseotp.h"

class SamsungOtp :
    public BaseOtp
{
public:
    SamsungOtp(BaseDevice *dev);
    ~SamsungOtp(void);

//protected:
    int get_wb_gain(WB_RATIO *ratio, WB_RATIO *ratio_target, WB_GAIN *gain);
    int get_wb_cali_data(WB_GAIN *gain, void *out);

    int write_reg(char *buf, int len);
    int chagetexttohex(char *buf, char *output, int len);
};

