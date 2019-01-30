#pragma once
#include <stdint.h>
#include "BaseDevice.h"
#include "AdapterDev.h"

#ifdef EEPROM_EXPORTS
#define EEPROM_API __declspec(dllexport)
#else
#define EEPROM_API __declspec(dllimport)
#endif

namespace UTS
{
#define SIZE_K  1024

	enum EEPROM_TYPE
	{
		EEPROM_TYPE_Virtual,
		EEPROM_TYPE_24C16,
		EEPROM_TYPE_24C32,
		EEPROM_TYPE_24C256,
		EEPROM_TYPE_DW9761,
		EEPROM_TYPE_24C64,
		EEPROM_TYPE_ZC533,
		EEPROM_TYPE_FM24C64D,
		EEPROM_TYPE_BL24C64,
		EEPROM_TYPE_CAT24C64,
		EEPROM_TYPE_24P64,
		EEPROM_TYPE_GT9767,
		EEPROM_TYPE_M24C64,
		EEPROM_TYPE_SIZE
	};
	enum EEPROM_ERROR
	{
		EEPROM_ERROR_NO,
		EEPROM_ERROR_NOTSUPPORT,
		EEPROM_ERROR_I2C,
	};


	class EEPROM_API EEPROMDriver
	{
	public:
		EEPROMDriver(BaseDevice *dev, int i2cAddr);
		virtual ~EEPROMDriver(void);

		virtual int Write(int addr, const void *data, int len);
		virtual int Write(int page,int addr, const void *data, int len){return SET_ERROR(EEPROM_ERROR_NOTSUPPORT);}
		virtual int Read(int addr, void *data, int len);
		virtual int erase(int opt, void *args) {return SET_ERROR(EEPROM_ERROR_NOTSUPPORT);}

		BOOL RegWrite(WORD wRegAddr, WORD wRegData);
		BOOL I2CWrite(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf);
		BOOL I2CRead(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf);
		BOOL Power(int pin,float volt);

		int GetSize() {return size;}
		void SetDevice(BaseDevice *pDevice);
		int eepromType;
		//struct list_head list;

	protected:
		AdapterDev *Adev;
		BaseDevice *pdev;

		int i2cAddr;
		int i2cMode;
		int pageSize;
		int size;

		int errorNo;
		int SET_ERROR(int errorNo) { this->errorNo = errorNo; return -errorNo;}
	};


	EXTERN_C
	{
		EEPROM_API EEPROMDriver *GetEEPromDriverInstance(BaseDevice *dev, int eepromType);
	}
}

