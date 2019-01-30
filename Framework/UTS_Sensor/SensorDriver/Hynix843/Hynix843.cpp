#include "StdAfx.h"
#include "Hynix843.h"
#include "CommonFunc.h"

#define _Debug 0

Hynix843::Hynix843()
{
	name = _T("Hynix843");
	sensorType = Sensor_Hynix843;
    sid_len = 16;

}

int Hynix843::do_prog_otp(int page, int addr, const void *data, int len)
{
	uint8_t *cData = (uint8_t *)data;
	char BurnOtpData[1000];
	char ReadOtpData[1000];
	char BurnData;
	char ReadData;
	int retrycount = 0;

	memset(BurnOtpData, 0, 1000);
	memset(ReadOtpData, 0, 1000);

	for(int i=0; i<len; i++){
		BurnOtpData[i] = cData[i];
	}

	dev->i2c_write(BIT16_BIT8,0x0A02,0x01);
	dev->i2c_write(BIT16_BIT8,0x0A00,0x00);
	//Sleep(100);
	Sleep(10);
	dev->i2c_write(BIT16_BIT8,0x0F02,0x00);     //PLL disable
	dev->i2c_write(BIT16_BIT8,0x071A,0x01);    //CP TRI_H
	dev->i2c_write(BIT16_BIT8,0x071B,0x09);    //IPGM TRIM_H
	dev->i2c_write(BIT16_BIT8,0x0d04,0x01);    //FSync output enable 
	dev->i2c_write(BIT16_BIT8,0x0d00,0x07);    //FSync output Drivabiliy
	
	dev->i2c_write(BIT16_BIT8,0x003E,0x10);    //OTP RW
    dev->i2c_write(BIT16_BIT8,0x070f,0x05);    //OTP data rewrite
	dev->i2c_write(BIT16_BIT8,0x0A00,0x01);    //stand by off

	dev->i2c_write(BIT16_BIT8,0x070a,(((addr)>>8)&0xff));
	dev->i2c_write(BIT16_BIT8,0x070b,((addr)&0xff));

	dev->i2c_write(BIT16_BIT8,0x0702,0x02);

	Sleep(100);

	for(int i = 0; i<len; i++)
	{
		BurnData = 0;
		BurnData = BurnOtpData[i];
		dev->i2c_write(BIT16_BIT8,0x0706,BurnData);
		Sleep(4);
		ReadData = dev->i2c_read(BIT16_BIT8,0x070c);

		while(((BurnData) != ReadData) && (retrycount < 10))
		{
			retrycount++;
			BurnData = 0;
			BurnData = BurnOtpData[i];
			dev->i2c_write(BIT16_BIT8,0x0706,BurnData);
			Sleep(4);
			ReadData = dev->i2c_read(BIT16_BIT8,0x070c);
		}

		if(retrycount>=10) return -2;
		retrycount = 0;
	}
	//----------------------------------------------------------------//
	//-----------------Compare----------------------------------------//
	//----------------------------------------------------------------//
#if 0
	dev->i2c_write(BIT16_BIT8,0x0A02,0x01);
	dev->i2c_write(BIT16_BIT8,0x0A00,0x00);
	Sleep(10);
	dev->i2c_write(BIT16_BIT8,0x0F02,0x00);     //PLL disable
	dev->i2c_write(BIT16_BIT8,0x071A,0x01);    //CP TRI_H
	dev->i2c_write(BIT16_BIT8,0x071B,0x09);    //IPGM TRIM_H
	dev->i2c_write(BIT16_BIT8,0x0d04,0x01);    //FSync output enable 
	dev->i2c_write(BIT16_BIT8,0x0d00,0x07);    //FSync output Drivabiliy
	dev->i2c_write(BIT16_BIT8,0x003E,0x10);    //OTP RW
	dev->i2c_write(BIT16_BIT8,0x0A00,0x01);    //stand by off

	dev->i2c_write(BIT16_BIT8,0x070a,(((addr)>>8)&0xFF));
	dev->i2c_write(BIT16_BIT8,0x070b,((addr)&0xFF));
	dev->i2c_write(BIT16_BIT8,0x0702, 0x01);
	
	for(int i=0; i<len; i++)
	{
		ReadData = dev->i2c_read(BIT16_BIT8,0x0708);
		ReadOtpData[i] = ReadData;
		//Sleep(10);
	}

	dev->i2c_write(BIT16_BIT8,0x0A00,0x00);
	//Sleep(100);
	Sleep(10);
	dev->i2c_write(BIT16_BIT8,0x003E,0x00);   //OTP RW
	dev->i2c_write(BIT16_BIT8,0x0A00,0x01);
	Sleep(50);

	for(int i=0; i<len; i++){
		if(ReadOtpData[i] != BurnOtpData[i]){
			if (retrycount < 5){
				retrycount ++;
				goto REWRITE;
			}
			return -2;
		}
	}
#endif
	//-------------------------------------------------------------Read End
	dev->i2c_write(BIT16_BIT8,0x0A00,0x00);
	//Sleep(50);
	Sleep(10);
	dev->i2c_write(BIT16_BIT8,0x003E,0x00);   //OTP RW
	dev->i2c_write(BIT16_BIT8,0x0A00,0x01);
	Sleep(50);

   return 0;
}

int Hynix843::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	int starAddr = addr;
	int endAddr = addr + len -1;

	unsigned char ReadOtpValue;
	
	dev->i2c_write(BIT16_BIT8,0x0A02,0x01);
	dev->i2c_write(BIT16_BIT8,0x0A00,0x00);
	Sleep(10);
	dev->i2c_write(BIT16_BIT8,0x0F02,0x00);     //PLL disable
	dev->i2c_write(BIT16_BIT8,0x071A,0x01);    //CP TRI_H
	dev->i2c_write(BIT16_BIT8,0x071B,0x09);    //IPGM TRIM_H
	dev->i2c_write(BIT16_BIT8,0x0d04,0x01);    //FSync output enable 
	dev->i2c_write(BIT16_BIT8,0x0d00,0x07);    //FSync output Drivabiliy
	dev->i2c_write(BIT16_BIT8,0x003E,0x10);    //OTP RW
	dev->i2c_write(BIT16_BIT8,0x0A00,0x01);    //stand by off

	dev->i2c_write(BIT16_BIT8,0x070a,(((addr)>>8)&0xFF));
	dev->i2c_write(BIT16_BIT8,0x070b,((addr)&0xFF));
	dev->i2c_write(BIT16_BIT8,0x0702, 0x01);
	
	for(int i=0; i<len; i++)
	{
		//dev->i2c_write(BIT16_BIT8,0x070a,(((i+addr)>>8)&0xFF));
		//dev->i2c_write(BIT16_BIT8,0x070b,((i+addr)&0xFF));
		//dev->i2c_write(BIT16_BIT8,0x0702, 0x01);
		ReadOtpValue = 0;
		ReadOtpValue = dev->i2c_read(BIT16_BIT8,0x0708);
		cData[i] = ReadOtpValue;
	}

	dev->i2c_write(BIT16_BIT8,0x0A00,0x00);
	//Sleep(100);
	Sleep(10);
	dev->i2c_write(BIT16_BIT8,0x003E,0x00);   //OTP RW
	dev->i2c_write(BIT16_BIT8,0x0A00,0x01);
	Sleep(50);

   return 0;
}
//-----------------------------------------------------------------------------
int Hynix843::do_get_sid(uint8_t *id)
{
    if (do_read_otp(0, 0x7000, (char*)id, sid_len) < 0)
    {
        return -1;
    }
    return 0;
}

BOOL Hynix843::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;

	BYTE wFuseId[9] = {0};
	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if(do_read_otp(0,0x0001,wFuseId,9))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				0x7000 + i, wFuseId[i]);
			return FALSE;
		}
		// Æ´½ÓFUSE_ID×÷ÎªSensorId
		strSensorId.AppendFormat(_T("%0X"), wFuseId[i]);
	}

	return TRUE;
}

int Hynix843::wb_writeback(uint8_t *regs, int len)
{
// 	dev->write_sensor(0x5032, regs, len);
// 
 	int val = dev->read_sensor(0x5000);
 	dev->write_sensor(0x5000, val | 0x08);

	dev->write_sensor(0x5100, regs, len);


	return 0;
}