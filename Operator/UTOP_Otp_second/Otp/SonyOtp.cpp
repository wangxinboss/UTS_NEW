#include "StdAfx.h"
#include "SonyOtp.h"

#pragma comment(lib, "UTS_Sensor.lib")
using namespace UTS;

SonyOtp::SonyOtp(BaseDevice *dev) : BaseOtp(dev)
{
}


SonyOtp::~SonyOtp(void)
{
}

//-------------------------------------------------------------------------------------------------
int SonyOtp::get_wb_gain(WB_RATIO *ratio, WB_RATIO *ratio_target, WB_GAIN *gain)
{
    get_gain_by_wb_ratio(ratio,ratio_target,0x100,gain);
    return 0;
}

int SonyOtp::get_wb_cali_data(WB_GAIN *gain, void *out)
{
    uint8_t *buf = (uint8_t *)out;
    put_be_val(gain->GGain, buf, 2);
    put_be_val(gain->RGain, buf+2, 2);
    put_be_val(gain->BGain, buf+4, 2);
    put_be_val(gain->GGain, buf+6, 2);
    return 8;
}

