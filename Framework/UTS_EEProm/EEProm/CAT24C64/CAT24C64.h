#ifndef _CAT24C64_H_
#define _CAT24C64_H_

#include <stdint.h>
#include "../../EEProm.h"

class CAT24C64 : public UTS::EEPROMDriver
{
public:
	CAT24C64(UTS::BaseDevice *dev, int i2cAddr);
};

#endif