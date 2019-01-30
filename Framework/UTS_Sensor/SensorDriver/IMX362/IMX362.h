#ifndef _IMX362_H_
#define _IMX362_H_

#include <stdint.h>
#include "../../SensorDriver.h"

#define OTP_IMX362_MAX_SIZE     415
#define OTP_IMX362_START_ADDR   0x7220

class IMX362 : public OVSensor
{
public:
	IMX362();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    int do_get_sid(uint8_t *id);
    BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);
};

#endif
