#include "StdAfx.h"
#include "OV5675.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
OV5675::OV5675()
{
	name = _T("OV5675");
	sensorType = Sensor_OV5675;
    sid_len = 16;
}

#define BIT16_BIT8 2
//-------------------------------------------------------------------------------------------------
int OV5675::do_prog_otp(int page, int addr, const void *data, int len)
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
	int retrycount = 0;
	memset(ReadOtpData, 0, 600);
	memset(BurnOtpData, 0, 600);
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
	int temp =	dev->i2c_read(BIT16_BIT8,0x5001);
	//dev->i2c_write(BIT16_BIT8,0x5001,(temp & (~0x08)));
	dev->i2c_write(BIT16_BIT8,0x5001,0x02);
	dev->i2c_write(BIT16_BIT8,0x5000,0x77);

	for(int i=0;i<len;i++) 
	{
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer, recommended use continuous write to accelarate
	}

	//temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	//dev->i2c_write(BIT16_BIT8,0x3d84, 0x40 | temp);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x40 );
	//temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	//dev->i2c_write(BIT16_BIT8,0x3d84, ((~0x80) & temp));

	dev->i2c_write(BIT16_BIT8,0x3d85, 0x00); // update by ysz on 2018/01/12 

	dev->i2c_write(BIT16_BIT8,0x3d88, (starAddr>>8)&0xff); // OTP start address
	dev->i2c_write(BIT16_BIT8,0x3d89, starAddr&0xff);
	dev->i2c_write(BIT16_BIT8,0x3d8A, (endAddr>>8)&0xff); // OTP start address
	dev->i2c_write(BIT16_BIT8,0x3d8B, endAddr&0xff);
	
	dev->i2c_write(BIT16_BIT8,0x0100, 0x01);
	Sleep(20);

	for(i = 0; i < len; i++)
	{
		BurnData = 0;
		BurnData = BurnOtpData[i];
		//if(dev->i2c_write(BIT16_BIT8,addr+i,&BurnData,1) != 0)
		dev->i2c_write(BIT16_BIT8,addr+i, BurnData);
	}
	
	//temp =	dev->i2c_read(BIT16_BIT8,0x3d80);
	//dev->i2c_write(BIT16_BIT8,0x3d80, temp | 0x01);
	dev->i2c_write(BIT16_BIT8,0x3d80, 0x01);
	Sleep(200);
	dev->i2c_write(BIT16_BIT8,0x3d80, 0x00);
	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer, recommended use continuous write to accelarate
	}
	//set 0x5000[4] to "1"
	//temp = dev->i2c_read(BIT16_BIT8,0x5001);
	//dev->i2c_write(BIT16_BIT8,0x5001,(temp | 0x08));
	dev->i2c_write(BIT16_BIT8,0x5001,0x0a);
	//*************************************************Compare************************************//
#if 1
//	unsigned char ReadOtpValue;
	uint8_t ReadDataValue[600];
	for(i=0;i<200;i++) ReadDataValue[i] = 0;
	//temp =	dev->i2c_read(BIT16_BIT8,0x5001);
	dev->i2c_write(BIT16_BIT8,0x5001,0x02);

	temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x40 | temp);

	/*temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x80 | temp);*/

	dev->i2c_write(BIT16_BIT8,0x3d88, (starAddr>>8)&0xff); // OTP start address
	dev->i2c_write(BIT16_BIT8,0x3d89, starAddr&0xff);
	dev->i2c_write(BIT16_BIT8,0x3d8A, (endAddr>>8)&0xff); // OTP end address
	dev->i2c_write(BIT16_BIT8,0x3d8B, endAddr&0xff);
	dev->i2c_write(BIT16_BIT8,0x3d85, 0x06); // update by ysz on 2018/01/12 
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
			return 0;
		}
	}
#endif
	//**************************Compare********************************//
	for(int i=0;i<len;i++) {
		dev->i2c_write(BIT16_BIT8,addr+i,0); // clear OTP buffer, recommended use continuous write to accelarate
	}
	//set 0x5000[4] to "1"
	//temp = dev->i2c_read(BIT16_BIT8,0x5001);
	dev->i2c_write(BIT16_BIT8,0x5001,0x0A);

	return 1;
/*	int i; 

	int endAddr=addr+len-1;
	WORD StartAddrH=(addr>>8)&0xff;
	WORD StartAddrL=(addr&0xff);
	WORD EndAddrH=(endAddr>>8)&0xff;
	WORD EndAddrL=endAddr&0xff;

	WORD temphehe[2]={0};
	if (!dev->i2c_read(0x5001,temphehe[0]))
	{
		printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),0x5001, temphehe[0]);
		return FALSE;	
	}
	if (!dev->i2c_write(2,0x5001,temphehe[0]&0xF7))   //bit[3] to 0
	{
		printk(_T("RegWrite Error. [Reg = 0x%x][Data = 0x%x]"),0x5001, temphehe[0]&0xF7);
		return FALSE;	
	}

	for (i=addr; i<=endAddr; i++) 
	{
		dev->i2c_write(2,i, 0x00); 
	} 

	dev->i2c_write(2,0x3D84,0x40);      //进入OTP写入模式
	Sleep(50);	

	dev->i2c_write(2,0x3d88,StartAddrH);    //Start addr
	dev->i2c_write(2,0x3d89,StartAddrL);
	dev->i2c_write(2,0x3d8A,EndAddrH);      //end addr
	dev->i2c_write(2,0x3d8B,EndAddrL);

	uint8_t *cdata=(uint8_t *)data;
	for(int i=0;i<len;i++){
	if(!dev->i2c_write(2,i+0x70,cdata[i]));
	{
		printk(_T("RegWrite Error. [Reg = 0x%04x]"),i+0x7010);
		return FALSE;
	}
	}
	dev->i2c_write(2,0x3d80,0x01);
	Sleep(50);
	
	for (i=addr; i<=endAddr; i++)   //clear otp buffer
	{
		dev->i2c_write(2,i, 0x00); 
	} 
	Sleep(10);
	dev->i2c_write(2,0x3D84,temphehe[0]);
	Sleep(5);*/
	return TRUE;
}

int OV5675::do_read_otp(int page, int addr, void *data, int len)
{
		unsigned char *cData = (unsigned char *)data;
	int starAddr = addr;
	int endAddr = addr + len -1;

	unsigned char ReadOtpValue;

	int temp =	dev->i2c_read(BIT16_BIT8,0x5001);
	dev->i2c_write(BIT16_BIT8,0x5001,(temp& (~0x08)));

	temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x40 | temp);

	/*temp =	dev->i2c_read(BIT16_BIT8,0x3d84);
	dev->i2c_write(BIT16_BIT8,0x3d84, 0x80 | temp);*/

	dev->i2c_write(BIT16_BIT8,0x3d88, (starAddr>>8)&0xff); // OTP start address
	dev->i2c_write(BIT16_BIT8,0x3d89, starAddr&0xff);
	dev->i2c_write(BIT16_BIT8,0x3d8A, (endAddr>>8)&0xff); // OTP end address
	dev->i2c_write(BIT16_BIT8,0x3d8B, endAddr&0xff);

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
	temp = dev->i2c_read(BIT16_BIT8,0x5001);
	dev->i2c_write(BIT16_BIT8,0x5001, (temp | 0x08));

 //  return 0;
	//int i; 

	//int endAddr=addr+len-1;
	//WORD StartAddrH=(addr>>8)&0xff;
	//WORD StartAddrL=(addr&0xff);
	//WORD EndAddrH=(endAddr>>8)&0xff;
	//WORD EndAddrL=endAddr&0xff;

	//WORD temphehe[2]={0};
	//if (!dev->i2c_read(0x5001,temphehe[0]))
	//{
	//	printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),0x5001, temphehe[0]);
	//	return FALSE;	
	//}
	//if (!dev->i2c_write(2,0x5001,temphehe[0]&0xF7))   //bit[3] to 0
	//{
	//	printk(_T("RegWrite Error. [Reg = 0x%x][Data = 0x%x]"),0x5001, temphehe[0]&0x08);
	//	return FALSE;	
	//}

	//for (i=addr; i<=endAddr; i++) 
	//{
	//	dev->i2c_write(2,i, 0x00); 
	//} 

	//dev->i2c_write(2,0x3D84,0xc0);      //进入OTP写入模式
	//Sleep(50);	


	//dev->i2c_write(2,0x3d88,StartAddrH);    //Start addr
	//dev->i2c_write(2,0x3d89,StartAddrL);
	//dev->i2c_write(2,0x3d8A,EndAddrH);      //end addr
	//dev->i2c_write(2,0x3d8B,EndAddrL);

	//dev->i2c_write(2,0x03d81,0x01);  //load otp data
	//Sleep(50);


	//uint8_t *cdata=(uint8_t *)data;
	//for(int i=0;i<507;i++)
	//{
	//if(!dev->i2c_read(i+0x7010,cdata[i]))
	//{
	//	printk(_T("RegRead Error. [Reg = 0x%04x]"),i+0x7010);
	//}
	//}
	//Sleep(10);

	//for (i=addr; i<=endAddr; i++)   //clear otp buffer
	//{
	//	dev->i2c_write(2,i, 0x00); 
	//} 
	//Sleep(10);
	//dev->i2c_write(2,0x5001,0x40);
	//Sleep(5);
	return TRUE;
 //  return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int OV5675::do_get_sid(uint8_t *id)
{
    if (!do_read_otp(0, 0x7000, (char*)id, sid_len))
    {
        return -1;
    }
    return 0;
}

BOOL OV5675::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;

	WORD wFuseId[16] = {0};

	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x7000 +  i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%02x]"),
				0x7000+i, wFuseId[i]);
			return FALSE;
		}

		// 拼接FUSE_ID作为SensorId
		strSensorId.AppendFormat(_T("%02X"), wFuseId[i]);
	 }

	return TRUE;
}

int OV5675::wb_writeback(uint8_t *regs, int len)
{
  	int val = dev->read_sensor(0x5000);
  	dev->write_sensor(0x5000, val | 0x08);

	dev->write_sensor(0x5019, regs, len);

	return 0;
}