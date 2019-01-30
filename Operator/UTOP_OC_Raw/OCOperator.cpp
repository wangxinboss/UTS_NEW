#include "StdAfx.h"
#include "OCOperator.h"
#include "UTOP_OC_Raw.h"
#include "otpdb.h"
#include "CommonFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "CommonFunc.lib")

namespace UTS
{
    OCOperator::OCOperator(void)
    {
        OPERATOR_INIT;
    }

    OCOperator::~OCOperator(void)
    {
    }

    BOOL OCOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nPixelOffsetSpec"), m_param.nPixelOffsetSpec, 70, _T("Pixel Offset from image center spec"));
  		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCommitData_En"), m_param.nCommit_En, 0, _T("0: Disable 1:Enable"));
        return TRUE;
    }

    BOOL OCOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        m_dYvalue = 0.0;
        memset(m_Offset,0,sizeof(double)*4);
        memset(m_ptOcCenter,0,sizeof(POINT)*4);

        //------------------------------------------------------------------------------
        // 抓图
        if (m_param.nReCapture != 0)
        {
            // 重新设定Sensor序列
            CString strRegName = m_strOperatorName;
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
        }
        else
        {
            // 使用上次的抓图
            m_pDevice->GetCapturedBuffer(m_bufferObj);
        }

        // 画图
        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

        //------------------------------------------------------------------------------
        // 判断画面平均亮度
        UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
        if (m_dYvalue < m_param.dLTMinY || m_dYvalue > m_param.dLTMaxY)
        {
            *pnErrorCode = uts.errorcode.E_Linumance;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 测试
		int nOutMode = 0;

		m_pDevice->ReadValue(eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
			                 &nOutMode, sizeof(nOutMode));

		OpticalCenterRAW(m_bufferObj.pRaw8Buffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, nOutMode,m_ptOcCenter);
		
		for (int i = 0 ;i<4 ;i++)
		{
			m_Offset[i] = Algorithm::Math::GetDistance2D(
				m_ptOcCenter[i].x,
				m_ptOcCenter[i].y,
				m_bufferInfo.nWidth / 2,
				m_bufferInfo.nHeight / 2);
		
		}

        //------------------------------------------------------------------------------
        // 判断规格

		*pnErrorCode = uts.errorcode.E_Pass;

		uint8_t tmp[16];

		for (int i = 0 ; i< 4 ;i++)
		{
			put_le_val( m_ptOcCenter[i].x, tmp + 4*i , 2);
			put_le_val( m_ptOcCenter[i].y, tmp + 4*i +2, 2);

			if (m_Offset[i] >  m_param.nPixelOffsetSpec)
			{
				*pnErrorCode = uts.errorcode.E_OC;
			}
		}
        
		if(*pnErrorCode == uts.errorcode.E_Pass && m_param.nCommit_En == 1)
        {	
			USES_CONVERSION;

			int ret = 0;
			uts.otpdb->get_instance();

			if (is_otp_data_locked(uts.otpdb->GetModuleID(T2A(uts.info.strSN))) == FALSE)
			{
				ret = uts.otpdb->UpdateOtpByType(uts.otpdb->GetModuleID(T2A(uts.info.strSN)), 14, (char *)tmp, sizeof(tmp));
				if (ret < 0)
				{
					uts.log.Error(_T("update OC  data error!!!"));
				}
			}else
			{
				uts.log.Debug(_T("OC  data omit!!!"));
			}
			
        }
		
end:
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        //------------------------------------------------------------------------------
        // 保存图片文件
        if (m_param.nReCapture != 0)
        {
            SaveImage();
        }

        //------------------------------------------------------------------------------
        // 保存数据文件
        SaveData();

        return m_bResult;
    }

    void OCOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_OC);
    }

    void OCOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void OCOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),Y_Avg,OC_Result,")
            _T("OC_OFFECT_R,CenterR_X,CenterR_Y,")
			_T("OC_OFFECT_Gr,CenterR_Gr,CenterR_Gr,")
			_T("OC_OFFECT_Gb,CenterR_Gb,CenterR_Gb,")
			_T("OC_OFFECT_B,CenterR_B,CenterR_B,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%.1f,%s,")
			_T("%.2f,")
            _T("%d,%d,")
			_T("%.2f,")
			_T("%d,%d,")
			_T("%.2f,")
			_T("%d,%d,")
			_T("%.2f,")
			_T("%d,%d,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
            , m_Offset[0]
			, m_ptOcCenter[0].x, m_ptOcCenter[0].y
			, m_Offset[1]
			, m_ptOcCenter[1].x, m_ptOcCenter[1].y
			, m_Offset[2]
			, m_ptOcCenter[2].x, m_ptOcCenter[2].y
			, m_Offset[3]
			, m_ptOcCenter[3].x, m_ptOcCenter[3].y
            , strVersion, uts.info.strUserId);
    }

	BOOL OCOperator::is_otp_data_locked(int module_id)
	{
		time_t time;
		if (uts.otpdb->get_otp_data_lock_time(module_id, &time) < 0)
		{
			return is_otp_programed(module_id);
		}
		return (time > 0 ? TRUE : FALSE);
	}

	BOOL OCOperator::is_otp_programed(int module_id)
	{
		time_t time;
		if (uts.otpdb->GetOtpBurnTime(module_id, &time) < 0)
		{
			return FALSE;
		}
		return (time > 0 ? TRUE : FALSE);
	}

    
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new OCOperator);
    }
    //------------------------------------------------------------------------------
}
