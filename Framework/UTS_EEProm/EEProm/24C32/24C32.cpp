#include "StdAfx.h"
#include <string>
#include "UTSDefine.h"
#include "AdapterDev.h"
#include "24C32.h"

EE24C32::EE24C32(UTS::BaseDevice *dev, int i2cAddr) : EEPROMDriver(dev, i2cAddr)
{
	pageSize = 32;
	size = 4*SIZE_K;
	i2cMode = UTS::BIT16_BIT8;
}
