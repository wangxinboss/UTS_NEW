#ifndef _IMX363_H_
#define _IMX363_H_

#include <stdint.h>
#include "../../SensorDriver.h"

class IMX363 : public SonySensor
{
public:
	IMX363();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    int do_get_sid(uint8_t *id);
    BOOL GetSensorId(__out CString &strSensorId);
	int GetThermal(int *meter);
};

#endif
