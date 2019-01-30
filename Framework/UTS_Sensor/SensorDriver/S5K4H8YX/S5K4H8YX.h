#ifndef _S5K4H8YX_H_
#define _S5K4H8YX_H_

#include <stdint.h>
#include "../../SensorDriver.h"


class S5K4H8YX : public SamsungSensor
{
public:
	S5K4H8YX();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);
	int wb_writeback(uint8_t *regs, int len);
	BOOL GetSensorId(__out CString &strSensorId);
	int do_lsc_cali();
};

#endif
