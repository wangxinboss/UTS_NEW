#include "StdAfx.h"
#include "GC5024.h"
#include "CommonFunc.h"

GC5024::GC5024(void):GCSensor()
{
	name = _T("GC5024");
	sensorType = Sensor_GC5024;
	//sid_len = 5;
}


GC5024::~GC5024(void)
{
}

int GC5024::do_prog_otp(int page, int addr, const void *data, int len)
{
	const char *cData = (const char *)data;
	char databuf[2];
	//一个时序只能写一个数据
	for (int i = 0; i < len; i++)
	{
		databuf[0]= 0x01;
		dev->i2c_write(BIT8_BIT8, 0xf7, databuf, 1);

		databuf[0]= 0xae;
		dev->i2c_write(BIT8_BIT8, 0xf9, databuf, 1);

		databuf[0]= 0xae;
		dev->i2c_write(BIT8_BIT8, 0xfc, databuf, 1);

		databuf[0]= 0x94;
		dev->i2c_write(BIT8_BIT8, 0xfa, databuf, 1);

		databuf[0]= 0x80;
		dev->i2c_write(BIT8_BIT8, 0xd4, databuf, 1);
	
	
		databuf[0]= addr+i*8;
		dev->i2c_write(BIT8_BIT8, 0xd5, databuf, 1);

		databuf[0]= cData[i];
		dev->i2c_write(BIT8_BIT8, 0xd6, databuf, 1);
	
		Sleep(1);
		databuf[0]= 0x40;
		dev->i2c_write(BIT8_BIT8, 0xf3, databuf, 1);
	}

	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}

int GC5024::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	char databuf[2];
	//一个时序只能读一个数据
	for (int i = 0; i < len; i++)
	{
		databuf[0]= 0x01;
		dev->i2c_write(BIT8_BIT8, 0xf7, databuf, 1);

		databuf[0]= 0xae;
		dev->i2c_write(BIT8_BIT8, 0xf9, databuf, 1);

		databuf[0]= 0xae;
		dev->i2c_write(BIT8_BIT8, 0xfc, databuf, 1);

		databuf[0]= 0x94;
		dev->i2c_write(BIT8_BIT8, 0xfa, databuf, 1);

		databuf[0]= 0x80;
		dev->i2c_write(BIT8_BIT8, 0xd4, databuf, 1);

	
		databuf[0]= addr+i*8;
		dev->i2c_write(BIT8_BIT8, 0xd5, databuf, 1);

		Sleep(1);
		databuf[0]= 0x20;
		dev->i2c_write(BIT8_BIT8, 0xd3, databuf, 1);

		dev->i2c_read(BIT8_BIT8, 0xd7, cData+i, 1);
	}

	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}