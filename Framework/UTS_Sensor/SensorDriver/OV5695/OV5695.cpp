#include "StdAfx.h"
#include "OV5695.h"
#include "CommonFunc.h"

#define _Debug 0

//-------------------------------------------------------------------------------------------------
OV5695::OV5695()
{
	name = _T("OV5695");
	sensorType = Sensor_OV5695;
    sid_len = 16;

}

//-------------------------------------------------------------------------------------------------
int OV5695::do_prog_otp(int page, int addr, const void *data, int len)
{
	//const char *cData = (const char *)data;
	uint8_t *cData = (uint8_t *)data;
	int otp_flag = 0;
	int starAddr = addr;
	int endAddr = addr + len-1;
	int i = 0;
	char ReadOtpData[20];
	char BurnOtpData[20];
	char BurnData;
	int retrycount = 0;
	memset(ReadOtpData, 0, 20);
	memset(BurnOtpData, 0, 20);
	for(i=0; i<len; i++){
		BurnOtpData[i] = cData[i];
	}
	//*******************************Read OTP ***********************************//
#if 0
	starAddr = 0x7220; endAddr = 0x73a8;
	unsigned char ReadOtpValue;
	uint8_t ReadDataValue[600];
	for(i=0;i<600;i++) ReadDataValue[i] = 0;
	int temp1 =	dev->i2c_read(BIT16_BIT8,0x5000);
	dev->i2c_write(BIT16_BIT8,0x5000,(temp1& (~0x10)));

	temp1 =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x40 | temp1);

	temp1 =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x80 | temp1);

	dev->i2c_write(BIT16_BIT8,0x3d88, starAddr>>8); // OTP start address
	dev->i2c_write(BIT16_BIT8,0x3d89, starAddr&0x00ff);
	dev->i2c_write(BIT16_BIT8,0x3d8A, endAddr>>8); // OTP end address
	dev->i2c_write(BIT16_BIT8,0x3d8B, endAddr&0x00ff);

	for(int i=0;i<600;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); 
	}

	temp1 = dev->i2c_read(BIT16_BIT8,0x3d81);
	dev->i2c_write(BIT16_BIT8,0x3d81, temp1 | 0x01); // load otp into buffer
	Sleep(10);

	for(int i=0; i<600; i++)
	{
		ReadOtpValue = 0;
		//dev->i2c_read(BIT16_BIT8,addr+i,&ReadOtpValue,1);
		ReadOtpValue = dev->i2c_read(BIT16_BIT8,addr+i);
		ReadDataValue[i] = ReadOtpValue;
	}

	for(int i=0;i<500;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer, recommended use continuous write to accelarate
	}
	//set 0x5000[4] to "1"
	temp1 = dev->i2c_read(BIT16_BIT8,0x5000);
	dev->i2c_write(BIT16_BIT8,0x5000,(temp1 | 0x10));

#endif
	//*******************************Read OTP ***********************************//
REWRITE:
	int temp =	dev->i2c_read(BIT16_BIT8,0x5000);
	dev->i2c_write(BIT16_BIT8,0x5000,(temp & (~0x10)));

	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer, recommended use continuous write to accelarate
	}

	temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x40 | temp);

	temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, ((~0x80) & temp));

	dev->i2c_write(BIT16_BIT8,0x3d88, starAddr>>8); // OTP start address
	dev->i2c_write(BIT16_BIT8,0x3d89, starAddr&0x00ff);
	dev->i2c_write(BIT16_BIT8,0x3d8A, endAddr>>8); // OTP start address
	dev->i2c_write(BIT16_BIT8,0x3d8B, endAddr&0x00ff);
	
	
	for(i = 0; i < len; i++)
	{
		BurnData = 0;
		BurnData = BurnOtpData[i];
		//if(dev->i2c_write(BIT16_BIT8,addr+i,&BurnData,1) != 0)
		dev->i2c_write(BIT16_BIT8,addr+i, BurnData);
	}
	
	temp =	dev->i2c_read(BIT16_BIT8,0x3d80);
	dev->i2c_write(BIT16_BIT8,0x3d80, temp | 0x01);
	Sleep(200);

	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer, recommended use continuous write to accelarate
	}
	//set 0x5000[4] to "1"
	temp = dev->i2c_read(BIT16_BIT8,0x5000);
	dev->i2c_write(BIT16_BIT8,0x5000,(temp | 0x10));
	//*************************************************Compare************************************//
#if 1
	unsigned char ReadOtpValue;
	uint8_t ReadDataValue[200];
	for(i=0;i<200;i++) ReadDataValue[i] = 0;
	temp =	dev->i2c_read(BIT16_BIT8,0x5000);
	dev->i2c_write(BIT16_BIT8,0x5000,(temp& (~0x10)));

	temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x40 | temp);

	temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x80 | temp);

	dev->i2c_write(BIT16_BIT8,0x3d88, starAddr>>8); // OTP start address
	dev->i2c_write(BIT16_BIT8,0x3d89, starAddr&0x00ff);
	dev->i2c_write(BIT16_BIT8,0x3d8A, endAddr>>8); // OTP end address
	dev->i2c_write(BIT16_BIT8,0x3d8B, endAddr&0x00ff);

	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); 
	}

	temp = dev->i2c_read(BIT16_BIT8,0x3d81);
	dev->i2c_write(BIT16_BIT8,0x3d81, temp | 0x01); // load otp into buffer
	Sleep(200);

	for(int i=0; i<len; i++)
	{
		ReadOtpValue = dev->i2c_read(BIT16_BIT8,addr+i);
		ReadDataValue[i] = ReadOtpValue;
	}

	for(i=0; i<len; i++){
		if(ReadDataValue[i] != cData[i]){
			if (retrycount < 2){
				retrycount ++;
				goto REWRITE;
			}
			return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
		}
	}
#endif
	//**************************Compare********************************//
	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer, recommended use continuous write to accelarate
	}
	//set 0x5000[4] to "1"
	temp = dev->i2c_read(BIT16_BIT8,0x5000);
	dev->i2c_write(BIT16_BIT8,0x5000,(temp | 0x10));

	return 0;
}


int OV5695::do_read_otp(int page, int addr, void *data, int len)
{
//	int otp_flag,i;
	unsigned char *cData = (unsigned char *)data;
	int starAddr = addr;
	int endAddr = addr + len -1;

	unsigned char ReadOtpValue;

	int temp =	dev->i2c_read(BIT16_BIT8,0x5000);
	dev->i2c_write(BIT16_BIT8,0x5000,(temp& (~0x10)));

	temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x40 | temp);

	temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x80 | temp);

	dev->i2c_write(BIT16_BIT8,0x3d88, starAddr>>8); // OTP start address
	dev->i2c_write(BIT16_BIT8,0x3d89, starAddr&0x00ff);
	dev->i2c_write(BIT16_BIT8,0x3d8A, endAddr>>8); // OTP end address
	dev->i2c_write(BIT16_BIT8,0x3d8B, endAddr&0x00ff);

	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); 
	}

	temp = dev->i2c_read(BIT16_BIT8,0x3d81);
	dev->i2c_write(BIT16_BIT8,0x3d81, temp | 0x01); // load otp into buffer
	Sleep(50);

	for(int i=0; i<len; i++)
	{
		ReadOtpValue = 0;
		//dev->i2c_read(BIT16_BIT8,addr+i,&ReadOtpValue,1);
		ReadOtpValue = dev->i2c_read(BIT16_BIT8,addr+i);
		cData[i] = ReadOtpValue;
	}

	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); 
	}
	temp = dev->i2c_read(BIT16_BIT8,0x5000);
	dev->i2c_write(BIT16_BIT8,0x5000, (temp | 0x10));

   return 0;
}
//-----------------------------------------------------------------------------
int OV5695::do_get_sid(uint8_t *id)
{
    if (do_read_otp(0, 0x7000, (char*)id, sid_len) < 0)
    {
        return -1;
    }
    return 0;
}

BOOL OV5695::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;

 	WORD wFuseId[8] = {0};
	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x7000 +  i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				0x7000 + i, wFuseId[i]);
			return FALSE;
		}
		// Æ´½ÓFUSE_ID×÷ÎªSensorId
		strSensorId.AppendFormat(_T("%02X"), wFuseId[i]);
	}

	return TRUE;
}

int OV5695::wb_writeback(uint8_t *regs, int len)
{
// 	dev->write_sensor(0x5032, regs, len);
// 
 	int val = dev->read_sensor(0x5000);
 	dev->write_sensor(0x5000, val | 0x08);
/*
	dev->write_sensor(0x5014, regs[0] >> 8);
	dev->write_sensor(0x5015, regs[0] & 0x00ff);

	dev->write_sensor(0x5012, regs[1] >> 8);
	dev->write_sensor(0x5013, regs[1] & 0x00ff);

	dev->write_sensor(0x5000, regs[0] >> 8);
	dev->write_sensor(0x5001, regs[0] & 0x00ff);
*/
	return 0;
}