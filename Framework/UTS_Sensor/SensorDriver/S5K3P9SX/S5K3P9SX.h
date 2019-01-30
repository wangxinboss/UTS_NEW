#pragma once

#include <stdint.h>
#include "../../SensorDriver.h"

class S5K3P9SX : public SamsungSensor
{
public:
	S5K3P9SX();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    int wb_writeback(uint8_t *regs, int len);
    int do_get_sid(uint8_t *id);

	virtual BOOL GetSensorId(__out CString &strSensorId);
};

