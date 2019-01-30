#include "StdAfx.h"
#include "OV12A10.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
OV12A10::OV12A10()
{
	name = _T("OV12A10");
	sensorType = Sensor_OV12A10;
    sid_len = 16;

}

//-------------------------------------------------------------------------------------------------
int OV12A10::do_prog_otp(int page, int addr, const void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}

int OV12A10::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-----------------------------------------------------------------------------
int OV12A10::do_get_sid(uint8_t *id)
{
    if (do_read_otp(0, 0x7000, (char*)id, sid_len) < 0)
    {
        return -1;
    }
    return 0;
}

BOOL OV12A10::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;

	WORD wFuseId[16] = {0};
	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x6000 +  i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%x]"),
				0x6000 + i, wFuseId[i]);
			return FALSE;
		}
		// Æ´½ÓFUSE_ID×÷ÎªSensorId
		strSensorId.AppendFormat(_T("%0X"), wFuseId[i]);
	}

	return TRUE;
}

int OV12A10::wb_writeback(uint8_t *regs, int len)
{
// 	dev->write_sensor(0x5032, regs, len);
// 
 	int val = dev->read_sensor(0x5000);
 	dev->write_sensor(0x5000, val | 0x08);

	dev->write_sensor(0x5100, regs, len);


	return 0;
}