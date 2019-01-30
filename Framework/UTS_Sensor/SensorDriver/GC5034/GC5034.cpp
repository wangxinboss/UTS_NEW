#include "StdAfx.h"
#include "GC5034.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
GC5034::GC5034()
{
	name = _T("GC5034");
	sensorType = Sensor_GC5034;
    sid_len = 9;
}

#define BIT16_BIT8 2
#define BIT16_BIT16 3
//-------------------------------------------------------------------------------------------------

int GC5034::do_prog_otp(int page, int addr, const void *data, int len)
{
	const char *cData = (const char *)data;

	char data_write=0x00;	
	char pageNo=(char)page;

	char pagedata[7]={0x68,0x01,0x00,0x04,0x03,0x00,0x01};

	dev->i2c_write(BIT16_BIT8, 0x3B42, &pagedata[0], 1);	//write 0x68 test mode
	dev->i2c_write(BIT16_BIT8, 0x3B41, &pagedata[1], 1);	//test mode enable
	dev->i2c_write(BIT16_BIT8, 0x3B40, &pagedata[2], 1);	//write disable off
	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[3], 1);	//initial state
	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[4], 1);	//set write mode
	dev->i2c_write(BIT16_BIT8, 0x3B42, &pagedata[5], 1);	//test mode
	dev->i2c_write(BIT16_BIT8, 0x3B45, &pagedata[6], 1);	//write time unlimited mode
	
	dev->i2c_write(BIT16_BIT8, 0x0A02, &pageNo, 1);  //write page setting
	dev->i2c_write(BIT16_BIT8, 0x0A00, &pagedata[4], 1);	  //write 0x03  //set write mode
	Sleep(10);

	for(int i=0;i<len;i++)
	{	
		data_write=cData[i];
		dev->i2c_write(BIT16_BIT8, addr+i, &data_write, 1);	//Write to buffer	
	}
	Sleep(100);

	data_write=0x04;
	dev->i2c_write(BIT16_BIT8, 0x0A00, &data_write, 1);
	data_write=0x00;
	dev->i2c_write(BIT16_BIT8, 0x0A00, &data_write, 1);	
	data_write=0x01;
	dev->i2c_write(BIT16_BIT8, 0x3B40, &data_write, 1);	
	return TRUE;
}

int GC5034::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;

	unsigned char data_read=0;	
	char data_write=0x00;	

	data_write=0x04;	
	dev->i2c_write(BIT16_BIT8, 0x0A00, &data_write, 1);	//initial state
	data_write=page;	
	dev->i2c_write(BIT16_BIT8, 0x0A02, &data_write, 1);	//Write page
	data_write=0x01;	
	dev->i2c_write(BIT16_BIT8, 0x0A00, &data_write, 1);   //set read mode
	Sleep(150);

	for(int i=0;i<len;i++)
	{	
		data_read=0;
		dev->i2c_read(BIT16_BIT8, addr+i, &data_read, 1);
		cData[i]=data_read;
	}
	//dev->i2c_read(BIT16_BIT8, addr, cData, len);
	Sleep(300);

	data_write=0x04;
	dev->i2c_write(BIT16_BIT8, 0x0A00, &data_write, 1);
	return TRUE;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//int GC5034::do_get_sid(uint8_t *id)
//{
//    if (!do_read_otp(0, 0x0001, (char*)id, sid_len))
//    {
//        return -1;
//    }
//    return 0;
//}
//
//BOOL GC5034::GetSensorId(__out CString &strSensorId)
//{
//	strSensorId = EMPTY_STR;
//
//	char wFuseId[9] = {0};
//
//	if (!do_read_otp(0, 0x0001, wFuseId, ARRAY_SIZE(wFuseId)))
//	{
//		return FALSE;
//	}
//
//	for (size_t i = 0; i < ARRAY_SIZE(wFuseId); i++)
//	{
//
//		printk(_T("RegRead Error. [Reg = 0x%x][Data = 0x%02x]"),
//				0x0001+i, wFuseId[i]);
//		// Æ´½ÓFUSE_ID×÷ÎªSensorId
//		strSensorId.AppendFormat(_T("%02X"), wFuseId[i]);
//	 }
//
//	return TRUE;
//}

int GC5034::wb_writeback(uint8_t *regs, int len)
{
  //	int val = dev->read_sensor(0x0A05);
	/*char data_write;
	unsigned char data_read;
	char * regs_wb= (char*)regs;

	dev->i2c_read(BIT16_BIT8, 0x0A05, &data_read, 1);
	data_write = (data_read|0x08);
  	dev->i2c_write(BIT16_BIT8,0x0A05,&data_write ,1);
	dev->i2c_write(BIT16_BIT8,0x0078, regs_wb, len);*/

	dev->write_sensor(0x020E, regs, len);
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
int GC5034::set_exposure(int e)
{
	uint8_t buf[2];
	put_be_val(e, buf, sizeof(buf));
	int ret = dev->i2c_write(BIT16_BIT8, 0x0202, 
		(const char*)buf, sizeof(buf));
	Sleep(10);

	if (ret != 0 ) return -1;
	else return 0;
}
int GC5034::get_exposure(void)
{
	uint8_t buf[2];

	int ret = dev->i2c_read(BIT16_BIT8, 0x0202, 
		buf, sizeof(buf));
	if (ret != 0 ) return -1;

	return get_be_val(buf, sizeof(buf));
}
int GC5034::get_exposure_settings(int e, std::map<int, int> &regs)
{
	int i2c = 0;
	if (!dev->GetIICMode(i2c))
	{
		return -1;
	}

	if (i2c == BIT16_BIT16) {
		regs[0x0202] = e;
	} else {
		regs[0x0202] = GET_BITS(e, 8, 0xFF);
		regs[0x0203] = GET_BITS(e, 0, 0xFF);
	}
	return 0;
}