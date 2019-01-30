#include "StdAfx.h"
#include "Hi846.h"
#include "CommonFunc.h"

Hi846::Hi846(void):HynixSensor()
{
	name = _T("Hi846");
	sensorType = Sensor_Hi846;
	sid_len = 10;
}


Hi846::~Hi846(void)
{
}


int Hi846::do_prog_otp(int page, int addr, const void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	char pagedata[2];

	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0A02, pagedata, 1);
	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata, 1);

	Sleep(50);
	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0F02, pagedata, 1);//PLL disable

	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x071A, pagedata, 1);//CP TRI_H

	pagedata[0]= 0x09;
	dev->i2c_write(BIT16_BIT8, 0x071B, pagedata, 1);//IPGM TRIM_H

	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0d04, pagedata, 1);//FSync output enable 
	
	pagedata[0]= 0x07;
	dev->i2c_write(BIT16_BIT8, 0x0d00, pagedata, 1);//FSync output Drivabiliy

	pagedata[0]= 0x10;
	dev->i2c_write(BIT16_BIT8, 0x003E, pagedata, 1); //OTP RW
	// 	WriteSensorReg(CurrentSensor.SlaveID,0x070F,0x05, I2CMode16_8);    //// ?????????????OTP data rewrite
	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata, 1);//stand by off

	pagedata[0]= (addr>>8)&0xFF;
	dev->i2c_write(BIT16_BIT8, 0x070a, pagedata, 1);

	pagedata[0]= addr&0xFF;
	dev->i2c_write(BIT16_BIT8, 0x070b, pagedata, 1);

	pagedata[0]= 0x02;
	dev->i2c_write(BIT16_BIT8, 0x0702, pagedata, 1);

    Sleep(50);
	for (int i=0; i<len; i++)
	{   
		pagedata[0]= cData[i];
		dev->i2c_write(BIT16_BIT8, 0x0706, pagedata, 1);
		Sleep(1);	
	}

	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata, 1);
	Sleep(50);

	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x003E, pagedata, 1); //OTP RW

	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata, 1);
	Sleep(50);
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}


int Hi846::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	char pagedata[2];

	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0A02, pagedata, 1);
	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata, 1);
	Sleep(10);

	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0F02, pagedata, 1);//PLL disable

	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x071A, pagedata, 1);//CP TRI_H
	pagedata[0]= 0x09;
	dev->i2c_write(BIT16_BIT8, 0x071B, pagedata, 1);//IPGM TRIM_H
	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0d04, pagedata, 1);//FSync output enable 
	pagedata[0]= 0x07;
	dev->i2c_write(BIT16_BIT8, 0x0d00, pagedata, 1);//FSync output Drivabiliy
	pagedata[0]= 0x10;
	dev->i2c_write(BIT16_BIT8, 0x003E, pagedata, 1);//OTP RW
	//	WriteSensorReg(CurrentSensor.SlaveID,0x070F,0x05, I2CMode16_8);    //// ?????????????OTP data rewrite
	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata, 1);//stand by off
	
	pagedata[0]= (addr>>8)&0xFF;
	dev->i2c_write(BIT16_BIT8, 0x070a, pagedata, 1);
	pagedata[0]= addr&0xFF;
	dev->i2c_write(BIT16_BIT8, 0x070b, pagedata, 1);
	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0702, pagedata, 1);

	for (int i=0;i<len;i++)
	{
		dev->i2c_read(BIT16_BIT8, 0x0708, cData+i, 1);
	}

	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata, 1);
	Sleep(10);

	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x003E, pagedata, 1);//OTP RW
	
	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata, 1);
	Sleep(10);
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}


BOOL Hi846::GetSensorId(__out CString &strSensorId)
{
	strSensorId = _T("");
	BYTE wFuseId[10] = {0};
	do_read_otp(0,0x00,wFuseId,10);
	for ( int j=0; j<10; j++)
	{ 
		strSensorId.AppendFormat(_T("%02X"), wFuseId[j]);
	} 

	return TRUE;
}

int Hi846::wb_writeback(uint8_t *regs, int len)
{
	dev->i2c_write(BIT16_BIT8, 0x0078, (const char*)regs, len);
	return 0;
}