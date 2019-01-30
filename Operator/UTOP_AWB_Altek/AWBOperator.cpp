#include "StdAfx.h"
#include "AWBOperator.h"
#include "UTOP_AWB_Altek.h"
//#include "Global_Memory_SFC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    AWBOperator::AWBOperator(void)
    {
        OPERATOR_INIT;
    }

    AWBOperator::~AWBOperator(void)
    {
    }

    BOOL AWBOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dThreshold"), m_param.dThreshold, 0.15, _T("Defect Threshold"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nROIWidth"), m_param.nROIWidth, 32, _T("ROI width(pixel count)"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nROIHeight"), m_param.nROIHeight, 32, _T("ROI height(pixel count)"));

        return TRUE;
    }

    BOOL AWBOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
		m_dYvalue = 0.0;
		memset(&m_AWBInfo, 0, sizeof(AWBInfo));
        //------------------------------------------------------------------------------
        if (m_param.nReCapture != 0)
        {
            CString strRegName = m_strOperatorName;
            if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
                strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
            {
                uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }

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
            m_pDevice->GetCapturedBuffer(m_bufferObj);
        }

        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

        //------------------------------------------------------------------------------

        UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
        if (m_dYvalue < m_param.dLTMinY || m_dYvalue > m_param.dLTMaxY)
        {
            *pnErrorCode = uts.errorcode.E_Linumance;
            goto end;
        }

        //------------------------------------------------------------------------------
 
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
			AWBY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_param.nROIWidth,m_param.nROIHeight,m_AWBInfo);
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        if (m_AWBInfo.dRGDelta > m_param.dThreshold
         || m_AWBInfo.dBGDelta > m_param.dThreshold)
        {
            *pnErrorCode = uts.errorcode.E_AWB;
        }
        else
        {
            *pnErrorCode = uts.errorcode.E_Pass;
        }
end:

        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        //------------------------------------------------------------------------------

        if (m_param.nReCapture != 0)
        {
            SaveImage();
        }

        //------------------------------------------------------------------------------
        SaveData();

        return m_bResult;
    }

    void AWBOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_AWB);
    }

    void AWBOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void AWBOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),Y_Avg,AWB_Result,")
            _T("RGRatio_UL,BGRatio_UL,RGRatio_UR,BGRatio_UR,RGRatio_BL,BGRatio_BL,RGRatio_BR,BGRatio_BR,RGRatio_Mean,BGRatio_Mean,DeltaRG,DeltaBG,")
            _T("Version,OP_SN\n");

		double RGMean=0.0;
		double BGMean=0.0;
		double RGSum=0.0;
		double BGSum=0.0;
		for (int i=0;i<4;i++)
		{
			RGSum += m_AWBInfo.dRG[i];
			BGSum += m_AWBInfo.dBG[i];
		}
		RGMean=RGSum/4;
		BGMean=BGSum/4;

        strData.Format(
            _T("%s,%s,%.1f,%.1f,%s,")
            _T("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
			, m_AWBInfo.dRG[0]
			, m_AWBInfo.dBG[0]
			, m_AWBInfo.dRG[1]
			, m_AWBInfo.dBG[1]
			, m_AWBInfo.dRG[2]
			, m_AWBInfo.dBG[2]
			, m_AWBInfo.dRG[3]
			, m_AWBInfo.dBG[3]
			, RGMean
			, BGMean
			, m_AWBInfo.dRGDelta
			, m_AWBInfo.dBGDelta
            , strVersion, uts.info.strUserId);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new AWBOperator);
    }
    //------------------------------------------------------------------------------
}
