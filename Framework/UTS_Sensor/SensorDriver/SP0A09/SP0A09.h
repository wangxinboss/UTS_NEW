#ifndef _SP0A09_H_
#define _SP0A09_H_

#include <stdint.h>
#include "../../SensorDriver.h"


class SP0A09 : public SuperPixSensor
{
public:
	SP0A09();

    int do_prog_otp(int page, int addr, const void *data, int len);
    int do_read_otp(int page, int addr, void *data, int len);

	int set_exposure(int e);
	int get_exposure(void);
	int get_exposure_settings(int e, std::map<int, int> &regs);
};

#endif
