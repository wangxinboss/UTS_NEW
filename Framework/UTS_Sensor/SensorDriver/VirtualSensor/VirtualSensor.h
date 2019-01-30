#pragma once
#include "SensorDriver.h"

namespace UTS
{
    class VirtualSensor :
        public SensorDriver
    {
    public:
        VirtualSensor(void);
        ~VirtualSensor(void);

        virtual int SetVcmMove(__in int nVCM_CurrentDAC);
        virtual BOOL GetSensorId(__out CString &strSensorId);

        virtual int do_prog_otp(int page, int addr, const void *data, int len) { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT); }
        virtual int do_read_otp(int page, int addr, void *data, int len) { return SET_ERROR(SENSORDRIVER_ERROR_NOTSUPPORT); }

    };

}
