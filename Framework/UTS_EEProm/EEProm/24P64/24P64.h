#ifndef _24P64_H_
#define _24P64_H_

#include <stdint.h>
#include "../../EEProm.h"

class EE24P64 : public UTS::EEPROMDriver
{
public:
	EE24P64(UTS::BaseDevice *dev, int i2cAddr);

	int Write(int addr, const void *data, int len);
};

#endif