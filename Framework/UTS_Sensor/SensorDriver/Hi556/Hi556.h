#ifndef _Hi556_H_
#define _Hi556_H_

#include <stdint.h>
#include "../../SensorDriver.h"

#define OTP_Hi556_MAX_SIZE     415
#define OTP_Hi556_START_ADDR   0x7010

class Hi556 : public HynixSensor
{
public:
	Hi556();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    int do_get_sid(uint8_t *id);
    BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);
};

#endif
