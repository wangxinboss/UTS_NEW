#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_DataLose.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif


namespace UTS
{
	//-------------------------------------------------------------------------
	typedef union
	{
		unsigned char Value;
		struct
		{
			unsigned char b0:1;
			unsigned char b1:1;
			unsigned char b2:1;
			unsigned char b3:1;
			unsigned char b4:1;
			unsigned char b5:1;
			unsigned char b6:1;
			unsigned char b7:1;
		};
	}
	ByteMap;

	bool DataLoseAnalysis(const unsigned char *pBmpBuffer,int nWidth,int nHeight,double DataLoseRate,double *DataPinValue,int *DataPinOS)
	{
		bool bRes = true;

		memset(DataPinValue,0,sizeof(double)*8);
		memset(DataPinOS,0,sizeof(int)*8);

		ByteMap iByte;iByte.Value = 0;

		for (int y = 0 ; y < nHeight ; y++)
		{
			for (int x = 0 ; x< nWidth ; x++)
			{				
				iByte.Value = pBmpBuffer[y*nWidth*3+x*3+1];
				DataPinValue[0] += iByte.b0;
				DataPinValue[1] += iByte.b1;
				DataPinValue[2] += iByte.b2;
				DataPinValue[3] += iByte.b3;
				DataPinValue[4] += iByte.b4;
				DataPinValue[5] += iByte.b5;
				DataPinValue[6] += iByte.b6;
				DataPinValue[7] += iByte.b7;
			}
		}

		for (int i=0;i<8;i++)
		{
			if (DataPinValue[i] > nWidth*nHeight*3*(1-DataLoseRate))
			{
				DataPinOS[i] = 1;
				bRes = false;
			}
			if (DataPinValue[i] < nWidth*nHeight*3*(DataLoseRate))
			{
				DataPinOS[i] = -1;
				bRes = false;
			}
		}
	//	uts.log.Error(_T("DataLose Algorithm Result = %d",bRes));
		return bRes;
	}
	//-------------------------------------------------------------------------
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;
		memset(&m_result,0,sizeof(OPERATOR_RESULT));
    }

    ImplOperator::~ImplOperator(void)
    {
    }

    BOOL ImplOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDataLoseRate"), m_param.dDataLoseRateSpec, 0.3, _T("DataLoseRate"));

        return TRUE;
    }

 
    
    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        UI_MARK uiMark;
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        ;

        // 重新设定Sensor序列
        CString strRegName = _T("Color Level Mode");
        if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
            strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
        {
            uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
            *pnErrorCode = uts.errorcode.E_Fail;
            goto end;
        }

        // 抓图
        if (!m_pDevice->Recapture(
            m_bufferObj,
            uts.info.nLTDD_DummyFrame,
            uts.info.nLTDD_AvgFrame))
        {
            uts.log.Error(_T("Recapture error. "));
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 测试
		m_result.DataPin = true;
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
			m_result.DataPin = DataLoseAnalysis(
				m_bufferObj.pBmpBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_param.dDataLoseRateSpec,
				m_result.DataPinValue,
				m_result.DataPinOS);
        }
        // 画图
        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);


		strRegName = _T("Normal Mode");
		if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_WRITE_SENSOR_REGISTER,
			strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
		{
			uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}

        //------------------------------------------------------------------------------
        // 判断规格
        if (m_result.DataPin == false)
        {
			*pnErrorCode = uts.errorcode.E_OpenShort;
			uts.log.Error(_T("uts.errorcode.E_OpenShort"));
            goto end;
        }

        *pnErrorCode = uts.errorcode.E_Pass;

end:


        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        //------------------------------------------------------------------------------
        // 保存图片文件
        SaveImage();

        //------------------------------------------------------------------------------
        // 保存数据文件
        SaveData();
        
        return m_bResult;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
		vecReturnValue.push_back(uts.errorcode.E_OpenShort);
    }

    void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(_T("Color Level Mode"));
		vecRegister.push_back(_T("Normal Mode"));
    }

    void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        //-------------------------------------------------------------------------
        // Head
        strHeader = _T("Time,SN,SensorID,DeviceIndex,TestTime(ms),Result,");
        strData.Format(_T("%s,%s,%s,%d,%.1f,%s,"),
            lpTime,
            uts.info.strSN,
            uts.info.strSensorId,
            uts.info.nDeviceIndex,
            m_TimeCounter.GetPassTime(),
            strResult);

			strHeader.Append(_T("Pin0,Pin1,Pin2,Pin3,Pin4,Pin5,Pin6,Pin7,"));
			strData.AppendFormat(_T("%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,"), 
				m_result.DataPinValue[0],
				m_result.DataPinValue[1],
				m_result.DataPinValue[2],
				m_result.DataPinValue[3],
				m_result.DataPinValue[4],
				m_result.DataPinValue[5],
				m_result.DataPinValue[6],
				m_result.DataPinValue[7]);
        
        //-------------------------------------------------------------------------
        // Tail
        strHeader.Append(_T("Version,OP_SN\n"));
        strData.AppendFormat(_T("%s,%s\n"),
            strVersion,
            uts.info.strUserId);
    }
    
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
