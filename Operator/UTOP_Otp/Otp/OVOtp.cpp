#include "StdAfx.h"
#include "OVOtp.h"


OVOtp::OVOtp(BaseDevice *dev) : BaseOtp(dev)
{
}

OVOtp::~OVOtp(void)
{
}

//-------------------------------------------------------------------------------------------------
int OVOtp::get_wb_gain(WB_RATIO *ratio, WB_RATIO *ratio_target, WB_GAIN *gain)
{
    get_gain_by_wb_ratio(ratio, ratio_target, 0x400, gain);
    return 0;
}

int OVOtp::get_wb_cali_data(WB_GAIN *gain, void *out)
{
    uint8_t *buf = (uint8_t *)out;
    put_be_val(gain->RGain, buf, 2);
    put_be_val(gain->GGain, buf+2, 2);
    put_be_val(gain->BGain, buf+4, 2);
    return 6;
}

void OVOtp::code_wb_info(const WB_DATA_UCHAR *wb, WB_RATIO *ratio, OV_WB *ov_wb)
{
    get_wb_ratio(wb, ratio, 512.0);
    ov_wb->rg_msb = ratio->r_g >> 2;
    ov_wb->bg_msb = ratio->b_g >> 2;
    ov_wb->rg_bg_lsb = ((ratio->r_g & 0x03) << 6) | ((ratio->b_g & 0x03) << 4);
}

int OVOtp::get_group(int flag_addr, int max_group)
{
    uint8_t valid = 0;
    if (sensor->ReadOtp(0, flag_addr, (char *)&valid, 1) < 0)
        return SET_ERROR(OTPCALI_ERROR_SENSOR);

    return BaseOtp::get_group(valid, max_group, OV_GROUP_FLAG_VALID);
}
//-------------------------------------------------------------------------------------------------