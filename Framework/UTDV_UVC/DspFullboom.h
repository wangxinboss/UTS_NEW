#pragma once
#include "basedsp.h"
#include "Etlib.h"

class DspFullboom :
	public BaseDsp
{
public:
	DspFullboom(void);
	virtual ~DspFullboom(void);

	virtual HRESULT DspGetSensorRegister(USHORT *Value, IN int Address,BYTE flag);
	virtual HRESULT DspSetSensorRegister(USHORT Value, IN int Address,BYTE flag);

	virtual HRESULT DspReadEEPROM(BYTE *Data, IN int StartAddress, IN int ReadLen);
	virtual HRESULT DspWriteEEPROM(BYTE *Data, IN int StartAddress, IN int WriteLen);

	virtual HRESULT DspSetLEDValue(int Value);
protected:
	ETDEV_INFO m_devinfo;
};

