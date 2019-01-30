#include "StdAfx.h"
#include "BaseOperator.h"
//#include "Global_Memory_SFC.h"

namespace UTS
{
    BaseOperator::BaseOperator(void)
    {
        m_bMustStopOnFail = FALSE;  // fix bug #11: SN、LightOn失败不受StopOnFail限制。
    }

    BaseOperator::~BaseOperator(void)
    {
    }

    BOOL BaseOperator::OnReadSpec()
    {
        return TRUE;
    }

    void BaseOperator::OnPreviewStart()
    {
    }

    BOOL BaseOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        return TRUE;
    }

    BOOL BaseOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
    {
        return TRUE;
    }

    void BaseOperator::OnPreviewStop()
    {
    }

    void BaseOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
    }

    void BaseOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
    }

    void BaseOperator::SetDevice(BaseDevice *pDevice)
    {
        m_pDevice = pDevice;
    }

    BaseDevice* BaseOperator::GetDevice(void)
    {
        return m_pDevice;
    }

    TEST_BUFFER& BaseOperator::GetTestBuffer(void)
    {
        return m_bufferObj;
    }

    BUFFER_INFO& BaseOperator::GetBufferInfo(void)
    {
        return m_bufferInfo;
    }

    //-------------------------------------------------------------------------
    // fix bug #11: SN、LightOn失败不受StopOnFail限制。
    BOOL BaseOperator::IsMustStopOnFail(void)
    {
        return m_bMustStopOnFail;
    }
    //-------------------------------------------------------------------------

    void BaseOperator::SaveImage(LPCTSTR lpName)
    {
        CString strName;
        if (lpName == nullptr)
        {
            strName = m_strOperatorName;
        }
        else
        {
            strName = lpName;
        }
		if (uts.info.nDeviceIndexInFileName != 0)
		{
			if (uts.info.nDeviceIndex == 0)
			{
				strName += _T("_L");
				strName.Replace(_T("_L_L"), _T("_L"));
			}
			else
			{
				strName += _T("_R");
				strName.Replace(_T("_R_R"), _T("_R"));
			}
		}

        SYSTEMTIME st;
        GetLocalTime(&st);
        //-------------------------------------------------------------------------
        // 输出Image file
        CString strFilePath;
		//!!---Chi-jen.Laio取消SFC，目前是MES
//         if (uts.info.nShopFlowEn != 0)
//         {
//             GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
//             OSUtil::CreateMultipleDirectory(uts.info.strShopFlowFilePath);
//             strFilePath.Format(_T("%s\\%s-%s-%s"),
//                 OSUtil::GetNoBSPath(uts.info.strShopFlowFilePath),
//                 uts.info.strSN,
//                 gmsfc->sSFC_ServerTime,
//                 strName);
//         }
//         else
//         {
            CString strDirPath;
            strDirPath.Format(_T("%s\\Image-%s-%s-%s-%04d%02d%02d\\"),
                uts.info.strResultPath,
                uts.info.strProjectName,
                uts.info.strLineName,
                uts.info.strStationName,
                st.wYear,
                st.wMonth, 
                st.wDay);
            UTS::OSUtil::CreateMultipleDirectory(strDirPath);
            strFilePath.Format(_T("%s\\%s-%s-%04d%02d%02d-%02d%02d%02d"),
                strDirPath,
                strName,
                uts.info.strSN,
                st.wYear,
                st.wMonth, 
                st.wDay,
                st.wHour,
                st.wMinute,
                st.wSecond);
        //}

        if (uts.info.nSaveBmpFile != 0)
        {
            if (!uts.imgFile.SaveBmpFile(
                strFilePath,
                m_bufferObj.pBmpBuffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight))
            {
                CString strMsg;
                strMsg.Format(_T("SaveBmpFile Fail. Path = %s"), strFilePath);
                uts.log.Error(strMsg);
                AfxMessageBox(strMsg);
                return;
            }
            if (uts.info.nZipFile != 0)
            {
                OSUtil::ZipFile(strFilePath + ".bmp.rar", strFilePath + ".bmp");
            }
        }

        if (uts.info.nSaveRawFile != 0)
        {
            if (!uts.imgFile.SaveRawFile(
                strFilePath,
                m_bufferObj.pRaw8Buffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight))
            {
                CString strMsg;
                strMsg.Format(_T("SaveRawFile Fail. Path = %s"), strFilePath);
                uts.log.Error(strMsg);
                AfxMessageBox(strMsg);
                return;
            }
            if (uts.info.nZipFile != 0)
            {
                OSUtil::ZipFile(strFilePath + ".raw.rar", strFilePath + ".raw");
            }
        }

        if (uts.info.nSaveYuvBmpFile != 0)
        {
            if (!uts.imgFile.SaveYUVBmpFile(
                strFilePath,
                m_bufferObj.pYuv24Buffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight))
            {
                CString strMsg;
                strMsg.Format(_T("SaveYUVBmpFile Fail. Path = %s"), strFilePath);
                uts.log.Error(strMsg);
                AfxMessageBox(strMsg);
                return;
            }
            if (uts.info.nZipFile != 0)
            {
                OSUtil::ZipFile(strFilePath + ".yuvbmp.rar", strFilePath + ".yuvbmp");
            }
        }
    }

    CString BaseOperator::GetResultCSVFilename(LPCTSTR lpName, const SYSTEMTIME &st)
    {
        CString strDirPath;
        strDirPath.Format(_T("%s\\Data-%s-%s-%s-%04d%02d%02d\\"),
            uts.info.strResultPath,
            uts.info.strProjectName,
            uts.info.strLineName,
            uts.info.strStationName,
            st.wYear,
            st.wMonth, 
            st.wDay);
        UTS::OSUtil::CreateMultipleDirectory(strDirPath);
        CString strFilePath;
        strFilePath.Format(_T("%s\\%s-%04d%02d%02d.csv"),
            strDirPath,
            lpName,
            st.wYear,
            st.wMonth, 
            st.wDay);
        return strFilePath;
    }

    void BaseOperator::SaveData()
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        //-------------------------------------------------------------------------
        // 输出Data file
        CString strFilePath = GetResultCSVFilename(m_strOperatorName, st);

        FILE* fp = NULL;
        long fileLength ;
        errno_t etRet = _tfopen_s(&fp, strFilePath, _T("a+"));
        if (0 != etRet)
        {
            CString strMsg;
            strMsg.Format(_T("Open file Fail. path = %s"), strFilePath);
            uts.log.Error(strMsg);
            AfxMessageBox(strMsg);
            return;
        }
        fseek(fp, 0, SEEK_END);
        fileLength = ftell(fp);

        CString strTime;
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d:%03d"),
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        CString strHeader;
        CString strContent;
        CString strSFCFilter;
        GetDataContent(strTime, strHeader, strContent, strSFCFilter);
        
        //------------------------------------------------------------------------------
        // 输出调试信息
        uts.log.Debug(_T("----- %s Result -----"), m_strOperatorName);
      //uts.log.Debug(_T("| %s"), strHeader);
      //uts.log.Debug(_T("| %s"), strContent);
        CString strTmp;
        for (int i = 0; i < m_strOperatorName.GetLength(); i++)
        {
            strTmp += _T("-");
        }
        uts.log.Debug(_T("------%s-------------"), strTmp);

        //------------------------------------------------------------------------------
        // 输出文件头
        if (fileLength < 3)
        {	
            _ftprintf_s(fp, _T("%s"), strHeader);
        }

        //------------------------------------------------------------------------------
        // 输出文件内容
        _ftprintf_s(fp, _T("%s"), strContent);
        fclose(fp);
		//!!---Chi-jen.Laio取消SFC，目前是MES
//         if (uts.info.nShopFlowEn != 0)
//         {
//             TestItemResult tir;
//             tir.sItemName = m_strOperatorName;
//             tir.sHeader = strHeader;
//             tir.sData = strContent;
//             if (strSFCFilter.IsEmpty())
//             {
//                 tir.sSFC_Filter = strHeader;
//             }
//             else
//             {
//                 tir.sSFC_Filter = strSFCFilter;
//             }
//             GLOBAL_MEMORY_SFC *gm = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
//             gm->vecTestItemResult.push_back(tir);
//         }
    }

    void BaseOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        strHeader = _T("Time,SN,TestTime(ms),Result\n");
        
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);
        strData.Format(_T("%s,%s,%.1f,%s\n"),
            lpTime,
            uts.info.strSN,
            m_TimeCounter.GetPassTime(),
            strResult);
    }
    
    BOOL BaseOperator::GetSensorOutMode(int &nOutMode)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
            &nOutMode, sizeof(nOutMode)))
        {
            return FALSE;
        }
        return TRUE;
    }

}

