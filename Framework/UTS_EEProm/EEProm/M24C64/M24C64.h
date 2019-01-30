#ifndef _M24C64_H_
#define _M24C64_H_

#include <stdint.h>
#include "../../EEProm.h"

class M24C64 : public UTS::EEPROMDriver
{
	enum EEPROM_ERROR
	{
		EEPROM_ERROR_NO,
		EEPROM_ERROR_NOTSUPPORT,
		EEPROM_ERROR_I2C,
	};
public:
	M24C64(UTS::BaseDevice *dev, int i2cAddr);
	int Read(int addr, void *data, int len);
};

#endif