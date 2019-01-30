#pragma once

#ifndef REALTEKUSER
#define REALTEKUSER

class CUser
{
public:
	CUser(void);
	~CUser(void);
};

void DownloadProgressCB(PVOID Context);

enum EEPROM_ID
{
	SPI_EEP_AT25640A=1,
	SPI_SF_MX25L512,SPI_SF_EN25F05,SPI_SF_SST25VF512,SPI_SF_PM25LV512,
	TL_EEP_AT93C46,TL_EEP_AT93C56,TL_EEP_AT93C66,TL_EEP_AT93C86,
	I2C_EEP_M24C64,I2C_EEP_M24128,I2C_EEP_24LC04B,I2C_EEP_24LC64,
	I2C_EEP_24AA64,I2C_EEP_24AA128,I2C_EEP_AT24C64,I2C_EEP_AT24C128,
	I2C_EEP_CAT24C32,I2C_EEP_24LC32,I2C_EEP_CAT24C64,I2C_EEP_CAT24C16,
	MTP_ID, SPI_SF_MLX128, OTP_ID, I2C_EEP5806AL0, I2C_EEP5806AL1
};

enum EEPROMTYPE
{
	THREEL_EEP128=1, THREEL_EEP256, THREEL_EEP512, THREEL_EEP2048, I2C_EEP, I2C_EEP16, SPI_EEP, SPI_SF, MTP_TYPE, OTP_TYPE, I2C_EEP16AL0,
	I2C_EEP16AL1
};

struct _tag_ProgressInfo;
typedef struct _tag_ProgressInfo ProgressInfo;

typedef void (*PfnDownloadProgressCB)(PVOID Context);
typedef void (* PfnProgressCB)(PVOID Context, UINT uPosIncrement);
typedef BOOL (*pfn_SGBufferCB)(BYTE * pBuffer, long& BufferLen);

struct _tag_ProgressInfo 
{
	PVOID Context;
	UINT nStage;
	UINT nTotal;
	UINT nProgress;
	BOOL bCancel;
	PfnDownloadProgressCB pSelfCB;
};

typedef BOOL    (*pfn_RvcLib_Initialize)(BOOL bInternalLog, BOOL bLibType);
typedef BOOL    (*pfn_RvcLib_UnInitialize)(BOOL bLibType);
typedef BOOL    (*pfn_ReadSensorRegister8)(WORD wRegAddr, UINT uLen, BYTE* byRegData);
typedef BOOL    (*pfn_WriteSensorRegister8)(WORD wRegAddr, UINT uLen, BYTE* byRegData);
typedef BOOL    (*pfn_GetAsicRegister)(WORD wRegAddr, UINT uLen, BYTE* byRegData);
typedef BOOL    (*pfn_SetAsicRegister)(WORD wRegAddr, UINT uLen, BYTE* byRegData);
typedef LRESULT (*pfn_EEPROMRead)(EEPROM_ID id, DWORD dwAddress, DWORD dwLength, LPBYTE pData, PfnDownloadProgressCB pfnProgressCB, ProgressInfo* progressInfo);
typedef LRESULT (*pfn_EEPROMWrite)(EEPROM_ID EepromID, DWORD dwLength, LPBYTE pData, PfnDownloadProgressCB pfnProgressCB, ProgressInfo* progressInfo);
typedef BOOL    (*pfn_SetLEDMode)(BOOL bLedOn);
typedef BOOL    (*pfn_SetCameraStatus)(BOOL bStandBy);
typedef LRESULT (*pfn_EEPROMWriteSN)(EEPROM_ID EepromID, DWORD dwLength, LPBYTE pData, CString strSN, PfnDownloadProgressCB pfnProgressCB, ProgressInfo* progressInfo);
typedef BOOL    (*pfn_UVCGetManufacturer)(CString& strManufactor);

 #endif