#include "StdAfx.h"
#include "S5K3L6.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
S5K3L6::S5K3L6()
{
	name = _T("S5K3L6");
	sensorType = Sensor_S5K3L6;
    sid_len = 12;
}
//-------------------------------------------------------------------------------------------------
int S5K3L6::do_prog_otp(int page, int addr, const void *data, int len)
{
    return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-------------------------------------------------------------------------------------------------
int S5K3L6::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-------------------------------------------------------------------------------------------------

int S5K3L6::do_get_sid(uint8_t *id)
{
    return do_read_otp(0, 32, id, sid_len);
}

BOOL S5K3L6::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	char data[2];

	data[0]= 0x40;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x6028, data, 2);
	
	data[0]= 0x00;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A02, data, 2);

	data[0]= 0x01;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A00, data, 2);
	 
	// 读取FUSE_ID
	WORD wFuseId[3] = {0};
	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x0A04 + 2 * i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				0x0A04 +  2 * i, wFuseId[i]);
			return FALSE;
		}
		// 拼接FUSE_ID作为SensorId
		strSensorId.AppendFormat(_T("%04X"), wFuseId[i]);
	}

	data[0]= 0x00;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A00, data, 2);
	return TRUE;
}


int S5K3L6::wb_writeback(uint8_t *regs, int len)
{
 	char en = 2;
	dev->i2c_write(BIT16_BIT16, 0x3058, &en, 2);
	dev->i2c_write(BIT16_BIT16, 0x0100, (const char*)regs, len);
	return 0;
}