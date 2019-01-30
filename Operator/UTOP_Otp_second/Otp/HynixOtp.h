#pragma once
#include "baseotp.h"

struct Hynix_WB
{
    uint8_t rg_msb;
	uint8_t rg_lsb;
    uint8_t bg_msb;
    uint8_t bg_lsb;
};

class HynixOtp :
    public BaseOtp
{
public:
    HynixOtp(BaseDevice *dev);
    ~HynixOtp(void);

protected:
    int get_wb_gain(WB_RATIO *ratio, WB_RATIO *ratio_target, WB_GAIN *gain);
    int get_wb_cali_data(WB_GAIN *gain, void *out);
    int get_group(int flag_addr, int max_group);
    void code_wb_info(const WB_DATA_UCHAR *wb, WB_RATIO *ratio, Hynix_WB *hi_wb);
};

