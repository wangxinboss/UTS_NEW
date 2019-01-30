#include "StdAfx.h"
#include "S5K3P9SX.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
S5K3P9SX::S5K3P9SX()
{
	name = _T("S5K3P9SX");
	sensorType = Sensor_S5K3P9SX;
    sid_len = 12;
}
//-------------------------------------------------------------------------------------------------
int S5K3P9SX::do_prog_otp(int page, int addr, const void *data, int len)
{
    return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
int S5K3P9SX::do_read_otp(int page, int addr, void *data, int len)
{
    return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}

//-------------------------------------------------------------------------------------------------
int S5K3P9SX::wb_writeback(uint8_t *regs, int len)
{
	//!! 2018.07.27 Chi-jen.Liao 依据宗强伟的Code
	char en = 1;
	dev->i2c_write(BIT16_BIT8, 0x020D, &en, 1);

    dev->i2c_write(BIT16_BIT8, 0x020E, (const char*)regs, len);
	/* char en = 1;
	dev->i2c_write(BIT16_BIT8, 0x305C, &en, 1);*/

    return 0;
}
//-------------------------------------------------------------------------------------------------
int S5K3P9SX::do_get_sid(uint8_t *id)
{
    return do_read_otp(0, 32, id, sid_len);
}
//-------------------------------------------------------------------------------------------------
BOOL S5K3P9SX::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// ¶ÁÈ¡¶Á±£»¤Êý¾Ý
	char data[2];

	data[0]= 0x40;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x6028, data, 2);

	data[0]= 0x00;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A02, data, 2);

	data[0]= 0x01;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, data, 2);


	// ¶ÁÈ¡FUSE_ID
	WORD wFuseId[3] = {0};
	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x0A24 + 2*i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				0x0A24 + i, wFuseId[i]);
			return FALSE;
		}
		// Æ´½ÓFUSE_ID×÷ÎªSensorId
		strSensorId.AppendFormat(_T("%04X"), wFuseId[i]);
	}

	data[0]= 0x00;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, data, 2);

	return TRUE;
}