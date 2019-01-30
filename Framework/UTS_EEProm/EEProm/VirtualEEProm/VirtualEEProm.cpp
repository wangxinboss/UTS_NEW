#include "StdAfx.h"
#include <string>
#include "UTSDefine.h"
#include "VirtualEEProm.h"

VirtualEEProm::VirtualEEProm(UTS::BaseDevice *dev, int i2cAddr) : EEPROMDriver(dev, i2cAddr)
{
	size = 16*SIZE_K;

    fp = fopen(VIRTUAL_EEPROM_FILE, "wb+");
//	if (!fp) uts.log.Error(_T("Open %s failed!"),VIRTUAL_EEPROM_FILE);
}

int VirtualEEProm::Write(int addr, const void *data, int len)
{
    if (!fp) return -1;

    fseek(fp, addr, SEEK_SET);
    fwrite(data, 1, len, fp);
    fflush(fp);

    return len;
}
int VirtualEEProm::Read(int addr, void *data, int len)
{
    if (!fp) return -1;

    fseek(fp, 0, SEEK_END);
    int fileLen = ftell(fp);
    if (addr >= fileLen)
    {
        memset(data, 0xFF, len);
        return len;
    }

    fseek(fp, addr, SEEK_SET);
    fread(data, 1, len, fp);

    return len;
}
