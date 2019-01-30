#ifndef _24C64_H_
#define _24C64_H_

#include <stdint.h>
#include "../../EEProm.h"

class EE24C64 : public UTS::EEPROMDriver
{
public:
	EE24C64(UTS::BaseDevice *dev, int i2cAddr);
};

#endif