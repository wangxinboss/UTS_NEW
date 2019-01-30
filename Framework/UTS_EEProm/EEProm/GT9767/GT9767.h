#ifndef _GT9767_H_
#define _GT9767_H_

#include <stdint.h>
#include "../../EEProm.h"

class EEGT9767 : public UTS::EEPROMDriver
{
public:
	EEGT9767(UTS::BaseDevice *dev, int i2cAddr);
};

#endif