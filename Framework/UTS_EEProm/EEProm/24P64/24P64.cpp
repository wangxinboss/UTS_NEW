#include "StdAfx.h"
#include <string>
#include "UTS.h"
#include "UTSDefine.h"
#include "AdapterDev.h"
#include "24P64.h"


EE24P64::EE24P64(UTS::BaseDevice *dev, int i2cAddr) : EEPROMDriver(dev, i2cAddr)
{
	pageSize = 32;
	size = 8*SIZE_K;
	i2cMode = UTS::BIT16_BIT8;
}
//-----------------------------------------------------------------------------
int EE24P64::Write(int addr, const void *data, int len)
{
	if (addr + len > size) return -1;
	char dataBuf[2];

	dataBuf[0] = 0x06;
	int ret = Adev->i2c_write(i2cMode,i2cAddr,0x8000,dataBuf,1);
	if (ret != 0 ) {
		return -1;
	}

	Sleep(100);

	const char *cData = (const char *)data;
	while (len > 0) {
		int realLen = min(pageSize-addr%pageSize, len);
		ret = Adev->i2c_write(i2cMode,i2cAddr,addr,cData,realLen);

		if (ret != 0 ) {
			//uts.log.Error(_T("EEPROM I2C Write Error!"));
			return -1;
		}
		Sleep(20); 
		len -= realLen;
		addr += realLen;
		cData += realLen;
	}

    dataBuf[0] = 0x0E;
	ret = Adev->i2c_write(i2cMode,i2cAddr,0x8000,dataBuf,1);
	if (ret != 0 ) {
		return -1;
	}

	Sleep(100);

	return (int)cData - (int)data;
}
