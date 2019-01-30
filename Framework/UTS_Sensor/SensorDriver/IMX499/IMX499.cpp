#include "StdAfx.h"
#include "IMX499.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
IMX499::IMX499()
{
	name = _T("IMX499");
	sensorType = Sensor_IMX499;
    sid_len = 11;

}

//-------------------------------------------------------------------------------------------------
int IMX499::do_prog_otp(int page, int addr, const void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}

int IMX499::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-----------------------------------------------------------------------------
int IMX499::do_get_sid(uint8_t *id)
{
    if (do_read_otp(0x27, 0x0A20, (char*)id, sid_len) < 0)
    {
        return -1;
    }
    return 0;
}

BOOL IMX499::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	char data[2];

	data[0]= 0x27;
	dev->i2c_write(BIT16_BIT16, 0x0A02, data, 1);

	data[0]= 0x01;
	dev->i2c_write(BIT16_BIT16, 0x0A00, data, 1);
	 
	// 读取FUSE_ID
	WORD wFuseId[11] = {0};
	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x0A20 + i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				0x0A21 + i, wFuseId[i]);
			return FALSE;
		}
		// 拼接FUSE_ID作为SensorId
		strSensorId.AppendFormat(_T("%02X"), wFuseId[i]);
	}
	return TRUE;
}

int IMX499::wb_writeback(uint8_t *regs, int len)
{
// 	dev->write_sensor(0x5032, regs, len);
// 
 	dev->write_sensor(0x3ff9, 0x00);

	dev->write_sensor(0x020e, regs, len);  //GRBG


	return 0;
}