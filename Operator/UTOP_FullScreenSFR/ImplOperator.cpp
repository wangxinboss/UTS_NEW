#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_FullScreenSFR.h"

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

        //uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
        //uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
        //uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));

        //CString strValue;
        //vector<int> vecValue;
        //for (int i = 0; i < RI_ROI_COUNT; i++)
        //{
        //    CString strKeyName;
        //    strKeyName.Format(_T("stROIInfo[%s]"), strROIName[i]);
        //    CString strComment;
        //    if (i == RI_Center)
        //    {
        //        strComment.Format(_T("ROI[%s](左,上,宽,高)"), strROIName[i]);
        //        uts.dbCof.GetOperatorSingleSpec(strSection, strKeyName, strValue, strDefaultValue[i], strComment);
        //        SplitInt(strValue, vecValue);
        //        m_param.stRIParam.rcPos[i].left = vecValue[0];
        //        m_param.stRIParam.rcPos[i].top = vecValue[1];
        //        m_param.stRIParam.rcPos[i].right = vecValue[0] + vecValue[2];
        //        m_param.stRIParam.rcPos[i].bottom = vecValue[1] + vecValue[3];
        //    }
        //    else
        //    {
        //        strComment.Format(_T("ROI[%s](左,上,宽,高,最小规格)"), strROIName[i]);
        //        uts.dbCof.GetOperatorSingleSpec(strSection, strKeyName, strValue, strDefaultValue[i], strComment);
        //        SplitInt(strValue, vecValue);
        //        m_param.stRIParam.rcPos[i].left = vecValue[0];
        //        m_param.stRIParam.rcPos[i].top = vecValue[1];
        //        m_param.stRIParam.rcPos[i].right = vecValue[0] + vecValue[2];
        //        m_param.stRIParam.rcPos[i].bottom = vecValue[1] + vecValue[3];
        //        m_param.dRIMinSpec[i] = vecValue[4];
        //    }
        //}

        return TRUE;
    }
    
    //enum {VERTEX_LEFT = 0, VERTEX_TOP, VERTEX_RIGHT, VERTEX_BOTTOM};
    //void GetAroundVertex(unsigned char *pYBuffer, int nWidth, int nHeight, const POINT &ptCenter, const vector<POINT> &vecVertex);


    BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        m_pDevice->GetBufferInfo(m_bufferInfo);
        CImageProc::GetInstance().Cal_RGBtoYBuffer(bufferObj.pBmpBuffer, nWidth, nHeight, bufferObj.pYBuffer);
        //-------------------------------------------------------------------------
        // get center logic position
        POINT ptCenter = {nWidth / 2, nHeight / 2};
        vector<POINT> vecVertex;
        //GetAroundVertex(bufferObj.pYBuffer, nWidth, nHeight, ptCenter, vecVertex);

        //------------------------------------------------------------------------------
        // OSD绘制
        UI_RECT rc;
        UI_POINT pt;
        UI_TEXT text;
        UI_TEXT_REL textRel;
        UI_LINE line;

        return TRUE;
    }
    
    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
//        //------------------------------------------------------------------------------
//        // 初始化
//        CStringA strHeader, strKey;
//        UI_MARK uiMark;
//        m_TimeCounter.SetStartTime();
//        m_pDevice->GetBufferInfo(m_bufferInfo);
//
//        //------------------------------------------------------------------------------
//        // 初始化结果
//        ;
//
//        if (m_param.nReCapture != 0)
//        {
//            // 重新设定Sensor序列
//            CString strRegName = m_strOperatorName;
//            if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
//                strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
//            {
//                uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
//                *pnErrorCode = uts.errorcode.E_Fail;
//                goto end;
//            }
//
//            // 抓图
//            if (!m_pDevice->Recapture(
//                m_bufferObj,
//                uts.info.nLTDD_DummyFrame,
//                uts.info.nLTDD_AvgFrame))
//            {
//                uts.log.Error(_T("Recapture error. "));
//                *pnErrorCode = uts.errorcode.E_NoImage;
//                goto end;
//            }
//        }
//        else
//        {
//            // 使用上次的抓图
//            m_pDevice->GetCapturedBuffer(m_bufferObj);
//        }
//
//        // 画图
//        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
//        
//        //------------------------------------------------------------------------------
//        // 测试
//        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_YUV24) != 0)
//        {
//            CImageProc::GetInstance().GetYUV24_YBuffer(
//                m_bufferObj.pYuv24Buffer,
//                m_bufferInfo.nWidth,
//                m_bufferInfo.nHeight,
//                m_bufferObj.pYBuffer);
//
//            // 画面亮度
//            RECT rcWholeImage = {0, 0, m_bufferInfo.nWidth, m_bufferInfo.nHeight};
//            GetBlockAvgY(
//                m_bufferObj.pYBuffer,
//                m_bufferInfo.nWidth,
//                m_bufferInfo.nHeight,
//                rcWholeImage,
//                m_result.dAvgY);
//
//            // RI
//            RI_Y(
//                m_bufferObj.pYBuffer,
//                m_bufferInfo.nWidth,
//                m_bufferInfo.nHeight,
//                m_param.stRIParam,
//                m_result.stRIResult);
//
//            for (int i = 1; i < RI_ROI_COUNT; i++)
//            {
//                m_result.stRIResult.dRICorner[i] *= 100.0;
//            }
//            m_result.stRIResult.dRIDelta *= 100.0;
//            m_result.stRIResult.dRIWorst *= 100.0;
//        }
//
//        //------------------------------------------------------------------------------
//        // 判断画面平均亮度
//        if (m_result.dAvgY < m_param.dLTMinY || m_result.dAvgY > m_param.dLTMaxY)
//        {
//            *pnErrorCode = uts.errorcode.E_Linumance;
//            goto end;
//        }
//        //------------------------------------------------------------------------------
//        // 判断规格
//        for (int i = 1; i < RI_ROI_COUNT; i++)
//        {
//            if (m_result.stRIResult.dRICorner[i] < m_param.dRIMinSpec[i])
//            {
//                *pnErrorCode = uts.errorcode.E_RI;
//                goto end;
//            }
//        }
//        
//        *pnErrorCode = uts.errorcode.E_Pass;
//
//end:
//        // 根据Errorcode设置结果
//        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);
//
//        //------------------------------------------------------------------------------
//        // 保存图片文件
//        if (m_param.nReCapture != 0)
//        {
//            SaveImage();
//        }
//
//        //------------------------------------------------------------------------------
//        // 保存数据文件
//        SaveData();
        
        return m_bResult;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        //vecReturnValue.push_back(uts.errorcode.E_Fail);
        //vecReturnValue.push_back(uts.errorcode.E_NoImage);
        //vecReturnValue.push_back(uts.errorcode.E_Linumance);
        //vecReturnValue.push_back(uts.errorcode.E_RI);
    }

    void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        //CString strVersion;
        //UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        //CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        ////-------------------------------------------------------------------------
        //// Head
        //strHeader = _T("Time,SN,SensorID,DeviceIndex,TestTime(ms),Result,");
        //strData.Format(_T("%s,%s,%s,%d,%.1f,%s,"),
        //    lpTime,
        //    uts.info.strSN,
        //    uts.info.strSensorId,
        //    uts.info.nDeviceIndex,
        //    m_TimeCounter.GetPassTime(),
        //    strResult);

        ////-------------------------------------------------------------------------
        //// Body
        //strHeader.Append(_T("Yavg,"));
        //strData.AppendFormat(_T("%.1f,"), m_result.dAvgY);
        //for (int i = 1; i < RI_ROI_COUNT; i++)
        //{
        //    strHeader.AppendFormat(_T("RI[%s],"), strROIName[i]);
        //    strData.AppendFormat(_T("%.1f,"), m_result.stRIResult.dRICorner[i]);
        //}

        ////-------------------------------------------------------------------------
        //// Tail
        //strHeader.Append(_T("Version,OP_SN\n"));
        //strData.AppendFormat(_T("%s,%s\n"),
        //    strVersion,
        //    uts.info.strUserId);
    }
    
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
