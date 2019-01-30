#include "StdAfx.h"
#include "HynixOtp.h"


HynixOtp::HynixOtp(BaseDevice *dev) : BaseOtp(dev)
{
}

HynixOtp::~HynixOtp(void)
{
}

//-------------------------------------------------------------------------------------------------
int HynixOtp::get_wb_gain(WB_RATIO *ratio, WB_RATIO *ratio_target, WB_GAIN *gain)
{
    get_gain_by_wb_ratio(ratio, ratio_target, 0x0200, gain);
    return 0;
}

int HynixOtp::get_wb_cali_data(WB_GAIN *gain, void *out)
{
	uint8_t *buf = (uint8_t *)out;
	put_be_val(gain->GGain, buf, 2);
	put_be_val(gain->GGain, buf+2, 2);
	put_be_val(gain->RGain, buf+4, 2);
	put_be_val(gain->BGain, buf+6, 2);
    return 8;
}

void HynixOtp::code_wb_info(const WB_DATA_UCHAR *wb, WB_RATIO *ratio, Hynix_WB *hi_wb)
{
    get_wb_ratio(wb, ratio, 512.0);
    hi_wb->rg_msb = (uint8_t)(ratio->r_g >> 8);
	hi_wb->rg_lsb = (uint8_t)(ratio->r_g & 0xff);
    hi_wb->bg_msb = (uint8_t)(ratio->b_g >> 8);
    hi_wb->bg_lsb = (uint8_t)(ratio->b_g & 0xff);
}

int HynixOtp::get_group(int flag_addr, int max_group)
{
    uint8_t valid = 0;
    if (sensor->ReadOtp(0, flag_addr, (char *)&valid, 1) < 0)
        return SET_ERROR(OTPCALI_ERROR_SENSOR);

    return BaseOtp::get_group(valid, max_group, OV_GROUP_FLAG_VALID);
}
//-------------------------------------------------------------------------------------------------