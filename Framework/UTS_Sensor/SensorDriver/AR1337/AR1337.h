#ifndef _AR1337_H_
#define _AR1337_H_

#include <stdint.h>
#include "../../SensorDriver.h"


class AR1337 : public SamsungSensor
{
public:
	AR1337();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

	int do_get_sid(uint8_t *id);
    BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);
};

#endif
