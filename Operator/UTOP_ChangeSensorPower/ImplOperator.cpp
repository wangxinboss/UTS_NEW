#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_ChangeSensorPower.h"
#include "CommonFunc.h"

#pragma comment(lib, "CommonFunc.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;
    }

    ImplOperator::~ImplOperator(void)
    {
    }

    BOOL ImplOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        
		CString strComment,strValue;
		vector<double> vecValue;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("fltCode"), strValue, _T("2.8,1.8,1.05,2.8,3"), 
										_T("(dPowerPin1(AVDD),dPowerPin2(DOVDD),dPowerPin3(DVDD),dPowerPin4(AFVDD),dPowerPin35"));
		SplitDouble(strValue, vecValue);
		m_param.dSensor_PowerVolt[0] = vecValue[0];
		m_param.dSensor_PowerVolt[1] = vecValue[1];
		m_param.dSensor_PowerVolt[2] = vecValue[2];
		m_param.dSensor_PowerVolt[3] = vecValue[3];
		m_param.dSensor_PowerVolt[4] = vecValue[4];
		vecValue.clear();

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_VCLK")   , m_param.nSensor_VCLK   , 0, _T("Sensor VCLK"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_VSYNC"), m_param.nSensor_VSYNC, 0, _T("Sensor VSYNC"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_RESET") , m_param.nSensor_RESET  , 0, _T("Sensor RESET"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_ENB")    , m_param.nSensor_ENB      , 0, _T("Sensor ENB"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_SlaveAddr") , m_param.nSensor_SlaveAddr, 0, _T("Sensor Slave Addr"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_ImageSizeX"), m_param.nSensor_ImageSizeX, 0, _T("Sensor Image Size X"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_ImageSizeY"), m_param.nSensor_ImageSizeY, 0, _T("Sensor Image Size Y"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_DataFormat"), m_param.nSensor_DataFormat, 0, _T("0:YUV, 1:8bit Bayer 2: 10bit Bayer"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_OutputMode"), m_param.nSensor_OutputMode, 0, _T("1:BGGR  2:RGGB  3:GBRG  4:GRBG  5:BlackWhite"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_MIPIMode"), m_param.nSensor_MIPIMode, 0, _T("0:1L, 1:2L, 3:3L, 7:4L"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensor_I2CMode"), m_param.nSensor_I2CMode, 0, _T("0:8_8, 1:8_16, 2:16_8, 3:16_16"));


		return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
		unsigned char writeBuffer[256] = {0};

		int nUseSize = 0;
		memset(writeBuffer,0,sizeof(unsigned char)*256);
		//Reset Power Volt
		for(int i = 0 ; i< 5;i
			++)
		{
			memcpy(writeBuffer + nUseSize, &m_param.dSensor_PowerVolt[i], sizeof(double));
			nUseSize += sizeof(double);
		}
        
		m_pDevice->WriteValue(
			eDeviceWriteValueType::DWVT_RESET_POWER_PIN,
			writeBuffer, nUseSize);

		//Reset Power Sequence
		nUseSize = 0;
		memset(writeBuffer,0,sizeof(unsigned char)*256);

		memcpy(writeBuffer + nUseSize, &m_param.nSensor_VCLK, sizeof(int));
		nUseSize += sizeof(int);
		memcpy(writeBuffer + nUseSize, &m_param.nSensor_VSYNC, sizeof(int));
		nUseSize += sizeof(int);
		memcpy(writeBuffer + nUseSize, &m_param.nSensor_ENB, sizeof(int));
		nUseSize += sizeof(int);
		memcpy(writeBuffer + nUseSize, &m_param.nSensor_RESET, sizeof(int));
		nUseSize += sizeof(int);
		m_pDevice->WriteValue(
			eDeviceWriteValueType::DWVT_RESET_POWERSEQUENCE_PIN,
			writeBuffer, nUseSize);

		//Change Slave Addr
		m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_SENSOR_SLAV_ADDR,
			&m_param.nSensor_SlaveAddr, sizeof(int));
		
		//Change OuptMode: 1:BGGR  2:RGGB  3:GBRG  4:GRBG  5:BlackWhite
		m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_SENSOR_OUT_MODE,
			&m_param.nSensor_OutputMode, sizeof(int));

		//DataFormat : 0:YUV, 1:8bit Bayer 2: 10bit Bayer
		m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_RESET_DATAFORMAT,
			&m_param.nSensor_DataFormat, sizeof(int));

		//MIPIMODE : 0:1L, 1:2L, 3:3L, 7:4L
		m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_RESET_MIPIMode,
			&m_param.nSensor_MIPIMode, sizeof(int));

		//I2CMODE : 0:8_8, 1:8_16, 2:16_8, 3:16_16
		m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_RESET_I2CMode,
			&m_param.nSensor_I2CMode, sizeof(int));

		//Change Image SizeX,Y
		nUseSize = 0;
		memset(writeBuffer,0,sizeof(unsigned char)*256);
		memcpy(writeBuffer + nUseSize, &m_param.nSensor_ImageSizeX, sizeof(int));
		nUseSize += sizeof(int);
		memcpy(writeBuffer + nUseSize, &m_param.nSensor_ImageSizeY, sizeof(int));
		nUseSize += sizeof(int);
		m_pDevice->WriteValue(
			eDeviceWriteValueType::DWVT_RESET_IMAGESIZE,
			writeBuffer, nUseSize);

        return TRUE;
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
