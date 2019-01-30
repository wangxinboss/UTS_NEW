#include "StdAfx.h"
#include "VirtualSensor.h"
#include "CommonFunc.h"

namespace UTS
{

    VirtualSensor::VirtualSensor(void)
    {
    }


    VirtualSensor::~VirtualSensor(void)
    {
    }

    int VirtualSensor::SetVcmMove(__in int nVCM_CurrentDAC)
    {
        printk(_T("Virtual Sensor move VCM to Dac[%d]"), nVCM_CurrentDAC);
        return 0;
    }

    BOOL VirtualSensor::GetSensorId(__out CString &strSensorId)
    {
        static int id = 0;
        id++;
        strSensorId.Format(_T("%08d"), id);
        printk(_T("Get Virtual Sensor id[%s]"), strSensorId);

        return FALSE;
    }
}
