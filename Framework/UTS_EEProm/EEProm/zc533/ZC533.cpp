#include "StdAfx.h"
#include <string>
#include "UTSDefine.h"
#include "AdapterDev.h"
//#include "EEprom_I2C.h"
#include "ZC533.h"


ZC533::ZC533(UTS::BaseDevice *dev, int i2cSlave) : EEPROMDriver(dev, i2cSlave)
{
	pageSize = 64;
	size = 160*pageSize;
	//i2cMode = UTS::BIT16_BIT8;
	nSlaveAddr = i2cSlave;
}

int ZC533::Write(int addr, const void *data, int len)
{
	const char *cData = (const char *)data;
	unsigned char dataBuf[2] = {0};
	unsigned char readbuf[64]= {0},writebuf[66]= {0};
	BOOL res;

	int writepage = addr/pageSize;
	int writeaddr = addr%pageSize;

	while (len > 0) {
		int realLen = min(pageSize - writeaddr, len);

		//2. send command: EID 0x00 0x00; // read
		dataBuf[0] = writepage >> 2;
		dataBuf[1] = ((writepage & 0x03)<< 6);
		res = I2CWrite(0xB0, 0, 0x00, 2, dataBuf);
		Sleep(10);

		//3. receive data: char[0…63]; // 读出64 个字节的数据，存入数组;
		res = I2CRead(0xB0,0, 0x00, 64, readbuf);
		Sleep(10);

		//4. change data:
		memcpy(readbuf + writeaddr,cData,realLen);

		//5. send command: EID 0xC0 0x00; // erase
		dataBuf[0] = 0x80 + (writepage >> 2);
		dataBuf[1] = ((writepage & 0x03)<< 6);
		res = I2CWrite(0xB0, 0, 0x00, 2, dataBuf);
		Sleep(10);

		//6. send command: EID 0x40 0x00; // write
		//7. send data: char[0…63]; // 写入数组中的64 个字节值。
		memcpy(writebuf+2,readbuf,64);
		writebuf[0] = 0x40 + (writepage >> 2);
		writebuf[1] = ((writepage & 0x03)<< 6);
		res = I2CWrite(0xB0, 0, 0x00, 66, writebuf);
		Sleep(20); 
		len -= realLen;
		writepage ++;
		writeaddr = 0;
		cData += realLen;
	}
	
// 	//8. restart. 
// 	Power(3,0);
// 	Sleep(500);
// 	Power(3,float(2.8));
// 	Sleep(500);

	return (int)cData - (int)data;
}

int ZC533::Read(int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	unsigned char dataBuf[2] = {0};
	unsigned char readbuf[64]= {0},writebuf[66]= {0};
	BOOL res;

	int readpage = addr/pageSize;
	int readaddr = addr%pageSize;

	while (len > 0) 
	{
		int realLen = min(pageSize - readaddr, len);

		//2. send command: EID 0x00 0x00; // read
		dataBuf[0] = readpage >> 2;
		dataBuf[1] = ((readpage & 0x03)<< 6);
		res = I2CWrite(0xB0, 0, 0x00, 2, dataBuf);
		Sleep(10);

		//3. receive data: char[0…63]; // 读出64 个字节的数据，存入数组;
		res = I2CRead(0xB0,0, 0x00, 64, readbuf);
		Sleep(10);

		//
		memcpy(cData,readbuf + readaddr,realLen);
		len -= realLen;
		readaddr = 0;
		cData += realLen;
		readpage ++;
	}

	return (int)cData - (int)data;
}