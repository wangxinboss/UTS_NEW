#include "StdAfx.h"
#include <string>
#include "UTSDefine.h"
#include "AdapterDev.h"
#include "24C64D.h"

EEFM24C64D::EEFM24C64D(UTS::BaseDevice *dev, int i2cAddr) : EEPROMDriver(dev, i2cAddr)
{
	pageSize = 32;
	size = 8*SIZE_K;
	i2cMode = UTS::BIT16_BIT8;
}
