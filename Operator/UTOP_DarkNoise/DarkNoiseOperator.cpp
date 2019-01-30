#include "StdAfx.h"
#include "DarkNoiseOperator.h"
#include "UTOP_DarkNoise.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    DarkNoiseOperator::DarkNoiseOperator(void)
    {
        OPERATOR_INIT;
    }

    DarkNoiseOperator::~DarkNoiseOperator(void)
    {
    }

    BOOL DarkNoiseOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDDMaxY"), m_param.dDDMaxY, 20.0, _T("Max Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
	
		CString strValue;
		vector<int> vecValue;
		vector<double> vecDoubleValue;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("TestROI"), strValue, _T("432,225,48,48"), _T("参考ROI(左,上,宽,高)"));
		SplitInt(strValue, vecValue);
		m_param.TestROI.left = vecValue[0];
		m_param.TestROI.top = vecValue[1];
		m_param.TestROI.right = vecValue[0] + vecValue[2];
		m_param.TestROI.bottom = vecValue[1] + vecValue[3];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Spec.dNoise"), strValue, _T("0,10"), _T("NoiseSpec(Min,Max)"));
		SplitInt(strValue, vecValue);
		m_param.dNoiseSpec.min = vecValue[0];
		m_param.dNoiseSpec.max = vecValue[1];

		vecValue.clear();
		vecDoubleValue.clear();

        return TRUE;
    }

	BOOL DarkNoiseOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_POINT pt;
		UI_TEXT text;
		UI_TEXT_REL textRel;
		UI_LINE line;

		// ref black
		rc.color = text.color = (COLOR_BLUE );
		memcpy(&rc.rcPos, &m_param.TestROI, sizeof(RECT));
		uiMark.vecUiRect.push_back(rc);

		return TRUE;
	}

    BOOL DarkNoiseOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        m_dYvalue = 0.0;
        m_dStdDevY = 0.0;

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

  
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
			int nROIWidth = m_param.TestROI.right - m_param.TestROI.left;
			int nROIHeight = m_param.TestROI.bottom - m_param.TestROI.top;

			int nChartROIBufferSize = nROIWidth * nROIHeight;
			unsigned char *pChartROIBmpBuffer = new unsigned char[nChartROIBufferSize * 3];

			CImageProc::GetInstance().GetBMPBlockBuffer(
				m_bufferObj.pBmpBuffer,pChartROIBmpBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_param.TestROI.left, m_param.TestROI.top, nROIWidth, nROIHeight);

			//------------------------------------------------------------------------------
			// 判断画面平均亮度
			UTS::Algorithm::CalYavg( pChartROIBmpBuffer,
				nROIWidth, nROIHeight, m_dYvalue);
			if (m_dYvalue > m_param.dDDMaxY)
			{
				*pnErrorCode = uts.errorcode.E_Linumance;
				goto end;
			}

			//------------------------------------------------------------------------------

            DarkNoise_WholeImage::DarkNoiseRGB(
                pChartROIBmpBuffer,
                nROIWidth, nROIHeight,
                m_dStdDevY);

			RELEASE_ARRAY(pChartROIBmpBuffer);
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 判断规格
        if (m_dStdDevY< m_param.dNoiseSpec.min || m_dStdDevY > m_param.dNoiseSpec.max)
        {
            *pnErrorCode = uts.errorcode.E_SNR;
        }
        else
        {
            *pnErrorCode = uts.errorcode.E_Pass;
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

    void DarkNoiseOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_DarkNoise); //tofix
    }

    void DarkNoiseOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void DarkNoiseOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),Y_Avg,Noise_Result,")
			_T("Std,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%.1f,%s,")
			 _T("%.2f,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
			,m_dStdDevY
            , strVersion, uts.info.strUserId);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new DarkNoiseOperator);
    }
    //------------------------------------------------------------------------------
}
