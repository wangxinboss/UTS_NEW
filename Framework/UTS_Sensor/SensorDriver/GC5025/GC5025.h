#pragma once

#include <stdint.h>
#include "../../SensorDriver.h"

class GC5025 : public GCSensor
{
public:
	GC5025(void);
	~GC5025(void);

	int do_prog_otp(int page, int addr, const void *data, int len);
	int do_read_otp(int page, int addr, void *data, int len);

	int do_get_sid(uint8_t *id);
	BOOL GetSensorId(__out CString &strSensorId);
	int wb_writeback(uint8_t *regs, int len);

	unsigned char  Gc5025_ReadData(USHORT ReadAddress);
};

