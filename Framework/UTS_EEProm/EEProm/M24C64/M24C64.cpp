#include "StdAfx.h"
#include <string>
#include "UTSDefine.h"
#include "AdapterDev.h"
#include "M24C64.h"

M24C64::M24C64(UTS::BaseDevice *dev, int i2cAddr) : EEPROMDriver(dev, i2cAddr)
{
	pageSize = 32;
	size = 8*SIZE_K;
	i2cMode = UTS::BIT16_BIT8;
}
int M24C64::Read(int addr, void *data, int len)
{
	if (addr + len > size) return -1;
	i2cAddr=i2cAddr+1;

	unsigned char *cData = (unsigned char *)data;
	while (len > 0) {
		int realLen = min(pageSize-addr%pageSize, len);
		int ret = Adev->i2c_read(i2cMode,i2cAddr,addr,cData,realLen);

		if (ret != 0) {
			return SET_ERROR(EEPROM_ERROR_I2C);
		}
		len -= realLen;
		addr += realLen;
		cData += realLen;
	}
	SET_ERROR(EEPROM_ERROR_NO);
	return (int)cData - (int)data;
}
//-----------------------------------------------------------------------------
// int M24C64::Write(int addr, const void *data, int len)
// {
// 	if (addr + len > size) return -1;
// 	char dataBuf[2];
// 
// 	dataBuf[0] = 0x06;
// 	int ret = Adev->i2c_write(i2cMode,i2cAddr,0x8000,dataBuf,1);
// 	if (ret != 0 ) {
// 		return -1;
// 	}
// 
// 	Sleep(100);
// 
// 	const char *cData = (const char *)data;
// 	while (len > 0) {
// 		int realLen = min(pageSize-addr%pageSize, len);
// 		ret = Adev->i2c_write(i2cMode,i2cAddr,addr,cData,realLen);
// 
// 		if (ret != 0 ) {
// 			//uts.log.Error(_T("EEPROM I2C Write Error!"));
// 			return -1;
// 		}
// 		Sleep(20); 
// 		len -= realLen;
// 		addr += realLen;
// 		cData += realLen;
// 	}
// 
// 	dataBuf[0] = 0x0E;
// 	ret = Adev->i2c_write(i2cMode,i2cAddr,0x8000,dataBuf,1);
// 	if (ret != 0 ) {
// 		return -1;
// 	}
// 
// 	Sleep(100);
// 
// 	return (int)cData - (int)data;
// }