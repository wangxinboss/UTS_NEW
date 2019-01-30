#include "StdAfx.h"
#include "S5K5E9.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
S5K5E9::S5K5E9()
{
	name = _T("S5K5E9");
	sensorType = Sensor_S5K5E9;
    sid_len = 12;
}
//-------------------------------------------------------------------------------------------------
int S5K5E9::do_prog_otp(int page, int addr, const void *data, int len)
{
    return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-------------------------------------------------------------------------------------------------
int S5K5E9::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-------------------------------------------------------------------------------------------------

int S5K5E9::do_get_sid(uint8_t *id)
{
    return do_read_otp(0, 32, id, sid_len);
}

BOOL S5K5E9::GetSensorId(__out CString &strSensorId)
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

	data[0]= 0x04;
	dev->i2c_write(BIT16_BIT8, 0x0A00, data, 1);

	data[0]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, data, 1);

	return TRUE;
}


int S5K5E9::wb_writeback(uint8_t *regs, int len)
{
// 	dev->write_sensor(0x5032, regs, len);
// 
 	char en = 0x01;
	dev->i2c_write(BIT16_BIT16, 0x3058, &en, 2);
	dev->i2c_write(BIT16_BIT16, 0x020e, (const char*)regs, len);
	return 0;


	return 0;
}