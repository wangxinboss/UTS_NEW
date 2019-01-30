#include "StdAfx.h"
#include <string>
#include "UTSDefine.h"
#include "AdapterDev.h"
#include "24C64.h"

EE24C64::EE24C64(UTS::BaseDevice *dev, int i2cAddr) : EEPROMDriver(dev, i2cAddr)
{
	pageSize = 32;
	size = 8*SIZE_K;
	i2cMode = UTS::BIT16_BIT8;
}
