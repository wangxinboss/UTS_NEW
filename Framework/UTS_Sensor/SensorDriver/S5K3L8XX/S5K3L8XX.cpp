#include "StdAfx.h"
#include "S5K3L8XX.h"
#include "CommonFunc.h"

//-------------------------------------------------------------------------------------------------
S5K3L8XX::S5K3L8XX()
{
	name = _T("S5K3L8XX");
	sensorType = Sensor_S5K3L8XX;
    sid_len = 12;
}
//-------------------------------------------------------------------------------------------------
int S5K3L8XX::do_prog_otp(int page, int addr, const void *data, int len)
{
	const char *cData = (const char *)data;
	char pagedata[64];
	char data_write[2];

	Sleep(10);
	data_write[0]= page;
	data_write[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A02, data_write, 2);	//set page#

	for(int i=0;i<64;i++)
		pagedata[i]=0;
	for(int i=0;i<len;i++)
	{
		pagedata[addr-0x0A04+i]=cData[i];
	}
	for(int i=0;i<64;i+=2)
	{	
		data_write[0]=0;
		data_write[1]=0;
		data_write[0]=pagedata[i];
		data_write[1]=pagedata[i+1];
		dev->i2c_write(BIT16_BIT16, 0x0A04+i, data_write, 2);	//Write to buffer	
	}
	data_write[0]= 0x03;
	data_write[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A00, data_write , 2);	//Write to NVM
	Sleep(40);
	data_write[0]= 0x00;
	data_write[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A00,data_write, 2);	//disable NVM
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int S5K3L8XX::do_read_otp(int page, int addr, void *data, int len)
{
	unsigned char *cData = (unsigned char *)data;
	char data_write[2];

	Sleep(10);
	data_write[0]= page;
	data_write[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A02, data_write, 2);	//set page#
	data_write[0]= 0x01;
	data_write[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A00, data_write, 2);		//enable read NVM
	for(int i=0;i<len;i+=2)
	{
		dev->i2c_read(BIT16_BIT16, addr+i, cData+i, 2);	//Read to buffer
	}	
	data_write[0]= 0x00;
	data_write[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A00,data_write, 2);	//disable NVM
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------

int S5K3L8XX::do_get_sid(uint8_t *id)
{
    return do_read_otp(0, 32, id, sid_len);
}

BOOL S5K3L8XX::GetSensorId(__out CString &strSensorId)
{
	strSensorId = EMPTY_STR;
	// 读取读保护数据
	char data[2];

	data[0]= 0x40;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x6028, data, 2);
	
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

	data[0]= 0x00;
	data[1]= 0x00;
	dev->i2c_write(BIT16_BIT16, 0x0A00, data, 2);
	return TRUE;
}


int S5K3L8XX::wb_writeback(uint8_t *regs, int len)
{
// 	dev->write_sensor(0x5032, regs, len);
// 
	char en = 1;
	dev->i2c_write(BIT16_BIT8, 0x3058, &en, 1);
	dev->i2c_write(BIT16_BIT8, 0x020E, (const char*)regs, len);

	/*WriteSensorReg( CurrentSensor.SlaveID, 0x6028, 0x4000, 4 );
	WriteSensorReg( CurrentSensor.SlaveID, 0x602a, 0x0100, 4 );
	WriteSensorReg( CurrentSensor.SlaveID, 0x6f12, 0x00, 3 );// Stream off
	Sleep(200);

	WriteSensorReg(CurrentSensor.SlaveID, 0x6028, 0x4000, 4);	
	WriteSensorReg(CurrentSensor.SlaveID, 0x602a, 0x3058,4);
	WriteSensorReg(CurrentSensor.SlaveID, 0x6f12, 0x01, 3);//setting

	WriteSensorReg(CurrentSensor.SlaveID, 0x602a, 0x0210,4);
	WriteSensorReg(CurrentSensor.SlaveID, 0x6f12, R_GAIN, 4);

	WriteSensorReg(CurrentSensor.SlaveID, 0x602a, 0x0212,4);
	WriteSensorReg(CurrentSensor.SlaveID, 0x6f12, B_GAIN, 4);

	WriteSensorReg(CurrentSensor.SlaveID, 0x602a, 0x020e,4);
	WriteSensorReg(CurrentSensor.SlaveID, 0x6f12, G_GAIN, 4);

	WriteSensorReg(CurrentSensor.SlaveID, 0x602a, 0x0214,4);
	WriteSensorReg(CurrentSensor.SlaveID, 0x6f12, G_GAIN, 4);

	WriteSensorReg( CurrentSensor.SlaveID, 0x6028, 0x4000, 4 );
	WriteSensorReg( CurrentSensor.SlaveID, 0x602a, 0x0100, 4 );
	WriteSensorReg( CurrentSensor.SlaveID, 0x6f12, 0x01, 3 );// Stream on*/

	return 0;
}