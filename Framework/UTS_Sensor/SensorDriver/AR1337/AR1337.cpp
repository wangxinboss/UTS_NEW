#include "StdAfx.h"
#include "AR1337.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
AR1337::AR1337()
{
	name = _T("AR1337");
	sensorType = Sensor_AR1337;
    sid_len = 16;
}
//-------------------------------------------------------------------------------------------------
int AR1337::do_prog_otp(int page, int addr, const void *data, int len)
{
    return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-------------------------------------------------------------------------------------------------
int AR1337::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-------------------------------------------------------------------------------------------------

int AR1337::do_get_sid(uint8_t *id)
{
    return do_read_otp(0, 32, id, sid_len);
}

BOOL AR1337::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	u16 reg=0x00;
	char data[2];

	data[0]= 0x00;
	data[1]= 0x10;
	dev->i2c_write(BIT16_BIT16, 0x301A, data, 2);//Set bit 4 in 0x301a to 1

	data[0]= 0x01;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x304C, data, 2);//Set record type to 0x01

	data[0]= 0x02;
	data[1]= 0x10;
	dev->i2c_write(BIT16_BIT16, 0x304A, data, 2);//Start auto read
	//dev->i2c_read(BIT16_BIT8,&reg);//check 0x304a bit 6 =1 then otp read succeeded
	reg = dev->read_sensor(0x304A);
	if (((reg&0x40)>>6) != 0x01)
	{
		printk(_T("RegRead Error read fuseid fail!"));
		return FALSE;
	}

	// 读取FUSE_ID
	WORD wFuseId[8] = {0};
	for (int i = 0; i < 8; i++)
	{
		if (!dev->read_sensor(0x380E - 2 * i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				0x380E -  2 * i, wFuseId[i]);
			return FALSE;
		}
		// 拼接FUSE_ID作为SensorId
		strSensorId.AppendFormat(_T("%04X"), wFuseId[i]);
	}

	data[0]= 0x00;
	data[1]= 0x1C;
	dev->i2c_write(BIT16_BIT16, 0x301A, data, 2);//Set record type to 0x01
	return TRUE;
}


int AR1337::wb_writeback(uint8_t *regs, int len)
{
	dev->i2c_write(BIT16_BIT8, 0x020E, (const char*)regs, len);
	return 0;
}