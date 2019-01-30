#ifndef _Hynix843_H_
#define _Hynix843_H_

#include <stdint.h>
#include "../../SensorDriver.h"

#define OTP_Hynix_MAX_SIZE     415
#define OTP_Hynix_START_ADDR   0x0201

class Hynix843 : public HynixSensor
{
public:
	Hynix843();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    int do_get_sid(uint8_t *id);
    BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);
};

#endif