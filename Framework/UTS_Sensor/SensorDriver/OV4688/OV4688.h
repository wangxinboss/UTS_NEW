#pragma once

#include <stdint.h>
#include "../../SensorDriver.h"



class OV4688 : public OVSensor
{
public:
	OV4688();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    int do_get_sid(uint8_t *id);
    BOOL GetSensorId(__out CString &strSensorId);
};

