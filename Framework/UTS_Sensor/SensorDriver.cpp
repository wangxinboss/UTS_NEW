#include "stdafx.h"
#include "CommonFunc.h"
#include "UTSDefine.h"
#include "SensorDefine.h"
#include "SensorDriver.h"
#include "SensorDriver/VirtualSensor/VirtualSensor.h"

#include "SensorDriver/AR1337/AR1337.h"
#include "SensorDriver/GC2385/GC2385.h"
#include "SensorDriver/GC5024/GC5024.h"
#include "SensorDriver/GC5025/GC5025.h"
#include "SensorDriver/GC5034/GC5034.h"
#include "SensorDriver/GC8024/GC8024.h"
#include "SensorDriver/Hi546/Hi546.h"
#include "SensorDriver/Hi556/Hi556.h"
#include "SensorDriver/Hi846/Hi846.h"
#include "SensorDriver/Hynix1333/Hynix1333.h"
#include "SensorDriver/Hynix843/Hynix843.h"
#include "SensorDriver/IMX214/IMX214.h"
#include "SensorDriver/IMX362/IMX362.h"
#include "SensorDriver/IMX363/IMX363.h"
#include "SensorDriver/IMX386/IMX386.h"
#include "SensorDriver/IMX499/IMX499.h"
#include "SensorDriver/IMX519/IMX519.h"
#include "SensorDriver/OV12870/OV12870.h"
#include "SensorDriver/OV12A10/OV12A10.h"
#include "SensorDriver/OV13855/OV13855.h"
#include "SensorDriver/OV13858/OV13858.h"
#include "SensorDriver/OV16885/OV16885.h"
#include "SensorDriver/OV4688/OV4688.h"
#include "SensorDriver/OV5648/OV5648.h"
#include "SensorDriver/OV5675/OV5675.h"
#include "SensorDriver/OV5695/OV5695.h"
#include "SensorDriver/OV8856/OV8856.h"
#include "SensorDriver/OV9750/OV9750.h"
#include "SensorDriver/S5K2L7/S5K2L7.h"
#include "SensorDriver/S5K2P7/S5K2P7.h"
#include "SensorDriver/S5K3L6/S5K3L6.h"
#include "SensorDriver/S5K3L8XX/S5K3L8XX.h"
#include "SensorDriver/S5K3P3/S5K3P3.h"
#include "SensorDriver/S5K3P8/S5K3P8.h"
#include "SensorDriver/S5K3P9SP/S5K3P9SP.h"
#include "SensorDriver/S5K3P9SX/S5K3P9SX.h"
#include "SensorDriver/S5K4H7YX/S5K4H7YX.h"
#include "SensorDriver/S5K4H8YX/S5K4H8YX.h"
#include "SensorDriver/S5K5E2YA/S5K5E2YA.h"
#include "SensorDriver/S5K5E8YX/S5K5E8YX.h"
#include "SensorDriver/S5K5E9/S5K5E9.h"
#include "SensorDriver/SP0A09/SP0A09.h"
#include "SensorDriver/SP2509V/SP2509V.h"
#include "SensorDriver/SP250A/SP250A.h"

#pragma comment(lib, "UTSFramework.lib")
#pragma warning (disable:4800)

namespace UTS
{    
    SensorDriver* g_pSensor = nullptr;
    int g_nSensorType = 0;

    LPCTSTR DLLGetSensor_Discription(void)
    {
        return SENSOR_DISCRIPTION;
    }

    SensorDriver* GetSensorInstance(
        BaseDevice* pDevice,
		int nSensorType)
    {
        assert(nullptr != pDevice);
        assert(nSensorType < Sensor_SUPPORT_NUM);

        if (g_pSensor != nullptr)
        {
            if (g_nSensorType == nSensorType)
            {
                return g_pSensor;
            }
            else
            {
                RELEASE_POINTER(g_pSensor);
            }
        }

        printk(SENSOR_DISCRIPTION);
        printk(_T("SensorType = %d"), nSensorType);

        switch (nSensorType)
        {
		case Sensor_AR1337:		g_pSensor = new AR1337;		break;
		case Sensor_GC2385:		g_pSensor = new GC2385;		break;
		case Sensor_GC5024:		g_pSensor = new GC5024;		break;
		case Sensor_GC5025:		g_pSensor = new GC5025;		break;
		case Sensor_GC5034:		g_pSensor = new GC5034;		break;
		case Sensor_GC8024:		g_pSensor = new GC8024;		break;
		case Sensor_Hi546:		g_pSensor = new Hi546;		break;
		case Sensor_Hi556:		g_pSensor = new Hi556;		break;
		case Sensor_Hi846:		g_pSensor = new Hi846;		break;
		case Sensor_Hynix1333:	g_pSensor = new Hynix1333;	break;
		case Sensor_Hynix843:	g_pSensor = new Hynix843;	break;
		case Sensor_IMX214:		g_pSensor = new IMX214;		break;
		case Sensor_IMX362:		g_pSensor = new IMX362;		break;
		case Sensor_IMX363:		g_pSensor = new IMX363;		break;
		case Sensor_IMX386:		g_pSensor = new IMX386;		break;
		case Sensor_IMX499:		g_pSensor = new IMX499;		break;
		case Sensor_IMX519:		g_pSensor = new IMX519;		break;
		case Sensor_OV12870:	g_pSensor = new OV12870;	break;
		case Sensor_OV12A10:	g_pSensor = new OV12A10;	break;
		case Sensor_OV13855:	g_pSensor = new OV13855;	break;
		case Sensor_OV13858:	g_pSensor = new OV13858;	break;
		case Sensor_OV16885:	g_pSensor = new OV16885;	break;
		case Sensor_OV4688:		g_pSensor = new OV4688;		break;
		case Sensor_OV5648:		g_pSensor = new OV5648;		break;
		case Sensor_OV5675:		g_pSensor = new OV5675;		break;
		case Sensor_OV5695:		g_pSensor = new OV5695;		break;
		case Sensor_OV8856:		g_pSensor = new OV8856;		break;
		case Sensor_OV9750:		g_pSensor = new OV9750;		break;
		case Sensor_S5K2L7:		g_pSensor = new S5K2L7;		break;
		case Sensor_S5K2P7:		g_pSensor = new S5K2P7;		break;
		case Sensor_S5K3L6:		g_pSensor = new S5K3L6;		break;
		case Sensor_S5K3L8XX:	g_pSensor = new S5K3L8XX;	break;
		case Sensor_S5K3P3:		g_pSensor = new S5K3P3;		break;
		case Sensor_S5K3P8:		g_pSensor = new S5K3P8;		break;
		case Sensor_S5K3P9SP:	g_pSensor = new S5K3P9SP;	break;
		case Sensor_S5K3P9SX:	g_pSensor = new S5K3P9SX;	break;
		case Sensor_S5K4H7YX:	g_pSensor = new S5K4H7YX;	break;
		case Sensor_S5K4H8YX:	g_pSensor = new S5K4H8YX;	break;
		case Sensor_S5K5E2YA:	g_pSensor = new S5K5E2YA;	break;
		case Sensor_S5K5E8YX:	g_pSensor = new S5K5E8YX;	break;
		case Sensor_S5K5E9:	    g_pSensor = new S5K5E9;		break;
		case Sensor_SP0A09:		g_pSensor = new SP0A09;		break;
		case Sensor_SP2509V:	g_pSensor = new SP2509V;	break;
		case Sensor_SP250A:		g_pSensor = new SP250A;		break;
        default:
            g_pSensor = new VirtualSensor;
            break;
        }

        g_pSensor->SetDevice(pDevice);
        g_nSensorType = nSensorType;

        return g_pSensor;
    }

    void DLLReleaseSensor_Instance(void)
    {
        if (nullptr != g_pSensor)
        {
            RELEASE_POINTER(g_pSensor);
        }
    }

    //-------------------------------------------------------------------------
    // BaseSensor Class
    //-------------------------------------------------------------------------
    int SensorDriver::is_virtual_senser_en = 0;
    SensorDriver::SensorDriver(void)
    {
        absolute_addr_oper = false;

        startAddr = 0;
        memset(user_pages, 0xFF, sizeof(user_pages));

        exposure_first_step = 50;
        exposure_max_step = 500;
        sid_len = -1;
        
        dev = new AdapterDev();
    }

    SensorDriver::~SensorDriver(void)
    {
        if (dev != nullptr)
        {
            RELEASE_POINTER(dev);
        }
    }

    int SensorDriver::SetVcmMove(__in int nVCM_CurrentDAC)
    {
        assert(nVCM_CurrentDAC >= 0);
        return 0;
    }

    BOOL SensorDriver::GetSensorId(__out CString &strSensorId)
    {
        strSensorId = _T("GetSensorId Not Impliment.");
        printk(strSensorId);
        return FALSE;
    }

    int SensorDriver::GetUserPageNum(void)
    {
        int i;
        for (i = 0; i < ARRAY_SIZE(user_pages); i++) {
            if (user_pages[i] == 0xFF) break;
        }
        return i;
    }

    void SensorDriver::SetDevice(BaseDevice *pDevice)
    {
        dev->SetDevice(pDevice);
    }

    //-----------------------------------------------------------------------------
    int SensorDriver::SetAttr(int attr, void *args)
    {
        switch (attr) {
        case AbsoluteAddrOperate:
            absolute_addr_oper = (bool)args;
            break;
        //case VirSenFileName:
        //    strcpy(vir_sensor_name, (const char*)args);
        //    if (fp) {
        //        fclose(fp);
        //        fp = fopen(vir_sensor_name, "wb+");
        //        if (!fp) printk("Open %s failed\n", vir_sensor_name);
        //    }
        //    break; 
        default:
            printk(_T("Not support SensorAttr[%d]\n"), attr);
            break;
        }
        return 0;
    }

    int SensorDriver::GetAttr(int attr, void *args)
    {
        switch (attr) {
        case AbsoluteAddrOperate:
            *(bool *)args = absolute_addr_oper;
            break;
        default:
            printk(_T("Not support SensorAttr[%d]\n"), attr);
            break;
        }
        return 0;
    }
    
    int SensorDriver::ProgOtp(int page, int addr, const void *data, int len)
    {
        if (is_virtual_senser_en) {
            return do_prog_vir(page,addr,data,len);
        }

        return do_prog_otp(page,addr,data,len);
    }

    int SensorDriver::ReadOtp(int page, int addr, void *data, int len)
    {
        if (is_virtual_senser_en) {
            return do_read_vir(page,addr,data,len);
        }

        return do_read_otp(page,addr,data,len);
    }

    int SensorDriver::do_prog_vir(int page, int addr, const void *data, int len)
    {
        if (!fp) return (SET_ERROR(SENSORDRIVER_ERROR_DEV));

        if (absolute_addr_oper) addr -= startAddr;

        fseek(fp, pageSize * page + addr, SEEK_SET);
        fwrite(data, 1, len, fp);
        fflush(fp);

        return len;
    }

    int SensorDriver::do_read_vir(int page, int addr, void *data, int len)
    {
        if (!fp) return (SET_ERROR(SENSORDRIVER_ERROR_DEV));

        if (absolute_addr_oper) addr -= startAddr;

        fseek(fp, 0, SEEK_END);
        int fileLen = ftell(fp);
        if (pageSize * page + addr >= fileLen) {
            memset(data, 0x00, len);
            return len;
        }

        fseek(fp, pageSize * page + addr, SEEK_SET);
        fread(data, 1, len, fp);

        return len;
    }




    //-----------------------------------------------------------------------------
    OVSensor::OVSensor()
    {
        exposure_first_step = 200;
        exposure_max_step = 3000;
    }
    int OVSensor::set_exposure(int e)
    {
		//!!--- 2018.08.27 Chi-Jen.Liao 0x3500部分开启操作
        dev->write_sensor(0x3500, GET_BITS(e, 16, 0xFF));
        dev->write_sensor(0x3501, GET_BITS(e, 8, 0xFF));
        dev->write_sensor(0x3502, GET_BITS(e, 0, 0xFF));
        return 0;
    }
    int OVSensor::get_exposure(void)
    {
		//!!--- 2018.08.27 Chi-Jen.Liao 0x3500部分开启操作
        int val = dev->read_sensor(0x3500);
        val <<= 8;
        val |= dev->read_sensor(0x3501);
        val <<= 8;
        val |= dev->read_sensor(0x3502);

        return val;
    }
    int OVSensor::get_exposure_settings(int e, std::map<int, int> &regs)
    {
		//!!--- 2018.08.27 Chi-Jen.Liao 0x3500部分开启操作
        regs[0x3500] = GET_BITS(e, 16, 0xFF);
        regs[0x3501] = GET_BITS(e, 8, 0xFF);
        regs[0x3502] = GET_BITS(e, 0, 0xFF);
        return 0;
    }
    //-----------------------------------------------------------------------------
    SamsungSensor::SamsungSensor()
    {
    }
    int SamsungSensor::set_exposure(int e)
    {
        uint8_t buf[2];
        put_be_val(e, buf, sizeof(buf));
        int ret = dev->i2c_write(BIT16_BIT16, 0x0202, 
            (const char*)buf, sizeof(buf));

        if (ret != 0 ) return -1;
		else return 0;
    }
    int SamsungSensor::get_exposure(void)
    {
        uint8_t buf[2];

        int ret = dev->i2c_read(BIT16_BIT16, 0x0202, 
            buf, sizeof(buf));
        if (ret != 0 ) return -1;

        return get_be_val(buf, sizeof(buf));
    }
    int SamsungSensor::get_exposure_settings(int e, std::map<int, int> &regs)
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
    //-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	SonySensor::SonySensor()
	{
		exposure_first_step = 200;
		exposure_max_step = 3000;
	}
	int SonySensor::set_exposure(int e)
	{
		uint8_t buf[2];
		put_be_val(e, buf, sizeof(buf));
		int ret = dev->i2c_write(BIT16_BIT8, 0x0202, 
			(const char*)buf, sizeof(buf));

		if (ret != 0 ) return -1;
		else return 0;
	}
	int SonySensor::get_exposure(void)
	{
		uint8_t buf[2];

		int ret = dev->i2c_read(BIT16_BIT8, 0x0202, 
			buf, sizeof(buf));
		if (ret != 0 ) return -1;

		return get_be_val(buf, sizeof(buf));
	}
	int SonySensor::get_exposure_settings(int e, std::map<int, int> &regs)
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
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	GCSensor::GCSensor()
	{

	}

	int GCSensor::set_exposure(int e)
	{
		dev->write_sensor(0x03, GET_BITS(e, 8, 0x7F));
		dev->write_sensor(0x04, GET_BITS(e, 0, 0xFF));
		return 0;
	}

	int GCSensor::get_exposure(void)
	{
		int val = dev->read_sensor(0x03);
		val <<= 8;
		val |= dev->read_sensor(0x04);

		return val;
	}

	int GCSensor::get_exposure_settings(int e, std::map<int, int> &regs)
	{

		regs[0x03] = GET_BITS(e, 8, 0x7F);
		regs[0x04] = GET_BITS(e, 0, 0xFF);
		return 0;
	}
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	HynixSensor::HynixSensor()
	{

	}

	int HynixSensor::set_exposure(int e)
	{
		uint8_t buf[2];
		put_be_val(e, buf, sizeof(buf));
		int ret = dev->i2c_write(BIT16_BIT16, 0x0074, 
			(const char*)buf, sizeof(buf));

		if (ret != 0 ) return -1;
		else return 0;
	}
	int HynixSensor::get_exposure(void)
	{
		uint8_t buf[2];

		int ret = dev->i2c_read(BIT16_BIT16, 0x0074, 
			buf, sizeof(buf));
		if (ret != 0 ) return -1;

		return get_be_val(buf, sizeof(buf));
	}
	int HynixSensor::get_exposure_settings(int e, std::map<int, int> &regs)
	{
		int i2c = 0;
		if (!dev->GetIICMode(i2c))
		{
			return -1;
		}

		if (i2c == BIT16_BIT16) {
			regs[0x0074] = e;
		} else {
			regs[0x0074] = GET_BITS(e, 8, 0xFF);
			regs[0x0075] = GET_BITS(e, 0, 0xFF);
		}
		return 0;
	}
	//-----------------------------------------------------------------------------
#if 0
	GCSensor::GCSensor()
	{
	}
	int GCSensor::set_exposure(int e)
	{
		/*dev->write_sensor(0x03, GET_BITS(e, 8, 0x3F));
		dev->write_sensor(0x04, GET_BITS(e, 0, 0xFF));
		return 0;*/

		uint8_t buf[2];
		put_be_val(e, buf, sizeof(buf));
		buf[0]=buf[0]&0x3f;
		int ret = dev->i2c_write(BIT8_BIT8, 0x03, 
			(const char*)buf, sizeof(buf));

		if (ret != 0 ) return -1;
		else return 0;	
	}
	int GCSensor::get_exposure(void)
	{
		/*int val = dev->read_sensor(0x03);
		val <<= 8;
		val |= dev->read_sensor(0x04);
		return val;*/

		uint8_t buf[2];
		int ret = dev->i2c_read(BIT8_BIT8, 0x03, 
			buf, sizeof(buf));
		if (ret != 0 ) return -1;

		return get_be_val(buf, sizeof(buf));
	}
	int GCSensor::get_exposure_settings(int e, std::map<int, int> &regs)
	{
		/*regs[0x03] = GET_BITS(e, 8, 0x3F);
		regs[0x04] = GET_BITS(e, 0, 0xFF);
		return 0;*/

		int i2c = 0;
		if (!dev->GetIICMode(i2c))
		{
			return -1;
		}

		regs[0x03] = GET_BITS(e, 8, 0x3F);
		regs[0x04] = GET_BITS(e, 0, 0xFF);
		return 0;
	}
#endif
	//-----------------------------------------------------------------------------

	SuperPixSensor::SuperPixSensor()
	{
	}
	int SuperPixSensor::set_exposure(int e)
	{
		uint8_t page[1]={0x01};

		//put_be_val(0x00, page, sizeof(page));
		int ret = dev->i2c_write(BIT8_BIT8, 0xfd, (const char*)page,sizeof(page));
		if (ret != 0 ) return -1;

		/*put_be_val(e, buf, sizeof(buf));
		ret = dev->i2c_write(BIT8_BIT8, 0x03, 
			(const char*)buf, sizeof(buf));*/


		dev->write_sensor(0x03, GET_BITS(e, 8, 0x07));
		dev->write_sensor(0x04, GET_BITS(e, 0, 0xFF));
		dev->write_sensor(0x01, 0x01);   //Add by ysz on 2018/01/04
		return 0;

		/*	if (ret != 0 ) return -1;
		else return 0;*/
	}
	int SuperPixSensor::get_exposure(void)
	{
		uint8_t buf[2],page[1]={0x01};

		//put_be_val(0x00, page, sizeof(page));
		int ret = dev->i2c_write(BIT8_BIT8, 0xfd, (const char*)page,sizeof(page));
		if (ret != 0 ) return -1;

		//ret = dev->i2c_read(BIT16_BIT8, 0x03, buf, sizeof(buf)); //disable by ysz on 2018/01/02
		ret = dev->i2c_read(BIT8_BIT8, 0x03, buf, sizeof(buf)); //update by ysz on 2018/01/02
		if (ret != 0 ) return -1;

		return get_be_val(buf, sizeof(buf));
	}
	int SuperPixSensor::get_exposure_settings(int e, std::map<int, int> &regs)
	{
		int i2c = 0;
		if (!dev->GetIICMode(i2c))
		{
			return -1;
		}

		if (i2c == BIT16_BIT16) {
			regs[0x03] = e;
		} else {
			regs[0x03] = GET_BITS(e, 8, 0xFF);
			regs[0x04] = GET_BITS(e, 0, 0xFF);
		}
		return 0;
	}
}
