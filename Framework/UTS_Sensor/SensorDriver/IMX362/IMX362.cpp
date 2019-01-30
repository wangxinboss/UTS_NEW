#include "StdAfx.h"
#include "IMX362.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
IMX362::IMX362()
{
	name = _T("IMX362");
	sensorType = Sensor_IMX362;
    sid_len = 8;

}

//-------------------------------------------------------------------------------------------------
int IMX362::do_prog_otp(int page, int addr, const void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}

int IMX362::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-----------------------------------------------------------------------------
int IMX362::do_get_sid(uint8_t *id)
{
    if (do_read_otp(0x7F, 0x0A22, (char*)id, sid_len) < 0)
    {
        return -1;
    }
    return 0;
}

BOOL IMX362::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	char data[2];

	data[0]= 0x7F;
	dev->i2c_write(BIT16_BIT16, 0x0A02, data, 1);

	data[0]= 0x01;
	dev->i2c_write(BIT16_BIT16, 0x0A00, data, 1);
	 
	// 读取FUSE_ID
	WORD wFuseId[8] = {0};
	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x0A22 + i, wFuseId[i]))
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

int IMX362::wb_writeback(uint8_t *regs, int len)
{
// 	dev->write_sensor(0x5032, regs, len);
// 
 	int val = dev->read_sensor(0x5000);
 	dev->write_sensor(0x5000, val | 0x08);

	dev->write_sensor(0x5100, regs, len);


	return 0;
}