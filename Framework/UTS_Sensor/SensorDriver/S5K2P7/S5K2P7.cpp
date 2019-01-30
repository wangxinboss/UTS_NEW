#include "StdAfx.h"
#include "S5K2P7.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
S5K2P7::S5K2P7()
{
	name = _T("S5K2P7");
	sensorType = Sensor_S5K2P7;
    sid_len = 5;

}

//-------------------------------------------------------------------------------------------------
int S5K2P7::do_prog_otp(int page, int addr, const void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}

int S5K2P7::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-----------------------------------------------------------------------------
int S5K2P7::do_get_sid(uint8_t *id)
{
    if (do_read_otp(0, 0x0A21, (char*)id, sid_len) < 0)
    {
        return -1;
    }
    return 0;
}

BOOL S5K2P7::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	char data[2];
	
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

int S5K2P7::wb_writeback(uint8_t *regs, int len)
{
	char data[2];

	data[0]= 0x01;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x3058, data,2);

	dev->i2c_write(BIT16_BIT16, 0x020E, (const char*)regs, len);
	return 0;


	return 0;
}