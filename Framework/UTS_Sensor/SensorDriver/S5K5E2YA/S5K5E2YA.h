#ifndef _S5K5E2YA_H_
#define _S5K5E2YA_H_

#include <stdint.h>
#include "../../SensorDriver.h"

#define OTP_S5K5E2YA_MAX_SIZE			510
#define OTP_S5K5E2YA_START_ADDR   0x0A04
#define OTP_S5K5E2YA_PAGE_SIZE		0x40

class S5K5E2YA : public SamsungSensor
{
public:
	S5K5E2YA();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    int do_get_sid(uint8_t *id);
	BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);
};

#endif
