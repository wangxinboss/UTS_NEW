#ifndef _S5K4H7YX_H_
#define _S5K4H7YX_H_

#include <stdint.h>
#include "../../SensorDriver.h"


class S5K4H7YX : public SamsungSensor
{
public:
	S5K4H7YX();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

	int wb_writeback(uint8_t *regs, int len);
	int do_get_sid(uint8_t *id);
	BOOL GetSensorId(__out CString &strSensorId);
};

#endif
