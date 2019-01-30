#ifndef _GC5034_H_
#define _GC5034_H_

#include <stdint.h>
#include "../../SensorDriver.h"

#define OTP_GC5034_MAX_SIZE     415
#define OTP_GC5034_START_ADDR   0x7010

class GC5034 : public GCSensor
{
public:
	GC5034();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

    //int do_get_sid(uint8_t *id);
    //BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);

	int set_exposure(int e);
	int get_exposure(void);
	int get_exposure_settings(int e, std::map<int, int> &regs);
};

#endif
