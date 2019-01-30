#include "StdAfx.h"
#include "GC5025.h"
#include "CommonFunc.h"

GC5025::GC5025(void):GCSensor()
{
	name = _T("GC5025");
	sensorType = Sensor_GC5025;
	sid_len = 9;
}


GC5025::~GC5025(void)
{
}

//-------------------------------------------------------------------------------------------------
int GC5025::do_prog_otp(int page, int addr, const void *data, int len)
{
	uint8_t *cData = (uint8_t *)data;
	char BurnOtpData[200];
	char BurnData = 0;

	memset(BurnOtpData, 0, 200);

	for(int i = 0; i < len; i++)
	{
		BurnOtpData[i] = cData[i];

		dev->i2c_write(BIT8_BIT8,0xf7,0x01);
		dev->i2c_write(BIT8_BIT8,0xf9,0x00);
		dev->i2c_write(BIT8_BIT8,0xfc,0x2e);		
		dev->i2c_write(BIT8_BIT8,0xfa,0xb0);    //OTP clk enable

		USHORT Address_High = ((addr+i*8) >> 8) & 0x03;
		USHORT Address_Low  =  (addr+i*8) & 0xFF;

		dev->i2c_write(BIT8_BIT8,0xd4, 0x84+Address_High); //OTP enable[7]  OTP page Select[2] AddressHigh[1:0]
		dev->i2c_write(BIT8_BIT8,0xd5, Address_Low);
		BurnData = BurnOtpData[i];
		if(BurnData == 0) //如果数据为0，表示没有数据，则跳过不烧
		{
			continue;
		}

		dev->i2c_write(BIT8_BIT8,0xd6, BurnData);	

		dev->i2c_write(BIT8_BIT8,0xd8,0x20);
		dev->i2c_write(BIT8_BIT8,0xf3,0x42);
		Sleep(3);
		dev->i2c_write(BIT8_BIT8,0xd8,0x10);
		Sleep(24);
		dev->i2c_write(BIT8_BIT8,0xd8,0x40);
		dev->i2c_write(BIT8_BIT8,0xf3,0x42);
		Sleep(3);
		dev->i2c_write(BIT8_BIT8,0xd8,0x10);
		Sleep(24);

	}	 
	return TRUE;
}
//-------------------------------------------------------------------------------------------------
int GC5025::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	int starAddr = addr;
	int endAddr = addr + len -1;

	dev->i2c_write(BIT8_BIT8,0xf7,0x01);
	dev->i2c_write(BIT8_BIT8,0xf9,0x00);
	dev->i2c_write(BIT8_BIT8,0xfc,0x2e);	
	dev->i2c_write(BIT8_BIT8,0xfa,0xb0);          //Enable OTP clk read mode


	for(int i = 0; i < len; i++)
	{
		unsigned char _ReadValue;
		_ReadValue = 0;
		_ReadValue = Gc5025_ReadData(addr+i*8);
		cData[i] = _ReadValue;

	}

	return TRUE;
}


unsigned char  GC5025::Gc5025_ReadData(USHORT ReadAddress)
{	
	//dev->i2c_write(BIT8_BIT8,0xf7,0x01);
	//dev->i2c_write(BIT8_BIT8,0xf9,0x00);
	//dev->i2c_write(BIT8_BIT8,0xfc,0x2e);	
	//dev->i2c_write(BIT8_BIT8,0xfa,0xb0);          //Enable OTP clk read mode

	USHORT Address_High = (ReadAddress >> 8) & 0x03;
	USHORT Address_Low  =  ReadAddress & 0xFF;

	dev->i2c_write(BIT8_BIT8,0xd4, 0x84+Address_High); //OTP enable[7]  OTP page Select[2] AddressHigh[1:0]
	dev->i2c_write(BIT8_BIT8,0xd5, Address_Low); //Address_Low byte
	Sleep(1);
	dev->i2c_write(BIT8_BIT8,0xf3, 0x20); //OTP Read		
	unsigned char ret=0;
	ret = dev->i2c_read(BIT8_BIT8,0xd7);

	return ret;
}
//-------------------------------------------------------------------------------------------------

int GC5025::do_get_sid(uint8_t *id)
{
	return do_read_otp(0, 0x3B0, id, sid_len);
}

BOOL GC5025::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;

	BYTE wFuseId[9] = {0};

	do_read_otp(0,0x3B0,wFuseId,sid_len);

	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		strSensorId.AppendFormat(_T("%02X"), wFuseId[i]);
	}

	return TRUE;
}

int GC5025::wb_writeback(uint8_t *regs, int len)
{
	//regs len = 8, 排列G,G,R,B  H,L
	//R_Gain = RG_Typical/RG_Current
	//B_Gain = BG_Typical/BG_Current

	//write R_Gain to 0xc7 and 0xc4 [2:0]
	//write G_Gain to 0xc6 and 0xc4 [6:4]
	//write G_Gain to 0xc9 and 0xc5 [2:0]
	//write B_Gain to 0xc8 and 0xc5 [6:4]

	BYTE data[6]={0};

	data[0] = regs[5]|regs[1]<<4;  //0xc4
	data[1] = regs[3]|regs[7]<<4;	//0xc5
	data[2] = regs[0];  //0xc6
	data[3] = regs[4];	//0xc7
	data[4] = regs[6];	//0xc8
	data[5] = regs[2];	//0xc9

	dev->write_sensor(0xc4, data, 6);

	return 0;
}


//int GC5025::do_prog_otp(int page, int addr, const void *data, int len)
//{
//	const char *cData = (const char *)data;
//	char databuf[2];
//	//一个时序只能写一个数据
//	for (int i = 0; i < len; i++)
//	{
//		databuf[0]= 0x01;
//		dev->i2c_write(BIT8_BIT8, 0xf7, databuf, 1);
//
//		databuf[0]= 0xae;
//		dev->i2c_write(BIT8_BIT8, 0xf9, databuf, 1);
//
//		databuf[0]= 0xae;
//		dev->i2c_write(BIT8_BIT8, 0xfc, databuf, 1);
//
//		databuf[0]= 0x94;
//		dev->i2c_write(BIT8_BIT8, 0xfa, databuf, 1);
//
//		databuf[0]= 0x80;
//		dev->i2c_write(BIT8_BIT8, 0xd4, databuf, 1);
//	
//	
//		databuf[0]= addr+i*8;
//		dev->i2c_write(BIT8_BIT8, 0xd5, databuf, 1);
//
//		databuf[0]= cData[i];
//		dev->i2c_write(BIT8_BIT8, 0xd6, databuf, 1);
//	
//		Sleep(1);
//		databuf[0]= 0x40;
//		dev->i2c_write(BIT8_BIT8, 0xf3, databuf, 1);
//	}
//
//	return SET_ERROR(SENSORDRIVER_ERROR_NO);
//}
//
//int GC5025::do_read_otp(int page, int addr, void *data, int len)
//{
//	unsigned char *cData = (unsigned char *)data;
//	char databuf[2];
//	//一个时序只能读一个数据
//	for (int i = 0; i < len; i++)
//	{
//		databuf[0]= 0x01;
//		dev->i2c_write(BIT8_BIT8, 0xf7, databuf, 1);
//
//		databuf[0]= 0xae;
//		dev->i2c_write(BIT8_BIT8, 0xf9, databuf, 1);
//
//		databuf[0]= 0xae;
//		dev->i2c_write(BIT8_BIT8, 0xfc, databuf, 1);
//
//		databuf[0]= 0x94;
//		dev->i2c_write(BIT8_BIT8, 0xfa, databuf, 1);
//
//		databuf[0]= 0x80;
//		dev->i2c_write(BIT8_BIT8, 0xd4, databuf, 1);
//
//	
//		databuf[0]= addr+i*8;
//		dev->i2c_write(BIT8_BIT8, 0xd5, databuf, 1);
//
//		Sleep(1);
//		databuf[0]= 0x20;
//		dev->i2c_write(BIT8_BIT8, 0xd3, databuf, 1);
//
//		dev->i2c_read(BIT8_BIT8, 0xd7, cData+i, 1);
//	}
//
//	return SET_ERROR(SENSORDRIVER_ERROR_NO);
//}
