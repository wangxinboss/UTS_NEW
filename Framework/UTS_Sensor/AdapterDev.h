#pragma once
#include "SensorDefine.h"
//#include "HVSI2C.h"

#ifdef SENSOR_EXPORTS
#define SENSOR_API __declspec(dllexport)
#else
#define SENSOR_API __declspec(dllimport)
#endif

namespace UTS
{
	typedef enum
	{
		BIT8_BIT8 = 0,
		BIT8_BIT16 = 1,
		BIT16_BIT8 = 2,
		BIT16_BIT16 = 3,
	} eI2CMode;

    class SENSOR_API AdapterDev
    {
    public:
        AdapterDev(void);
        ~AdapterDev(void);

        int i2c_read(int iic_mode, int start_addr, unsigned char* data, int len);
        int i2c_read(int mode, u16 reg);
        int i2c_write(int iic_mode, int start_addr, const char* data, int len);
        BOOL i2c_write(int mode, u16 reg, u16 val);

		int i2c_write(int iic_mode , int slav_addr,int start_addr, const char* data, int len);
		int i2c_read(int iic_mode, int slav_addr, int start_addr, unsigned char* data, int len);

        BOOL write_sensor(WORD wRegAddr, WORD wRegData);
        BOOL write_sensor(WORD wRegAddr, const void *data, int len);
        BOOL read_sensor(WORD wRegAddr, WORD &wRegData);
        int  read_sensor(int addr);
        int  read_sensor(int addr, void *data, int max_len);
        BOOL GetIICMode(int &nIICMode);
        BOOL SetIICMode(int nIICMode);
        BOOL set_power_pin(int idx, float volt);
        BOOL GetSlavAddr(int &nSlavAddr);
        BOOL I2CWrite(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf);
        BOOL I2CRead(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf);
        int  get_addr_len(int mode);
        void SetDevice(BaseDevice *pDevice);
        int write_reg_array(int mode, const struct regval *vals, int valnum);
        int get_regval_size(int mode, int *regsize, int *valsize);
        int GetOutOrder(void);
        int GetImageSize(SIZE &size);

    private:
        BaseDevice *m_pDevice;
    };
}
