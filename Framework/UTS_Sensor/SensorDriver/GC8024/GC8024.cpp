#include "StdAfx.h"
#include "GC8024.h"
#include "CommonFunc.h"
//-------------------------------------------------------------------------------------------------
GC8024::GC8024()
{
	name = _T("GC8024");
	sensorType = Sensor_GC8024;
    sid_len = 5;
}
//-------------------------------------------------------------------------------------------------
int GC8024::do_prog_otp(int page, int addr, const void *data, int len)
{
	const char *cData = (const char *)data;

	dev->write_sensor(0xf7,0x95);
	dev->write_sensor(0xf9,0x00);
	dev->write_sensor(0xfc,0xce);

	dev->write_sensor(0xfa,0x47);   //0x49会花屏
	dev->write_sensor(0xfe,0x00);

	dev->write_sensor(0xd4,0x80);

	page=(page<<2)|0x80;
	//switch(page)
	//{
	//case 0:
	//	page= page&0xfb;
	//	break;
	//case 1:
	//	page= page|0x04;
	//	break;
	//}
	dev->write_sensor(0xd4,page);
	for(int i= 0; i< len; i++)
	{
		dev->write_sensor(0xd5,addr+i);
		dev->write_sensor(0xd6,cData[i]);
		Sleep(10);
		dev->write_sensor(0xf3,0x40);
	}

	return TRUE;
}
//-------------------------------------------------------------------------------------------------
int GC8024::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;

	dev->write_sensor(0xf7,0x95);
	dev->write_sensor(0xf9,0x00);
	dev->write_sensor(0xfc,0xce);

	dev->write_sensor(0xfa,0x47);   //0x49会花屏
	dev->write_sensor(0xfe,0x00);

	dev->write_sensor(0xd4,0x80);

	page=(page<<2)|0x80;

	dev->write_sensor(0xd4,page);

	for(int i=0; i<len; i++)
	{
		dev->write_sensor(0xd5,addr+i);
		Sleep(10);
		dev->write_sensor(0xf3,0x20);
		cData[i]= dev->read_sensor(0xd7);
	}

	return TRUE;
}
//-------------------------------------------------------------------------------------------------
int GC8024::wb_writeback(uint8_t *regs, int len)
{
	char en = 0x00;

//	dev->i2c_write(BIT8_BIT8, 0xfe, &en, 1);
//	dev->i2c_write(BIT8_BIT8, 0xb8, (const char*)regs, len);

	return 0;
}

//-------------------------------------------------------------------------------------------------
BOOL GC8024::GetSensorId(__out CString &strSensorId)
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

