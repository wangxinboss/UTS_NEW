#include "StdAfx.h"
#include "IMX363.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
IMX363::IMX363()
{
	name = _T("IMX363");
	sensorType = Sensor_IMX363;
    sid_len = 8;

}

//-------------------------------------------------------------------------------------------------
int IMX363::do_prog_otp(int page, int addr, const void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}

int IMX363::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-----------------------------------------------------------------------------
int IMX363::do_get_sid(uint8_t *id)
{
    if (do_read_otp(0x7F, 0x0A22, (char*)id, sid_len) < 0)
    {
        return -1;
    }
    return 0;
}

BOOL IMX363::GetSensorId(__out CString &strSensorId)
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

BOOL IMX363::GetThermal(int *meter)
{
	//char data[2];

	//data[0]= 0x01;
	//dev->i2c_write(BIT16_BIT16, 0x0138, data, 1);

	//Sleep(200);
	*meter = dev->read_sensor(0x13A);

	return TRUE;
}

