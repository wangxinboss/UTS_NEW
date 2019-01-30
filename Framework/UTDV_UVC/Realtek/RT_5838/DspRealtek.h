#pragma once

#include "basedsp.h"
#include "../RvcLibFun.h"
#include <list>

class DspRealtek_5838 :
	public BaseDsp
{
public:
	DspRealtek_5838(void);
	virtual ~DspRealtek_5838(void);

	virtual HRESULT DspGetSensorRegister(USHORT *Value, IN int Address,BYTE flag);
	virtual HRESULT DspSetSensorRegister(USHORT Value, IN int Address,BYTE flag);

	virtual HRESULT DspReadEEPROM(BYTE *Data, IN int StartAddress, IN int ReadLen);
	virtual HRESULT DspWriteEEPROM(BYTE *Data, IN int StartAddress, IN int WriteLen);

	virtual HRESULT DspSetLEDValue(int Value);

	virtual HRESULT DspLoadFW_EEProm(LPBYTE lpData,int eepromsize);
	virtual HRESULT DspDumpFW_EEProm(LPBYTE lpData,int eepromsize);

	virtual HRESULT DspWriteSN_EEProm(LPBYTE lpData,CString strSN,int eepromsize);
	virtual HRESULT DspReadSN(CString &strSN);

protected:
	CRvcLibFun m_RvcLib;
};

