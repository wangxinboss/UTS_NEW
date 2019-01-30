#pragma once
#include "baseotp.h"

struct OV_WB
{
    uint8_t rg_msb;
    uint8_t bg_msb;
    uint8_t rg_bg_lsb;
};

class OVOtp :
    public BaseOtp
{
public:
    OVOtp(BaseDevice *dev);
    ~OVOtp(void);

protected:
    int get_wb_gain(WB_RATIO *ratio, WB_RATIO *ratio_target, WB_GAIN *gain);
    int get_wb_cali_data(WB_GAIN *gain, void *out);
    int get_group(int flag_addr, int max_group);
    void code_wb_info(const WB_DATA_UCHAR *wb, WB_RATIO *ratio, OV_WB *ov_wb);
};

