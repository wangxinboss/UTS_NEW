#include "StdAfx.h"
#include "Hi546.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
Hi546::Hi546()
{
	name = _T("Hi546");
	sensorType = Sensor_Hi546;
    sid_len = 9;
}

#define BIT16_BIT8 2
#define BIT16_BIT16 3
//-------------------------------------------------------------------------------------------------
int Hi546::do_prog_otp(int page, int addr, const void *data, int len)
{
	const char *cData = (const char *)data;

	char data_write;	
	char addr_write[2];
	unsigned char data_read;

	char pagedata[13]={0x01,0x00,0x00,0x01,0x09,0x01,0x07,0x10,0x01,0x02,0x00,0x00,0x01};


	addr_write[0]=(char)((addr>>8)&0xff);
	addr_write[1]=(char)(addr&0xff);

	dev->i2c_write(BIT16_BIT8, 0x0A02, &pagedata[0], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[1], 1);	
	Sleep(50);
	dev->i2c_write(BIT16_BIT8, 0x0F02, &pagedata[2], 1);	
	dev->i2c_write(BIT16_BIT8, 0x011A, &pagedata[3], 1);	
	dev->i2c_write(BIT16_BIT8, 0x011B, &pagedata[4], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0D04, &pagedata[5], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0D00, &pagedata[6], 1);	
	dev->i2c_write(BIT16_BIT8, 0x003E, &pagedata[7], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[8], 1);	

	dev->i2c_write(BIT16_BIT8, 0x010A, addr_write,   2);	
	dev->i2c_write(BIT16_BIT8, 0x0102, &pagedata[9], 1);	
	Sleep(50);

	for(int i=0;i<len;i++)
	{	
		data_write=cData[i];
		dev->i2c_write(BIT16_BIT8, 0x0106, &data_write, 1);	//Write to buffer	
		Sleep(4);
		dev->i2c_read(BIT16_BIT8, 0x0108, &data_read, 1);
		if (data_read!=cData[i]) return FALSE;
		
	}

	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[10], 1);	
	dev->i2c_write(BIT16_BIT8, 0x003E, &pagedata[11], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[12], 1);	

	return TRUE;
}

int Hi546::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;

	unsigned char data_read;	
	char addr_read[2];

	char pagedata[13]={0x01,0x00,0x00,0x01,0x09,0x01,0x07,0x10,0x01,0x01,0x00,0x00,0x01};


	addr_read[0]=(char)((addr>>8)&0xff);
	addr_read[1]=(char)(addr&0xff);

	dev->i2c_write(BIT16_BIT8, 0x0A02, &pagedata[0], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[1], 1);	
	Sleep(50);
	dev->i2c_write(BIT16_BIT8, 0x0F02, &pagedata[2], 1);	
	dev->i2c_write(BIT16_BIT8, 0x011A, &pagedata[3], 1);	
	dev->i2c_write(BIT16_BIT8, 0x011B, &pagedata[4], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0D04, &pagedata[5], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0D00, &pagedata[6], 1);	
	dev->i2c_write(BIT16_BIT8, 0x003E, &pagedata[7], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[8], 1);	

	dev->i2c_write(BIT16_BIT8, 0x010A, addr_read,  2);	
	dev->i2c_write(BIT16_BIT8, 0x0102, &pagedata[9], 1);	
	Sleep(50);

	for(int i=0;i<len;i++)
	{	
		dev->i2c_read(BIT16_BIT8, 0x0108, &data_read, 1);
		cData[i]=data_read;
		Sleep(4);
	}

	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[10], 1);	
	dev->i2c_write(BIT16_BIT8, 0x003E, &pagedata[11], 1);	
	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[12], 1);	

	return TRUE;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int Hi546::do_get_sid(uint8_t *id)
{
    if (!do_read_otp(0, 0x0001, (char*)id, sid_len))
    {
        return -1;
    }
    return 0;
}

BOOL Hi546::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;

	char wFuseId[9] = {0};

	if (!do_read_otp(0, 0x0001, wFuseId, ARRAY_SIZE(wFuseId)))
	{
		return FALSE;
	}

	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{

		printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%02x]"),
				0x0001+i, wFuseId[i]);
		// Æ´½ÓFUSE_ID×÷ÎªSensorId
		strSensorId.AppendFormat(_T("%02X"), wFuseId[i]);
	 }

	return TRUE;
}

int Hi546::wb_writeback(uint8_t *regs, int len)
{
  //	int val = dev->read_sensor(0x0A05);
	char data_write;
	unsigned char data_read;
	char * regs_wb= (char*)regs;

	dev->i2c_read(BIT16_BIT8, 0x0A05, &data_read, 1);
	data_write = (data_read|0x08);
  	dev->i2c_write(BIT16_BIT8,0x0A05,&data_write ,1);

	dev->i2c_write(BIT16_BIT8,0x0078, regs_wb, len);

	return 0;
}