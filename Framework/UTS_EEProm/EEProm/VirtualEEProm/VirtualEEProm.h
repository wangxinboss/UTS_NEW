#ifndef _VIRTUAL_EEPROM_H_
#define _VIRTUAL_EEPROM_H_

#include <stdint.h>
#include "../../EEProm.h"

#define VIRTUAL_EEPROM_FILE    "VirtualEEProm.bin"

class VirtualEEProm : public UTS::EEPROMDriver
{
public:
	VirtualEEProm(UTS::BaseDevice *dev, int i2cAddr);

    int Write(int addr, const void *data, int len);
    int Read(int addr, void *data, int len);

private:
    FILE *fp;
};

#endif
