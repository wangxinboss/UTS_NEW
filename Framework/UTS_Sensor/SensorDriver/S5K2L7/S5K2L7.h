#ifndef _S5K2L7_H_
#define _S5K2L7_H_

#include <stdint.h>
#include "../../SensorDriver.h"

#define OTP_S5K2L7_MAX_SIZE     415
#define OTP_S5K2L7_START_ADDR   0x7220

class S5K2L7 : public SamsungSensor
{
public:
	S5K2L7();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    int do_get_sid(uint8_t *id);
    BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);
};

#endif
