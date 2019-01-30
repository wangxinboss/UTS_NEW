#include "StdAfx.h"
#include "DspFullboom.h"



DspFullboom::DspFullboom(void)
{
	m_devinfo.device_list = NULL;
	m_devinfo.cur_index = m_devinfo.device_num = 0;
	m_devinfo.device_list = new CStringList;//NULL;
	HRESULT result = CheckDeviceInfo(&m_devinfo);
	if(result != S_OK)
	{
		AfxMessageBox( _T("Set Device Setting Fail."), MB_ICONERROR);
	}
}


DspFullboom::~DspFullboom(void)
{
}

HRESULT DspFullboom::DspGetSensorRegister(USHORT *Value, IN int Address,BYTE flag)
{
	HRESULT hr = GetSensorRegister(Value, Address,flag);
	return hr;
}

HRESULT DspFullboom::DspSetSensorRegister(USHORT Value, IN int Address,BYTE flag)
{
	HRESULT hr = SetSensorRegister(Value, Address, flag);
	return hr;
}

HRESULT DspFullboom::DspReadEEPROM(BYTE *Data, IN int StartAddress, IN int ReadLen)
{
	HRESULT hr = ReadEEPROM(Data, StartAddress, ReadLen);
	return hr;
}

HRESULT DspFullboom::DspWriteEEPROM(BYTE *Data, IN int StartAddress, IN int WriteLen)
{
	HRESULT hr =  WriteEEPROM(Data, StartAddress, WriteLen);
	return hr;
}

HRESULT DspFullboom::DspSetLEDValue(int Value)
{
	HRESULT hr = S_OK;
	switch(Value)
	{
	case 1:
		hr = SetLEDOn();
		break;
	case 2:
		hr = SetLEDOff();
		break;
	default:
		hr = -1;
		break;
	}
	return hr;
}

