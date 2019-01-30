#ifndef _FM24C64D_H_
#define _FM24C64D_H_

#include <stdint.h>
#include "../../EEProm.h"

class EEFM24C64D : public UTS::EEPROMDriver
{
public:
	EEFM24C64D(UTS::BaseDevice *dev, int i2cAddr);
};

#endif