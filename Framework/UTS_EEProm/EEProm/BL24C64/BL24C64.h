#ifndef _BL24C64_H_
#define _BL24C64_H_

#include <stdint.h>
#include "../../EEProm.h"

class BL24C64 : public UTS::EEPROMDriver
{
public:
	BL24C64(UTS::BaseDevice *dev, int i2cAddr);
};

#endif