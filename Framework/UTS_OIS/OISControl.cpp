#include "stdafx.h"
#include "UTSDefine.h"
#include "OISControl.h"
#include "./OISDriver/VirtualOIS/VirtualOIS.h"
#include "./OISDriver/Onsemi_LC898123/OnSemi_LC898123.h"
#include "CommonFunc.h"

#pragma comment(lib, "UTSFramework.lib")
#pragma comment(lib, "UTS_Sensor.lib")



namespace UTS
{
	OISDriver* g_pOIS = nullptr;
	int g_nOISType = 0;

	//---------------------------------------------------------------------------------------
	OISDriver::OISDriver(BaseDevice *dev) : dev(dev)
	{
		errorNo = OIS_ERROR_NO;
		

		Adev = new AdapterDev();
	}

	OISDriver::~OISDriver(void)
	{
		if (Adev != nullptr)
		{
			RELEASE_POINTER(Adev);
		}
	}

	void OISDriver::SetDevice(BaseDevice *pDevice)
	{
		Adev->SetDevice(pDevice);
	}

	//---------------------------------------------------------------------------------------
	
	//-----------------------------------------------------------------------------
	OISDriver* CreateOIS(OIS_TYPE type, BaseDevice *dev)
	{
		OISDriver *OIS = NULL;
		switch (type)
		{
		case OIS_TYPE_OnSemi_LC898123:
			OIS = new OIS_ON123(dev); break;
		
		default:
			OIS = new VirtualOIS(dev);
			break;
		}
		OIS->SetDevice(dev);
		OIS->oisType = type;
		return OIS;
	}

	int OISDriver::Write(int addr, const void *data, int len)
	{
		/*if (addr + len > size) return -1;

		const char *cData = (const char *)data;
		while (len > 0) {
			int realLen = min(pageSize-addr%pageSize, len);
			int ret = Adev->i2c_write(i2cMode,i2cAddr,addr,cData,realLen);

			if (ret != 0 ) {
				uts.log.Error(_T("EEPROM I2C Write Error!"));
				return SET_ERROR(OIS_ERROR_I2C);
			}
			Sleep(20); 
			len -= realLen;
			addr += realLen;
			cData += realLen;
		}
		SET_ERROR(OIS_ERROR_NO);
		return (int)cData - (int)data;
		*/
		const char *cData = (const char *)data;

	//	printk(_T("Adev->i2c_write\n"));
		int ret = Adev->i2c_write(i2cMode,i2cAddr,addr,cData,len);
	//	printk(_T("Adev->i2c_write end\n"));

		if (ret != 0 ) {
			uts.log.Error(_T("OIS I2C Write Error!"));
			return SET_ERROR(OIS_ERROR_I2C);
		}

		return 0 ;
	}
	int OISDriver::Read(int addr, void *data, int len)
	{
		/*if (addr + len > size) return -1;

		unsigned char *cData = (unsigned char *)data;
		while (len > 0) {
			int realLen = min(pageSize-addr%pageSize, len);
			int ret = Adev->i2c_read(i2cMode,i2cAddr,addr,cData,realLen);

			if (ret != 0) {
				uts.log.Error(_T("OIS I2C Read Error!"));
				return SET_ERROR(OIS_ERROR_I2C);
			}
			len -= realLen;
			addr += realLen;
			cData += realLen;
		}
		SET_ERROR(OIS_ERROR_NO);
		return (int)cData - (int)data;
		*/
		unsigned char *cData = (unsigned char *)data;
		int ret = Adev->i2c_read(i2cMode,i2cAddr,addr,cData,len);

		if (ret != 0) {
			uts.log.Error(_T("OIS I2C Read Error!"));
			return SET_ERROR(OIS_ERROR_I2C);
		}

		return 0 ;
	}


	OISDriver *GetOISDriverInstance(BaseDevice *dev, int oisType)
	{
		if (oisType < 0 || oisType >= OIS_TYPE_SIZE) {
			uts.log.Error(_T("No ois device!"));
			return NULL;
		}

		OIS_TYPE type = (OIS_TYPE)oisType;  
		g_pOIS = CreateOIS(type, dev);


		g_nOISType = type;

		return g_pOIS;
	}


	//-----------------------------------------------------------------------------
}

