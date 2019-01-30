#include "StdAfx.h"
#include <string>
#include "UTSDefine.h"
#include "AdapterDev.h"
#include "GT9767.h"

EEGT9767::EEGT9767(UTS::BaseDevice *dev, int i2cAddr) : EEPROMDriver(dev, i2cAddr)
{
	pageSize = 32;
	size = 8*SIZE_K;
	i2cMode = UTS::BIT16_BIT8;
}
