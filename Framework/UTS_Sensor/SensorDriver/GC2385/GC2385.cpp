#include "StdAfx.h"
#include "GC2385.h"
#include "CommonFunc.h"

GC2385::GC2385(void):GCSensor()
{
	name = _T("GC2385");
	sensorType = Sensor_GC2385;
	//sid_len = 5;
}


GC2385::~GC2385(void)
{
}

int GC2385::do_prog_otp(int page, int addr, const void *data, int len)
{

	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}

int GC2385::do_read_otp(int page, int addr, void *data, int len)
{
	return SET_ERROR(SENSORDRIVER_ERROR_NO);
}