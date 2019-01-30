#include "StdAfx.h"
#include "OV4688.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
OV4688::OV4688()
{
	name = _T("OV4688");
	sensorType = Sensor_OV4688;
    sid_len = 16;
}

//-------------------------------------------------------------------------------------------------
int OV4688::do_prog_otp(int page, int addr, const void *data, int len)
{
	return TRUE;
}

int OV4688::do_read_otp(int page, int addr, void *data, int len)
{
	return TRUE;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int OV4688::do_get_sid(uint8_t *id)
{
    if (!do_read_otp(0, 0x7000, (char*)id, sid_len))
    {
        return -1;
    }
    return 0;
}

BOOL OV4688::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;

	WORD wFuseId[16] = {0};

	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
	{
		if (!dev->read_sensor(0x7000 +  i, wFuseId[i]))
		{
			printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%02x]"),
				0x7000+i, wFuseId[i]);
			return FALSE;
		}

		// Æ´½ÓFUSE_ID×÷ÎªSensorId
		strSensorId.AppendFormat(_T("%02X"), wFuseId[i]);
	 }

	return TRUE;
}
