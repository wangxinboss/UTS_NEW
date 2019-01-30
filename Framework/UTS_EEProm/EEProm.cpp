#include "stdafx.h"
#include "UTSDefine.h"
#include "EEProm.h"
#include "EEProm/VirtualEEProm/VirtualEEProm.h"
#include "EEProm/24C16/24C16.h"
#include "EEProm/24C32/24C32.h"
#include "EEProm/24C64/24C64.h"
#include "EEProm/zc533/ZC533.h"
#include "EEProm/FM24C64D/24C64D.h"
#include "EEProm/BL24C64/BL24C64.h"
#include "EEProm/CAT24C64/CAT24C64.h"
#include "EEProm/24P64/24P64.h"
#include "EEProm/GT9767/GT9767.h"
#include "EEProm/M24C64/M24C64.h"

#pragma comment(lib, "UTSFramework.lib")
#pragma comment(lib, "UTS_Sensor.lib")

namespace UTS
{
	EEPROMDriver* g_pEEPROM = nullptr;
	int g_nEEPROMType = 0;

	//---------------------------------------------------------------------------------------
	EEPROMDriver::EEPROMDriver(BaseDevice *dev, int i2cAddr) : pdev(dev), i2cAddr(i2cAddr)
	{
		//i2cAddr = CHVSBase::INIReadIntSensor("Project_Setting", "EEPromAddr", 0xA0);
		errorNo = EEPROM_ERROR_NO;

		Adev = new AdapterDev();
	}

	EEPROMDriver::~EEPROMDriver(void)
	{
		if (Adev != nullptr)
		{
			RELEASE_POINTER(Adev);
		}
	}

	void EEPROMDriver::SetDevice(BaseDevice *pDevice)
	{
		Adev->SetDevice(pDevice);
	}

	//---------------------------------------------------------------------------------------
	int EEPROMDriver::Write(int addr, const void *data, int len)
	{
		if (addr + len > size) return -1;

		const char *cData = (const char *)data;
		while (len > 0) {
			int realLen = min(pageSize-addr%pageSize, len);
			int ret = Adev->i2c_write(i2cMode,i2cAddr,addr,cData,realLen);
			
			if (ret != 0 ) {
				uts.log.Error(_T("EEPROM I2C Write Error!"));
				return SET_ERROR(EEPROM_ERROR_I2C);
			}
			Sleep(20); 
			len -= realLen;
			addr += realLen;
			cData += realLen;
		}
		SET_ERROR(EEPROM_ERROR_NO);
		return (int)cData - (int)data;
	}
	int EEPROMDriver::Read(int addr, void *data, int len)
	{
		if (addr + len > size) return -1;

		unsigned char *cData = (unsigned char *)data;
		while (len > 0) {
			int realLen = min(pageSize-addr%pageSize, len);
			int ret = Adev->i2c_read(i2cMode,i2cAddr,addr,cData,realLen);
			
			if (ret != 0) {
				uts.log.Error(_T("EEPROM I2C Read Error!"));
				return SET_ERROR(EEPROM_ERROR_I2C);
			}
			len -= realLen;
			addr += realLen;
			cData += realLen;
		}
		SET_ERROR(EEPROM_ERROR_NO);
		return (int)cData - (int)data;
	}
	//-----------------------------------------------------------------------------
	EEPROMDriver* CreateEEProm(EEPROM_TYPE type,BaseDevice *dev)
	{
		EEPROMDriver *eeprom = NULL;
		switch (type)
		{
		case EEPROM_TYPE_24C16:
			eeprom = new EE24C16(dev, 0xA0); 
			break;

		case EEPROM_TYPE_24C32: 
			eeprom = new EE24C32(dev, 0xA0); 
			break;

		case EEPROM_TYPE_24C64: 
			eeprom = new EE24C64(dev, 0xA0); 
			break;
		case EEPROM_TYPE_ZC533: 
			eeprom = new ZC533(dev, 0xA0); 
			break;

		case EEPROM_TYPE_FM24C64D: 
			eeprom = new EEFM24C64D(dev, 0xA0); 
			break;

		case EEPROM_TYPE_BL24C64: 
			eeprom = new BL24C64(dev, 0xA0); 
			break;
		case EEPROM_TYPE_CAT24C64:
			eeprom = new CAT24C64(dev, 0xA0); 
			break;
		case EEPROM_TYPE_24P64: 
			eeprom = new EE24P64(dev, 0xA0); 
			break;		
		case EEPROM_TYPE_GT9767: 
			eeprom = new EEGT9767(dev, 0xB0); 
			break;
		case EEPROM_TYPE_M24C64: 
			eeprom = new M24C64(dev, 0xA8); 
			break;
		case EEPROM_TYPE_Virtual:
		default:
			uts.log.Error(_T("Not defined eeprom[%d], Use VirtualEEProm As Default"),type);
			eeprom = new VirtualEEProm(dev, 0x00);
			break;
		}
		eeprom->SetDevice(dev);
		eeprom->eepromType = type;
		return eeprom;
	}

	EEPROMDriver *GetEEPromDriverInstance(BaseDevice *dev, int eepromType)
	{
	    //int eepromType = CHVSBase::INIReadIntSensor("Project_Setting", "EEPromType", -1);
	    if (eepromType < 0 || eepromType >= EEPROM_TYPE_SIZE) {
	        uts.log.Error(_T("No eeprom device!"));
	        return NULL;
	    }
	    EEPROM_TYPE type = (EEPROM_TYPE)eepromType;  
	    g_pEEPROM = CreateEEProm(type, dev);
	   

		g_nEEPROMType = type;

	    return g_pEEPROM;
	}

// 	#define DEV_MAX_NUM	4
// 	EEPROM_Driver* GetEEPromDriverInstance(BaseDevice *dev, int eepromType, int i2cAddr)
// 	{
// 		//int eepromType = CHVSBase::INIReadIntSensor("Project_Setting", "EEPromType", -1);
// 		if (eepromType < 0 || eepromType >= EEPROM_TYPE_SIZE) {
// 			uts.log.Error(_T("No eeprom device!"));
// 			return NULL;
// 		}
// 		int usbid = dev->get_usbid();
// 		if (usbid >= DEV_MAX_NUM) return nullptr;
// 
// 		EEPROM_TYPE type = (EEPROM_TYPE)eepromType;
// 
// 		static EEPROM_Driver *eeprom[DEV_MAX_NUM] = {nullptr};
// 		if (eeprom[usbid] != nullptr) return eeprom[usbid];
// 
// 		eeprom[usbid] = CreateEEProm(type, dev, i2cAddr);
// 		return eeprom[usbid];
// 	}

	//-----------------------------------------------------------------------------
	BOOL EEPROMDriver::I2CWrite(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf)
	{
		unsigned char writeBuffer[256] = {0};
		int nUseSize = 0;

		/*
		id      type        supportDevice       vallue
		=================================================
		1       (*1)        V5U                 I2C_WRITE
		(*1):
			Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
			Output Format: [Not Use]
		*/
		memcpy(writeBuffer + nUseSize, &slAddr, sizeof(BYTE));
		nUseSize += sizeof(BYTE);
		memcpy(writeBuffer + nUseSize, &nAddrLen, sizeof(DWORD));
		nUseSize += sizeof(DWORD);
		memcpy(writeBuffer + nUseSize, &nAddr, sizeof(DWORD));
		nUseSize += sizeof(DWORD);
		memcpy(writeBuffer + nUseSize, &nCnt, sizeof(DWORD));
		nUseSize += sizeof(DWORD);
		memcpy(writeBuffer + nUseSize, buf, sizeof(unsigned char) * nCnt);
		nUseSize += sizeof(unsigned char) * nCnt;

		return pdev->WriteValue(eDeviceWriteValueType::DWVT_I2C_WRITE, writeBuffer, nUseSize);
	}

	BOOL EEPROMDriver::I2CRead(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf)
	{
		unsigned char writeBuffer[512] = {0};
		int nUseSize = 0;

		/*
		id      type        supportDevice       vallue
		=================================================
		15      (*5)        V5U                 I2C_READ
		15 (*5):
		Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
		Output Format: [DWORD|nCnt][unsigned char[?]|buf]
		*/
		memcpy(writeBuffer + nUseSize, &slAddr, sizeof(BYTE));
		nUseSize += sizeof(BYTE);
		memcpy(writeBuffer + nUseSize, &nAddrLen, sizeof(DWORD));
		nUseSize += sizeof(DWORD);
		memcpy(writeBuffer + nUseSize, &nAddr, sizeof(DWORD));
		nUseSize += sizeof(DWORD);
		memcpy(writeBuffer + nUseSize, &nCnt, sizeof(DWORD));
		nUseSize += sizeof(DWORD);
		memcpy(writeBuffer + nUseSize, buf, sizeof(unsigned char) * nCnt);
		nUseSize += sizeof(unsigned char) * nCnt;

		if (!pdev->ReadValue(eDeviceReadValueType::DRVT_I2C_READ, writeBuffer, nUseSize))
		{
			return FALSE;
		}

		DWORD dwReadCnt = 0;
		memcpy(&dwReadCnt, writeBuffer, sizeof(DWORD));
		if (dwReadCnt != nCnt)
		{
			return FALSE;
		}
		memcpy(buf, writeBuffer + sizeof(DWORD), sizeof(unsigned char) * dwReadCnt);

		return TRUE;
	}


	BOOL EEPROMDriver::RegWrite(WORD wRegAddr, WORD wRegData)
	{
		unsigned char writeBuffer[256] = {0};
		int nUseSize = 0;

		/*
		id      type        supportDevice       vallue
		=================================================
		2       (*2)        V5U                 REG_WRITE
		(*2):
			Input  Format: [WORD|wRegAddr][WORD|wRegData]
			Output Format: [Not Use]
			*/
		memcpy(writeBuffer + nUseSize, &wRegAddr, sizeof(WORD));
		nUseSize += sizeof(WORD);
		memcpy(writeBuffer + nUseSize, &wRegData, sizeof(WORD));
		nUseSize += sizeof(WORD);

		return pdev->WriteValue(eDeviceWriteValueType::DWVT_REG_WRITE, writeBuffer, nUseSize);
	}

	BOOL EEPROMDriver::Power(int idx,float volt)
	{
		unsigned char writeBuffer[256] = {0};
		int nUseSize = 0;

		memcpy(writeBuffer + nUseSize, &idx, sizeof(int));
		nUseSize += sizeof(int);
		memcpy(writeBuffer + nUseSize, &volt, sizeof(float));
		nUseSize += sizeof(float);
		return pdev->WriteValue(eDeviceWriteValueType::DWVT_SET_POWER_PIN,writeBuffer, nUseSize);
	}
}

