#include "StdAfx.h"
#include "S5K4H8YX.h"
#include "CommonFunc.h"
//-------------------------------------------------------------------------------------------------
S5K4H8YX::S5K4H8YX()
{
	name = _T("S5K4H8YX");
	sensorType = Sensor_S5K4H8YX;
    sid_len = 5;
}
//-------------------------------------------------------------------------------------------------
int S5K4H8YX::do_prog_otp(int page, int addr, const void *data, int len)
{
	const char *cData = (const char *)data;
	char pagedata[64];
	char data_write;

	Sleep(10);
	pagedata[0]= page;
	dev->i2c_write(BIT16_BIT8, 0x0A02, pagedata, 1);	//set page#

	for(int i=0;i<64;i++)
		pagedata[i]=0;
	for(int i=0;i<len;i++)
	{
		pagedata[addr-0x0A04+i]=cData[i];
	}
	for(int i=0;i<64;i++)
	{	
		data_write=0;
		data_write=pagedata[i];
		dev->i2c_write(BIT16_BIT8, 0x0A04+i, &data_write, 1);	//Write to buffer	
	}
	pagedata[0]= 0x03;
//	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata , 1);	//Write to NVM
	Sleep(40);
	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00,pagedata, 1);	//disable NVM
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int S5K4H8YX::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	char pagedata[64];

 	Sleep(10);
 	pagedata[0]= page;
	dev->i2c_write(BIT16_BIT8, 0x0A02, pagedata, 1);	//set page#
	pagedata[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata, 1);		//enable read NVM
	for(int i=0;i<len;i++)
	{
		dev->i2c_read(BIT16_BIT8, addr+i, cData+i, 1);	//Read to buffer
	}	
	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00,pagedata, 1);	//disable NVM
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int S5K4H8YX::wb_writeback(uint8_t *regs, int len)
{
	char en = 1;
	dev->i2c_write(BIT16_BIT8, 0x3058, &en, 1);
	dev->i2c_write(BIT16_BIT8, 0x020E, (const char*)regs, len);
	return 0;
}

//-------------------------------------------------------------------------------------------------
BOOL S5K4H8YX::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	char data[2];

	data[0]= 0xD0;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x6028, data, 2);

	data[0]= 0x00;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A02, data, 2);

	data[0]= 0x01;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, data, 2);

	
	// 读取FUSE_ID
	WORD wFuseId[3] = {0};
	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x0A24 + 2*i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				       0x0A24 + i, wFuseId[i]);
			return FALSE;
		}
		// 拼接FUSE_ID作为SensorId
		strSensorId.AppendFormat(_T("%04X"), wFuseId[i]);
	}


	return TRUE;
}

