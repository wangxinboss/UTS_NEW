#ifndef _24C32_H_
#define _24C32_H_

#include <stdint.h>
#include "../../EEProm.h"

class EE24C32 : public UTS::EEPROMDriver
{
public:
	EE24C32(UTS::BaseDevice *dev, int i2cAddr);
};

#endif