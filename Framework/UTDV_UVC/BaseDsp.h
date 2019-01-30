#pragma once
class BaseDsp
{
public:
	BaseDsp(void);
	virtual ~BaseDsp(void);

	virtual HRESULT DspGetSensorRegister(USHORT *Value, IN int Address,BYTE flag){return S_OK;}
	virtual HRESULT DspSetSensorRegister(USHORT Value, IN int Address,BYTE flag){return S_OK;}

	virtual HRESULT DspReadEEPROM(BYTE *Data, IN int StartAddress, IN int ReadLen){return S_OK;}
	virtual HRESULT DspWriteEEPROM(BYTE *Data, IN int StartAddress, IN int WriteLen){return S_OK;}

	virtual HRESULT DspSetLEDValue(int Value){return S_OK;}

	virtual HRESULT DspLoadFW_USBOTP(LPBYTE lpData,int eepromsize){return S_OK;}
	virtual HRESULT DspLoadFW_EEProm(LPBYTE lpData,int eepromsize){return S_OK;}
	virtual HRESULT DspDumpFW_USBOTP(LPBYTE lpData,int eepromsize){return S_OK;}
	virtual HRESULT DspDumpFW_EEProm(LPBYTE lpData,int eepromsize){return S_OK;}

	virtual HRESULT DspWriteSN_EEProm(LPBYTE lpData,CString strSN,int eepromsize){return S_OK;}
	virtual HRESULT DspWriteSN_USBOTP(LPBYTE lpData,CString strSN){return S_OK;}
	virtual HRESULT DspReadSN(CString &strSN){return S_OK;}
};

