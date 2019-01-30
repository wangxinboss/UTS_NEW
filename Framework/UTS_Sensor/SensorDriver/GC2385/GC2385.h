#pragma once

#include <stdint.h>
#include "../../SensorDriver.h"

class GC2385 : public GCSensor
{
public:
	GC2385(void);
	~GC2385(void);

	int do_prog_otp(int page, int addr, const void *data, int len);
	int do_read_otp(int page, int addr, void *data, int len);
};

