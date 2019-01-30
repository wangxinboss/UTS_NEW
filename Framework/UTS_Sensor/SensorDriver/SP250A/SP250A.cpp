#include "StdAfx.h"
#include "SP250A.h"
#include "CommonFunc.h"
//-------------------------------------------------------------------------------------------------
SP250A::SP250A()
{
	name = _T("SP250A");
	sensorType = Sensor_SP250A;
    sid_len = 5;
}
//-------------------------------------------------------------------------------------------------
int SP250A::do_prog_otp(int page, int addr, const void *data, int len)
{
    return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-------------------------------------------------------------------------------------------------
int SP250A::do_read_otp(int page, int addr, void *data, int len)
{
   return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);
}
//-------------------------------------------------------------------------------------------------
//int SP250A::set_exposure(int e)
//	{
//		uint8_t buf[2],page[1]={0x01};
//
//		//put_be_val(0x00, page, sizeof(page));
//		int ret = dev->i2c_write(BIT8_BIT8, 0xfd, (const char*)page,sizeof(page));
//		if (ret != 0 ) return -1;
//
//		/*put_be_val(e, buf, sizeof(buf));
//		ret = dev->i2c_write(BIT8_BIT8, 0x03, 
//			(const char*)buf, sizeof(buf));*/
//
//
//		dev->write_sensor(0x03, GET_BITS(e, 8, 0xFF));
//		dev->write_sensor(0x04, GET_BITS(e, 0, 0xFF));
//		dev->write_sensor(0x01, 0x01);   //Add by ysz on 2018/01/04
//		return 0;
//
//		/*	if (ret != 0 ) return -1;
//		else return 0;*/
//	}
//	int SP250A::get_exposure(void)
//	{
//		uint8_t buf[2],page[1]={0x01};
//
//		//put_be_val(0x01, page, sizeof(page));
//		int ret = dev->i2c_write(BIT8_BIT8, 0xfd, (const char*)page,sizeof(page));
//		if (ret != 0 ) return -1;
//
//		//ret = dev->i2c_read(BIT16_BIT8, 0x03, buf, sizeof(buf)); //disable by ysz on 2018/01/02
//		ret = dev->i2c_read(BIT8_BIT8, 0x03, buf, sizeof(buf)); //update by ysz on 2018/01/02
//		if (ret != 0 ) return -1;
//
//		return get_be_val(buf, sizeof(buf));
//	}
//	int SP250A::get_exposure_settings(int e, std::map<int, int> &regs)
//	{
//		int i2c = 0;
//		if (!dev->GetIICMode(i2c))
//		{
//			return -1;
//		}
//
//		if (i2c == BIT16_BIT16) {
//			regs[0x03] = e;
//		} else {
//			regs[0x03] = GET_BITS(e, 8, 0xFF);
//			regs[0x04] = GET_BITS(e, 0, 0xFF);
//		}
//		return 0;
//	}
