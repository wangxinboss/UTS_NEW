#pragma once

#include <stdint.h>
#include "../../SensorDriver.h"

class GC5024 : public GCSensor
{
public:
	GC5024(void);
	~GC5024(void);

	int do_prog_otp(int page, int addr, const void *data, int len);
	int do_read_otp(int page, int addr, void *data, int len);
};

