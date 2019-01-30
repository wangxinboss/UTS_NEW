#pragma once
#include <stdint.h>
#include "BaseDevice.h"
#include "AdapterDev.h"

#ifdef SENSOR_EXPORTS
#define SENSOR_API __declspec(dllexport)
#else
#define SENSOR_API __declspec(dllimport)
#endif

namespace UTS
{
    #define VIRTUAL_SENSOR_FILE "VirtualSensor.bin"

    enum SensorDriver_ErrorType
    {
        SENSORDRIVER_ERROR_NO,
        SENSORDRIVER_ERROR_DEV,
        SENSORDRIVER_ERROR_TIMEOUT,
        SENSORDRIVER_ERROR_NOTSUPPORT,
        SENSOR_OTP_NOEmpty,
        SENSORDRIVER_ERROR_SEGMENTFAULT,
		SENSORDRIVER_ERROR_LSCCALI,
    };

    enum SensorAttrType
    {
        AbsoluteAddrOperate,
        VirSenFileName
    };

    class SENSOR_API SensorDriver
    {
    public:
        SensorDriver(void);
        virtual ~SensorDriver(void);

        int ProgOtp(int page, int addr, const void *data, int len);
        int ReadOtp(int page, int addr, void *data, int len);

        virtual int do_prog_otp(int page, int addr, const void *data, int len) = 0;
        virtual int do_read_otp(int page, int addr, void *data, int len) = 0;
        virtual int do_get_sid(uint8_t *id) {return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
        virtual int wb_writeback(uint8_t *regs, int len) { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int lsc_writeback(uint8_t *regs, int len) { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int spc_writeback(uint8_t *regs, int len) { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int spc_setting1_writeback() { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int spc_setting2_writeback() { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int spc_setting3_writeback() { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int get_PD_data(int *confidence,int *pdtable) { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int set_DCC_stats_hold() { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int set_DCC_stats_reset() { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}

        virtual int SetVcmMove(__in int nVCM_CurrentDAC);
        virtual BOOL GetSensorId(__out CString &strSensorId);
        void SetDevice(BaseDevice *pDevice);
        int SetAttr(int attr, void *args);
        int GetAttr(int attr, void *args);
        int GetPageSize() { return pageSize; }
        int GetStartAddr() { return startAddr; }
        int GetUserPageNum(void);

		//AE
		virtual int set_exposure(int e){return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int get_exposure(void){return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
		virtual int get_exposure_settings(int e, std::map<int, int> &regs){return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}

		virtual int GetThermal(int *meter){return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT);}
        int exposure_first_step;
        int exposure_max_step;
        static int is_virtual_senser_en;
        int sensorType;
        const TCHAR *name;
        uint8_t user_pages[0x40];
        AdapterDev *dev;

    protected:
        int test_bit(int val, int bit) { return val & (1 << bit);}
        int SET_ERROR(int errorNo) { this->errorNo = errorNo; return -errorNo; }

        int errorNo;
        int sid_len;
        int pageSize;
        int startAddr;
        bool absolute_addr_oper;
        FILE *fp;

    private:
        int do_prog_vir(int page, int addr, const void *data, int len);
        int do_read_vir(int page, int addr, void *data, int len);
        int do_get_sid_vir(uint8_t *id);
    };

    class OVSensor : public SensorDriver
    {
    public:
        OVSensor();
        virtual int set_exposure(int e);
        virtual int get_exposure(void);
        virtual int get_exposure_settings(int e, std::map<int, int> &regs);
    };

    class SamsungSensor : public SensorDriver
    {
    public:
        SamsungSensor();
        virtual int set_exposure(int e);
        virtual int get_exposure(void);
        virtual int get_exposure_settings(int e, std::map<int, int> &regs);
    };

	class SonySensor : public SensorDriver
	{
	public:
		SonySensor();
		virtual int set_exposure(int e);
		virtual int get_exposure(void);
		virtual int get_exposure_settings(int e, std::map<int, int> &regs);
	};

	class GCSensor : public SensorDriver
	{
	public:
		GCSensor();
		virtual int set_exposure(int e);
		virtual int get_exposure(void);
		virtual int get_exposure_settings(int e, std::map<int, int> &regs);
	};

	class HynixSensor : public SensorDriver
	{
	public:
		HynixSensor();
		virtual int set_exposure(int e);
		virtual int get_exposure(void);
		virtual int get_exposure_settings(int e, std::map<int, int> &regs);
	};
	class SuperPixSensor : public SensorDriver
	{
	public:
		SuperPixSensor();
		virtual int set_exposure(int e);
		virtual int get_exposure(void);
		virtual int get_exposure_settings(int e, std::map<int, int> &regs);
	};
    EXTERN_C
    {
        SENSOR_API LPCTSTR DLLGetSensor_Discription(void);

        SENSOR_API SensorDriver* GetSensorInstance(
            BaseDevice* pDevice,
			int nSensorType);

        SENSOR_API void DLLReleaseSensor_Instance(void);
    }
}
