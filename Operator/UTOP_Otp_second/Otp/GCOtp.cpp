#include "StdAfx.h"
#include "GCOtp.h"

#pragma comment(lib, "UTS_Sensor.lib")
using namespace UTS;

GCOtp::GCOtp(BaseDevice *dev) : BaseOtp(dev)
{
}


GCOtp::~GCOtp(void)
{
}

//-------------------------------------------------------------------------------------------------
int GCOtp::get_wb_gain(WB_RATIO *ratio, WB_RATIO *ratio_target, WB_GAIN *gain)
{
    get_gain_by_wb_ratio(ratio,ratio_target,0x8080,gain);
    return 0;
}

int GCOtp::get_wb_cali_data(WB_GAIN *gain, void *out)
{
    uint8_t *buf = (uint8_t *)out;
    put_be_val(gain->GGain, buf, 2);
    put_be_val(gain->RGain, buf+2, 2);
    put_be_val(gain->BGain, buf+4, 2);
    put_be_val(gain->GGain, buf+6, 2);
    return 8;
}

int GCOtp::write_reg(char *buf, int len)
{
    unsigned long addr, reg;
    char addr_buf[5];
    addr_buf[4] = 0;
    char reg_buf[5];
    reg_buf[2] = 0;
    reg_buf[3] = 0;
    int ret = 0;
    for (int i = 0; i < len;) {
        if (buf[i] == '/') {
            for (; i < len;) {
                i++;
                if (buf[i] == '\n') break;
            }
        } else if (buf[i] == 's') {
            i++;
            for (int j = 0; j < 4; j++) {
                addr_buf[j] = buf[i];
                i++;
            }
            addr = strtoul(addr_buf, NULL, 16);
            for (int j = 0; j < 2; j++) {
                reg_buf[j] = buf[i];
                i++;
            }
            reg = strtoul(reg_buf, NULL, 16);
            sensor->dev->write_sensor((WORD)addr, (WORD)reg);
        } else if (buf[i] == 'p') {
            Sleep(10); i++;
        } else i++;
    }
    return 0;
}

int GCOtp::chagetexttohex(char *buf, char *output, int len)
{
    int reg;
    int count = 0;
    char	addr_buf[5];
    addr_buf[4] = 0;
    char reg_buf[5] = { 0 };

    int ret = 0;
    for (int i = 0; i < len;) {
        if (buf[i] == '/') {
            for (; i < len;) {
                i++;
                if (buf[i] == '\n') break;
            }
        } else if (buf[i] == 'm' || buf[i] == 'M') {
            i += 5;
            for (int m = 0; m < 70; m++) {
                memcpy(reg_buf, &buf[i], 2);
                i += 2;
                reg = strtoul(reg_buf, NULL, 16);
                *(output + count) = reg;
                count++;
                if (buf[i] == 'p' || buf[i] == 'P') break;
                if (buf[i] == '\r' || buf[i] == '\n') break;
            }
        } else i++;
    }
    return count;
}

