#include "StdAfx.h"
#include "OV5648.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
OV5648::OV5648()
{
	name = _T("OV5648");
	sensorType = Sensor_OV5648;
    sid_len = 5;
}

#define BIT16_BIT8 2
//-------------------------------------------------------------------------------------------------
int OV5648::do_prog_otp(int page, int addr, const void *data, int len)
{
 //  return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
		unsigned char ReadOtpValue;
		//const char *cData = (const char *)data;
	uint8_t *cData = (uint8_t *)data;
	int otp_flag = 0;
	int starAddr = addr;
	int endAddr = addr + len-1;
	int i = 0;
	char ReadOtpData[600];
	char BurnOtpData[600];
	char BurnData;
	unsigned char ReadData;
	int retrycount = 0;
	memset(ReadOtpData, 0, 600);
	memset(BurnOtpData, 0, 600);
	for(i=0; i<len; i++)
	{
		BurnOtpData[i] = cData[i];
	}

	//*******************************Write OTP ***********************************//
REWRITE:
	for ( i=0;i<16;i++)
	{
		dev->i2c_write(BIT16_BIT8,0x3D00+i,0x00);
	}

	dev->i2c_write(BIT16_BIT8,0x3D84,0x40);
	switch(page)
	{
	case 0:
		dev->i2c_write(BIT16_BIT8,0x3D85,0x00);
		dev->i2c_write(BIT16_BIT8,0x3D86,0x0F);
		break;
	case 1:
		dev->i2c_write(BIT16_BIT8,0x3D85,0x10);
		dev->i2c_write(BIT16_BIT8,0x3D86,0x1F);
		break;
	}

	for(i = 0; i < len; i++)
	{
		BurnData = 0;
		BurnData = BurnOtpData[i];
		//if(dev->i2c_write(BIT16_BIT8,addr+i,&BurnData,1) != 0)
		dev->i2c_write(BIT16_BIT8,addr+i, BurnData);
	}
	
	int temp =	dev->i2c_read(BIT16_BIT8,0x3d80);
	dev->i2c_write(BIT16_BIT8,0x3d80, temp | 0x01); //Enable write
	Sleep(200);
	dev->i2c_write(BIT16_BIT8,0x3d80, 0x00);  //disable write

	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer, recommended use continuous write to accelarate
	}
	
	//*************************************************Compare************************************//
#if 1
//	unsigned char ReadOtpValue;
	uint8_t ReadDataValue[200];
	for(i=0;i<200;i++) ReadDataValue[i] = 0;

	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer,
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
			return 0;
		}
	}
#endif
	//**************************Compare********************************//
	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer, recommended use continuous write to accelarate
	}

	return TRUE;
}

int OV5648::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	int starAddr = addr;
	int endAddr = addr + len -1;
	int i=0;
	unsigned char ReadOtpValue;
	for ( i=0;i<16;i++)
	{
		dev->i2c_write(BIT16_BIT8,0x3D00+i,0x00);
	}
	dev->i2c_write(BIT16_BIT8,0x3d84, 0xC0);

	switch(page)
	{
	case 0:
		dev->i2c_write(BIT16_BIT8,0x3D85,0x00);
		dev->i2c_write(BIT16_BIT8,0x3D86,0x0F);
		break;
	case 1:
		dev->i2c_write(BIT16_BIT8,0x3D85,0x10);
		dev->i2c_write(BIT16_BIT8,0x3D86,0x1F);
		break;
	}

	dev->i2c_write(BIT16_BIT8,0x3D81,0x01);
	Sleep(100);
	dev->i2c_write(BIT16_BIT8,0x3D81,0x00);

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
 
	return TRUE;
 //  return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int OV5648::do_get_sid(uint8_t *id)
{
    if (!do_read_otp(0, 0x3D00, (char*)id, sid_len))
    {
        return -1;
    }
    return 0;
}

BOOL OV5648::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;

	uint8_t wFuseId[5] = {0};

	do_read_otp(0,0x3D00,wFuseId,5);

	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%02x]"),0x3D00+i, wFuseId[i]);	
		// Æ´½ÓFUSE_ID×÷ÎªSensorId
		strSensorId.AppendFormat(_T("%02X"), wFuseId[i]);
	 }

	return TRUE;
}

int OV5648::wb_writeback(uint8_t *regs, int len)
{
  	
	dev->write_sensor(0x5186, regs, len);

	return 0;
}