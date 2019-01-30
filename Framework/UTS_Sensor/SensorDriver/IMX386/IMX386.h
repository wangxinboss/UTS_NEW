#pragma once

#include <stdint.h>
#include "../../SensorDriver.h"


class IMX386: public SonySensor
{
public:
	IMX386();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    int do_get_sid(uint8_t *id);
    BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);
};

