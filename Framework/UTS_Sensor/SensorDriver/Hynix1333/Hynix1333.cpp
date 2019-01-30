#include "StdAfx.h"
#include "Hynix1333.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
Hynix1333::Hynix1333()
{
	name = _T("Hynix1333");
	sensorType = Sensor_Hynix1333;
    sid_len = 16;

}

//-------------------------------------------------------------------------------------------------
int Hynix1333::do_prog_otp(int page, int addr, const void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}

int Hynix1333::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-----------------------------------------------------------------------------

int Hynix1333::do_get_sid(uint8_t *id)
{
	/*
    if (do_read_otp(0, 0x7000, (char*)id, sid_len) < 0)
    {
        return -1;
    }*/
    return 0;
}

BOOL Hynix1333::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	char data[2];

	data[0] = 0x01;
	data[1] = 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0100, data, 2);

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
		if (!dev->read_sensor(0x0A24 + 2 * i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				0x0A24 +  2 * i, wFuseId[i]);
			return FALSE;
		}
		// 拼接FUSE_ID作为SensorId
		strSensorId.AppendFormat(_T("%04X"), wFuseId[i]);
	}

	data[0] = 0x00;
	data[1] = 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0a00, data, 2);


	return TRUE;
}

int Hynix1333::wb_writeback(uint8_t *regs, int len)
{
// 	dev->write_sensor(0x5032, regs, len);
// 
 	int val = dev->read_sensor(0x5000);
 	dev->write_sensor(0x5000, val | 0x08);

	dev->write_sensor(0x5100, regs, len);


	return 0;
}