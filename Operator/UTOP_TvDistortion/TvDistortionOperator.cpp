#include "StdAfx.h"
#include "TvDistortionOperator.h"
#include "UTOP_TvDistortion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    TvDistortionOperator::TvDistortionOperator(void)
    {
        OPERATOR_INIT;
    }

    TvDistortionOperator::~TvDistortionOperator(void)
    {
    }

    BOOL TvDistortionOperator::OnReadSpec()
    {
        int nValue = 0;
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("tvdParam.sizeROI.cx"), nValue, 20, _T("Corner ROI width."));
        m_param.tvdParam.sizeROI.cx = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("tvdParam.sizeROI.cy"), nValue, 20, _T("Corner ROI height."));
        m_param.tvdParam.sizeROI.cy = nValue;
        return TRUE;
    }

    BOOL TvDistortionOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        //------------------------------------------------------------------------------
        // 初始化结果
        m_result.dALeft = 0.0;
        m_result.dARight = 0.0;
        m_result.dB = 0.0;
        m_result.dTvDistortion = 0.0;

        TV_Distortion_Rectangle::TvDistortion(
            pBmpBuffer,
            nWidth,
            nHeight,
            m_param.tvdParam,
            m_result);

        //------------------------------------------------------------------------------
        // OSD绘制
        UI_RECT rc;
        rc.color = COLOR_BLUE;
        UI_POINT pt;
        pt.color = COLOR_RED;
        for (int i = 0; i < Corner_SIZES; i++)
        {
            rc.rcPos = m_result.roi.rcCorner[i];
            uiMark.vecUiRect.push_back(rc);
            pt.ptPos = m_result.roi.ptCorner[i];
            uiMark.vecUiPoint.push_back(pt);
        }
        rc.rcPos = m_result.roi.rcTop;
        uiMark.vecUiRect.push_back(rc);
        pt.ptPos = m_result.roi.ptTop;
        uiMark.vecUiPoint.push_back(pt);

        rc.rcPos = m_result.roi.rcBottom;
        uiMark.vecUiRect.push_back(rc);
        pt.ptPos = m_result.roi.ptBottom;
        uiMark.vecUiPoint.push_back(pt);

        return TRUE;
    }

    BOOL TvDistortionOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        m_result.dALeft = 0.0;
        m_result.dARight = 0.0;
        m_result.dB = 0.0;
        m_result.dTvDistortion = 0.0;
        
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
                0,
                1))
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
        // 测试
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
            UI_MARK uiMark;
            OnPreview(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, uiMark);
            // 画图
            m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 判断规格
        //if (m_result.dRI < m_param.dRISpec
        //    || m_result.dRIDelta > m_param.dRIDeltaSpec)
        //{
        //    *pnErrorCode = uts.errorcode.E_RI;
        //}
        //else
        //{
        //    *pnErrorCode = uts.errorcode.E_Pass;
        //}

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

    void TvDistortionOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_DFOV);
    }

    void TvDistortionOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void TvDistortionOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        //strHeader = _T("Time,SN,TestTime(ms),Y_Avg,Partical_Result,")
        //    _T("Version,OP_SN\n");

        //strData.Format(
        //    _T("%s,%s,%.1f,%.1f,%s,")
        //    _T("%s,%s\n")
        //    , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
        //    , strVersion, uts.info.strUserId);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new TvDistortionOperator);
    }
    //------------------------------------------------------------------------------
}
