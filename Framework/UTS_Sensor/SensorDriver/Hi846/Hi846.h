#pragma once

#include <stdint.h>
#include "../../SensorDriver.h"

class Hi846: public HynixSensor
{
public:
	Hi846(void);
	~Hi846(void);

	int do_prog_otp(int page, int addr, const void *data, int len);
	int do_read_otp(int page, int addr, void *data, int len);
	BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);
};

