#include "StdAfx.h"
#include "RIOperator.h"
#include "UTOP_RI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    RIOperator::RIOperator(void)
    {
        OPERATOR_INIT;
    }

    RIOperator::~RIOperator(void)
    {
    }

    BOOL RIOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCenterType"), m_param.nCenterType, 0, _T("0: MaxY Center / 1: Image Center"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nROIWidth"), m_param.nROIWidth, 32, _T("ROI width(pixel count)"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nROIHeight"), m_param.nROIHeight, 32, _T("ROI height(pixel count)"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dRISpec"), m_param.dRISpec, 0.3, _T("worst corner RI Spec(cornerAvgY / centerAvgY), >= Spec will PASS"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dRIDeltaSpec"), m_param.dRIDeltaSpec, 0.2, _T("RI delta Spec, <= Spec will PASS"));
        return TRUE;
    }

    BOOL RIOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        m_dYvalue = 0.0;
        m_result.dRI = 0.0;
        m_result.dRIDelta = 0.0;
        m_result.ptCenterXY.x = 0;
        m_result.ptCenterXY.y = 0;
        for (int i = 0; i < Corner_SIZES; i++)
        {
            m_result.dRICorner[i] = 0.0;
        }
        
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
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
            if (m_param.nCenterType == 0)
            {
                RI_OpticalCenter::RI_RGB(
                    m_bufferObj.pBmpBuffer,
                    m_bufferInfo.nWidth,
                    m_bufferInfo.nHeight,
                    m_param.nROIWidth,
                    m_param.nROIHeight,
                    m_result);
            }
            else
            {
                RI_LogicalCenter::RI_RGB(
                    m_bufferObj.pBmpBuffer,
                    m_bufferInfo.nWidth,
                    m_bufferInfo.nHeight,
                    m_param.nROIWidth,
                    m_param.nROIHeight,
                    m_result);
            }
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 判断规格
        if (m_result.dRI < m_param.dRISpec
            || m_result.dRIDelta > m_param.dRIDeltaSpec)
        {
            *pnErrorCode = uts.errorcode.E_RI;
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

    void RIOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_RI);
    }

    void RIOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void RIOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),Y_Avg,RI_Result,")
                    _T("RI_Value,RI_LU,RI_RU,RI_LD,RI_RD,RI_Delta,RI_CTX,RI_CTY,")
                    _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%.1f,%s,")
            _T("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%d,%d,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
            , m_result.dRI, m_result.dRICorner[Corner_LU], m_result.dRICorner[Corner_RU]
            , m_result.dRICorner[Corner_LD], m_result.dRICorner[Corner_RD], m_result.dRIDelta
            , m_result.ptCenterXY.x, m_result.ptCenterXY.y
            , strVersion, uts.info.strUserId);

		if (uts.info.nMesEn!=0)
		{
			CString  str;  
			str.Format(_T("RI_YValue=%.1f,RI_Result=%s,RI_CornerLU=%d,RI_CornerRU=%d,RI_CornerLD=%d,RI_CornerRD=%d,RI_Delta=%d,"),
			 m_dYvalue, strResult,
			 m_result.dRICorner[Corner_LU], m_result.dRICorner[Corner_RU],
			 m_result.dRICorner[Corner_LD], m_result.dRICorner[Corner_RD], m_result.dRIDelta);
			uts.log.Debug((LPCTSTR)str);
			uts.info.strMESInsData += str;
		}
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new RIOperator);
    }
    //------------------------------------------------------------------------------
}
