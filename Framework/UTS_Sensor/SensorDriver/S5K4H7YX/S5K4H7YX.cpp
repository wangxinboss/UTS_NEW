#include "StdAfx.h"
#include "S5K4H7YX.h"
#include "CommonFunc.h"
//-------------------------------------------------------------------------------------------------
S5K4H7YX::S5K4H7YX()
{
	name = _T("S5K4H7YX");
	sensorType = Sensor_S5K4H7YX;
    sid_len = 5;
}
//-------------------------------------------------------------------------------------------------
int S5K4H7YX::do_prog_otp(int page, int addr, const void *data, int len)
{
	const char *cData = (const char *)data;
	char pagedata[64];
	char data_write;
	int nWriteCount = 5;
	unsigned char cReadData = 0;
	//PLL Setting
	pagedata[0] = 0x18;
	dev->i2c_write(BIT16_BIT8, 0x0136, pagedata, 1);
	pagedata[0] = 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0137, pagedata, 1);
	pagedata[0] = 0x06;
	dev->i2c_write(BIT16_BIT8, 0x0305, pagedata, 1);
	pagedata[0] = 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0306, pagedata, 1);
	pagedata[0] = 0x8C;
	dev->i2c_write(BIT16_BIT8, 0x0307, pagedata, 1);
	pagedata[0] = 0x06;
	dev->i2c_write(BIT16_BIT8, 0x030d, pagedata, 1);
	pagedata[0] = 0x00;
	dev->i2c_write(BIT16_BIT8, 0x030e, pagedata, 1);
	pagedata[0] = 0xAF;
	dev->i2c_write(BIT16_BIT8, 0x030f, pagedata, 1);
	pagedata[0] = 0x04;
	dev->i2c_write(BIT16_BIT8, 0x0301, pagedata, 1);
	//PLL Setting Colse

	//Streaming ON
	pagedata[0] = 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0100, pagedata, 1);
	
	//Delay 50ms
	Sleep(50);

	pagedata[0] = 0x00;
	dev->i2c_write(BIT16_BIT8, 0x3b3f, pagedata, 1);
	pagedata[0] = page;
	dev->i2c_write(BIT16_BIT8, 0x0a02, pagedata, 1);
	pagedata[0] = 0x03;
	dev->i2c_write(BIT16_BIT8, 0x0a00, pagedata, 1);

	while(nWriteCount > 0)
	{
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

		dev->i2c_read(BIT16_BIT8, 0x0a01, &cReadData, 1);
		if((cReadData & 0x01) == 0x01) 
			goto out;
		else
		{
			if((cReadData & 0x08) == 0x01)
				return -1;
			else
				nWriteCount--;
		}	
	}
	Sleep(100);
out:
	pagedata[0]= 0x03;
//	dev->i2c_write(BIT16_BIT8, 0x0A00, pagedata , 1);	//Write to NVM
	Sleep(40);
	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00,pagedata, 1);	//disable NVM
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int S5K4H7YX::do_read_otp(int page, int addr, void *data, int len)
{
	//MA84J Read Completed wait for verification // 2018-5-29
	unsigned char *cData = (unsigned char *)data;
	char pagedata[64];
	unsigned char cReadData = 0;

	//PLL Setting
	pagedata[0] = 0x18;
	dev->i2c_write(BIT16_BIT8, 0x0136, pagedata, 1);
	pagedata[0] = 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0137, pagedata, 1);
	pagedata[0] = 0x06;
	dev->i2c_write(BIT16_BIT8, 0x0305, pagedata, 1);
	pagedata[0] = 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0306, pagedata, 1);
	pagedata[0] = 0x8C;
	dev->i2c_write(BIT16_BIT8, 0x0307, pagedata, 1);
	pagedata[0] = 0x06;
	dev->i2c_write(BIT16_BIT8, 0x030d, pagedata, 1);
	pagedata[0] = 0x00;
	dev->i2c_write(BIT16_BIT8, 0x030e, pagedata, 1);
	pagedata[0] = 0xAF;
	dev->i2c_write(BIT16_BIT8, 0x030f, pagedata, 1);
	pagedata[0] = 0x04;
	dev->i2c_write(BIT16_BIT8, 0x0301, pagedata, 1);
	//PLL Setting Colse

	//Streaming ON
	pagedata[0] = 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0100, pagedata, 1);

	//Delay 50ms
	Sleep(50);

	//Write OTP Page
	pagedata[0] = page;
	dev->i2c_write(BIT16_BIT8, 0x0a02, pagedata, 1);

	//Write Read CMD    Write 0a00 01 //Read Command
	pagedata[0] = 0x01;
	dev->i2c_write(BIT16_BIT8, 0x0a00, pagedata, 1);
DelayTime:
	//Delay 50ms
	Sleep(50);

	
	dev->i2c_read(BIT16_BIT8, 0x0a01, &cReadData, 1);
	if((cReadData & 0x01) != 0x01) 
		goto DelayTime;

	//Read Data From data register
	for(int i=0;i<len;i++)
	{
		dev->i2c_read(BIT16_BIT8, addr+i, cData+i, 1);	//Read to buffer
	}

	//Make Initial State
	pagedata[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00,pagedata, 1);	//disable NVM
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int S5K4H7YX::wb_writeback(uint8_t *regs, int len)
{
	char en = 1;
	dev->i2c_write(BIT16_BIT8, 0x3058, &en, 1);
	dev->i2c_write(BIT16_BIT8, 0x020E, (const char*)regs, len);
	return 0;
}

//-------------------------------------------------------------------------------------------------
int S5K4H7YX::do_get_sid(uint8_t *id)
{
    if (do_read_otp(0, 0x0a04, (char*)id, sid_len) < 0)
    {
        return -1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
BOOL S5K4H7YX::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	dev->i2c_write(BIT16_BIT8,0x0100,0x01);
	Sleep(50);
	dev->i2c_write(BIT16_BIT8,0x0A02,0x00);
	dev->i2c_write(BIT16_BIT8,0x0A00,0x01);

	
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
	dev->i2c_write(BIT16_BIT8, 0x0A00, 0x0a00);

	return TRUE;
}

