#include "StdAfx.h"
#include "S5K5E2YA.h"
#include "CommonFunc.h"
//-------------------------------------------------------------------------------------------------
S5K5E2YA::S5K5E2YA()
{
	name = _T("S5K5E2YA");
	sensorType = Sensor_S5K5E2YA;
    sid_len = 5;
}
//-------------------------------------------------------------------------------------------------
int S5K5E2YA::do_prog_otp(int page, int addr, const void *data, int len)
{
	const char *cData = (const char *)data;
	char databuf[2];
	
	//test mode 
	databuf[0]= 0x68;
	dev->i2c_write(BIT16_BIT8, 0x3b42, databuf, 1);	//test mode(charge pump)
	databuf[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x3b41, databuf, 1);	//test mode enable

	databuf[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x3b40, databuf, 1);	//write disable off
	databuf[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x3b45, databuf, 1);	//for single write mode no limit mode

	databuf[0]= 0x04;
	dev->i2c_write(BIT16_BIT8, 0x0A00, databuf, 1);	//make initial state
	databuf[0]= 0x03;
	dev->i2c_write(BIT16_BIT8, 0x0A00, databuf, 1);	//set write mode of NVM controller interface1

	databuf[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x3b42, databuf, 1);	//test mode(charge pump)

	databuf[0]= page;
	dev->i2c_write(BIT16_BIT8, 0x0A02, databuf, 1);	//set page#
	databuf[0]= 0x03;
	dev->i2c_write(BIT16_BIT8, 0x0A00, databuf, 1);	//set write mode of NVM controller interface1
	
	for(int i=0;i< len;i++)
	{	
		databuf[0] = cData[i];
		dev->i2c_write(BIT16_BIT8, 0x0A04+i, databuf, 1);	//Write to buffer	
	}

	//Page Write
	//dev->i2c_write(BIT16_BIT8, 0x0A04, cData, len);

	Sleep(100);

	databuf[0]= 0x04;
	dev->i2c_write(BIT16_BIT8, 0x0A00, databuf, 1);	//make initial state
	databuf[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, databuf, 1);	//disable NVM controller
	databuf[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x3b40, databuf, 1);	//write disable on
	
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int S5K5E2YA::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	char pagedata[2];
	char databuf[2];

	databuf[0]= 0x04;
	dev->i2c_write(BIT16_BIT8, 0x0A00, databuf, 1);	//make initial state

	pagedata[0]= page;
	dev->i2c_write(BIT16_BIT8, 0x0A02, pagedata, 1);	//set page#
	databuf[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0A00, databuf, 1);	//set read mode of NVM controller interface1

	for(int i=0;i<len;i++)
	{
		dev->i2c_read(BIT16_BIT8, 0x0A04 + i, cData+i, 1);	//Read to buffer
	}	

	//Page read
	//dev->i2c_read(BIT16_BIT8, 0x0A04 , cData, 64);	//Read to buffer

	databuf[0]= 0x04;
	dev->i2c_write(BIT16_BIT8, 0x0A00, databuf, 1);	//make initial state
	databuf[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, databuf, 1);	//disable NVM controller 

   return SET_ERROR(SENSORDRIVER_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
int S5K5E2YA::do_get_sid(uint8_t *id)
{
    return do_read_otp(0, 32, id, sid_len);
}
//-------------------------------------------------------------------------------------------------
BOOL S5K5E2YA::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	char data[2];

	data[0]= 0x04;
	dev->i2c_write(BIT16_BIT8, 0x0A00, data, 1);

	data[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A02, data, 1);

	data[0]= 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0A00, data, 1);

	// 读取FUSE_ID
	WORD wFuseId[8] = {0};
	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x0A04 + i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				0x0A04 + i, wFuseId[i]);
			return FALSE;
		}
		// 拼接FUSE_ID作为SensorId
		strSensorId.AppendFormat(_T("%02X"), wFuseId[i]);
	}


	return TRUE;
}

//-------------------------------------------------------------------------------------------------
int S5K5E2YA::wb_writeback(uint8_t *regs, int len)
{
	char en = 1;
	dev->i2c_write(BIT16_BIT8, 0x020E, (const char*)regs, len);
	return 0;
}