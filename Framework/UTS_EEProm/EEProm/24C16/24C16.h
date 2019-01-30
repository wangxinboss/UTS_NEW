#ifndef _24C16_H_
#define _24C16_H_

#include <stdint.h>
#include "../../EEProm.h"

class EE24C16 : public UTS::EEPROMDriver
{
public:
	EE24C16(UTS::BaseDevice *dev, int i2cAddr);

    int Write(int addr, const void *data, int len);
    int Read(int addr, void *data, int len);

private:
    int blockSize;
};

#endif
