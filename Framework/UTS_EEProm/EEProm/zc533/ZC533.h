#ifndef _ZC533_H_
#define _ZC533_H_

#include <stdint.h>
#include "../../EEProm.h"

class ZC533 : public UTS::EEPROMDriver
{
public:
	ZC533(UTS::BaseDevice *dev, int i2cAddr);

	int Write(int addr, const void *data, int len);
	int Read(int addr, void *data, int len);

	int nSlaveAddr;
};

#endif