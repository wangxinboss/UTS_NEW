#include "StdAfx.h"
#include <string>
#include "UTSDefine.h"
#include "24C16.h"
#include "AdapterDev.h"


EE24C16::EE24C16(UTS::BaseDevice *dev, int i2cAddr) : EEPROMDriver(dev, i2cAddr)
{
    pageSize = 16;
    size = 2*SIZE_K;
    i2cMode = UTS::BIT8_BIT8;
    blockSize = 256;
}

int EE24C16::Write(int addr, const void *data, int len)
{
    if (addr + len > EEPROMDriver::size) 
		return -1;

    const char *cData = (const char *)data;
    while (len > 0)
    {
        int blockIdx = addr / blockSize;
        int blockOffset = addr % blockSize;

        i2cAddr = 0xA0 + (blockIdx<<1);

        int realLen = min(blockSize-addr%blockSize, len);
        if (EEPROMDriver::Write(blockOffset, cData, realLen) < 0)
            return -1;
        len -= realLen;
        addr += realLen;
        cData += realLen;
    }

    return (int)cData - (int)data;
}

int EE24C16::Read(int addr, void *data, int len)
{
    if (addr + len > EEPROMDriver::size) 
		return -1;

    char *cData = (char *)data;
    while (len > 0)
    {
        int blockIdx = addr / blockSize;
        int blockOffset = addr % blockSize;

        i2cAddr = 0xA0 + (blockIdx<<1);

        int realLen = min(blockSize-addr%blockSize, len);
        if (EEPROMDriver::Read(blockOffset, cData, realLen) < 0)
            return -1;
        len -= realLen;
        addr += realLen;
        cData += realLen;
    }
    return (int)cData - (int)data;
}
