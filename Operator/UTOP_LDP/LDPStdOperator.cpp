#include "StdAfx.h"
#include "LDPStdOperator.h"
#include "UTOP_LDP.h"
//#include "Global_Memory_SFC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    LDPStdOperator::LDPStdOperator(void)
    {
        OPERATOR_INIT;
    }

    LDPStdOperator::~LDPStdOperator(void)
    {
    }

    BOOL LDPStdOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dThreshold"), m_param.dThreshold, 0.35, _T("Defect Threshold"));
        int nValue = 0;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSizeROIX"), nValue, 31, _T("Defect ROI X"));
        m_param.sizeROI.cx = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSizeROIY"), nValue, 31, _T("Defect ROI Y"));
        m_param.sizeROI.cy = nValue;
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSingleSpec"), m_param.nSingleSpec, 100, _T("Single defect pixel count, <= Spec will PASS"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDoubleSpec"), m_param.nDoubleSpec, 0, _T("Double defect pixel count, <= Spec will PASS"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nMultipleSpec"), m_param.nMultipleSpec, 0, _T("Multiple defect pixel count, <= Spec will PASS"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDefectPixel3ChannelEn"), m_param.nDefectPixel3ChannelEn, 0, _T("0: No / 1: Yes"));

        return TRUE;
    }

    BOOL LDPStdOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        m_dYvalue = 0.0;
        memset(&m_defectInfo, 0, sizeof(H4ISPDefectInfo));
        memset(m_DefPointTable, 0, sizeof(AA_TwoDPoint) * DEFECT_COUNT_MAX);

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
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_RAW8) != 0)
        {
            int nOutMode = 0;
            if (!GetSensorOutMode(nOutMode))
            {
                uts.log.Error(_T("GetSensorOutMode error."));
                *pnErrorCode = uts.errorcode.E_NoImage;
                goto end;
            }
            DefectPixels(
                m_bufferObj.pRaw8Buffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight,
                m_param.dThreshold,
                m_param.sizeROI.cx,
                m_param.sizeROI.cy,
                m_param.nDefectPixel3ChannelEn,
                m_defectInfo,
                m_DefPointTable,
                nOutMode);
        }
        else if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
            DefectPixelsRGB(
                m_bufferObj.pBmpBuffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight,
                m_param.dThreshold,
                m_param.sizeROI.cx,
                m_param.sizeROI.cy,
                m_defectInfo,
                m_DefPointTable);
        }
        else
        {
            uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 判断规格
        if (m_defectInfo.TotalSingleDefectCount > m_param.nSingleSpec
         || m_defectInfo.TotalDoubleDefectCount > m_param.nDoubleSpec
         || m_defectInfo.TotalMultipleDefectCount > m_param.nMultipleSpec)
        {
            *pnErrorCode = uts.errorcode.E_DefectPixel;

			unsigned char *pBMPBuffer=new BYTE[m_bufferInfo.nWidth*m_bufferInfo.nHeight*3];
			DrawLocationBuffer(pBMPBuffer);
			SaveLocationBufer(pBMPBuffer);
			m_pDevice->DisplayImage(pBMPBuffer);
			RELEASE_ARRAY(pBMPBuffer);

			/*uts.board.ShowMsg(_T("Continue"));
			uts.keyboard.BeginListen();
			if (uts.info.nWaitKeyType == 0)
			{
			uts.keyboard.WaitSpace(pbIsRunning);
			}
			else
			{
			uts.keyboard.WaitReturn(pbIsRunning);
			}
			uts.keyboard.EndListen();
            
            uts.board.ShowMsg(EMPTY_STR);*/
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
        CString strLocationDataFilename;
        strLocationDataFilename.Format(_T("%s-Location"), m_strOperatorName);
        SaveLocationData(strLocationDataFilename);
        SaveData();

        return m_bResult;
    }

    void LDPStdOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_DefectPixel);
    }

    void LDPStdOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void LDPStdOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),Y_Avg,LD_Result,")
            _T("Single_Num,Double_Num,Multi_Num,ISPDef_Num,CDefect_Num,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%.1f,%s,")
            _T("%d,%d,%d,%d,%d,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
            , m_defectInfo.TotalSingleDefectCount
            , m_defectInfo.TotalDoubleDefectCount
            , m_defectInfo.TotalMultipleDefectCount
            , m_defectInfo.H4ISPDefectPixelCount
            , m_defectInfo.CDefectCount
            , strVersion, uts.info.strUserId);

		if (uts.info.nMesEn!=0)
		{
			CString  str;  
			str.Format(_T("LDP_YValue=%.1f,LDP_Result=%s,LDPSingle_Num=%d,LDPDouble_Num=%d,LDPMulti_Num=%d,"),m_dYvalue, strResult,m_defectInfo.TotalSingleDefectCount, m_defectInfo.TotalDoubleDefectCount,m_defectInfo.TotalMultipleDefectCount);
			uts.log.Debug((LPCTSTR)str);
			uts.info.strMESInsData += str;
		}
    }

    void LDPStdOperator::SaveLocationData(LPCTSTR lpOperatorName)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        CString strTime;
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d:%03d"),
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        //-------------------------------------------------------------------------
        // 数据准备
        TestItemResult tir = {0};
        tir.sItemName = lpOperatorName;
        tir.sHeader = _T("Time, SN\n");
        tir.sData.Format(_T("%s, %s"), strTime, uts.info.strSN);
        for (int i = 0 ; i < DEFECT_COUNT_MAX; i++)
        {
			if(m_param.nSingleSpec==0)
			{
				tir.sData.AppendFormat(
					_T(", x%d_y%d_v%.1lf")
					, m_DefPointTable[i].x
					, m_DefPointTable[i].y
					, m_DefPointTable[i].value		
					);
				if (i == DEFECT_COUNT_MAX - 1)
				{
					tir.sData.Append(_T(", too manay"));    // over 1000 defect pixel
					break;
				}
			}else
			{
				if (m_DefPointTable[i].value > 1)
				{
					tir.sData.AppendFormat(
						_T(", x%d_y%d_v%.1lf")
						, m_DefPointTable[i].x
						, m_DefPointTable[i].y
						, m_DefPointTable[i].value		
						);
					if (i == DEFECT_COUNT_MAX - 1)
					{
						tir.sData.Append(_T(", too manay"));    // over 1000 defect pixel
						break;
					}
				}
			}
        }
        tir.sData.Append(_T("\n"));

		//!!!---2018.08.01 Chi-Jen.Liao Remove SFC
		/*
        if (uts.info.nShopFlowEn != 0)
        {
            GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
            gmsfc->vecTestItemResult.push_back(tir);
        }
		*/
        //-------------------------------------------------------------------------
        // 输出Data file
        CString strFilePath = GetResultCSVFilename(lpOperatorName, st);

        FILE* fp = NULL;
        long fileLength;
        errno_t etRet = _tfopen_s(&fp, strFilePath, _T("a+"));
        if (0 != etRet)
        {
            uts.log.Error(_T("Open file Fail. path = %s"), strFilePath);
            return;
        }
        fseek(fp, 0, SEEK_END);
        fileLength = ftell(fp);

        //------------------------------------------------------------------------------
        // 输出文件
        if (fileLength < 3)
        {	
            _ftprintf_s(fp, tir.sHeader);
        }
        _ftprintf_s(fp, tir.sData);
        fclose(fp);
    }

# define DRAWRECT 50

	void LDPStdOperator::DrawLocationBuffer(unsigned char *pBMPBuffer)
	{
		memcpy(pBMPBuffer,m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth*m_bufferInfo.nHeight*3);

		int X,Y,nsX,neX,nsY,neY;
		int nCount = 0;
		
		if(m_param.nSingleSpec == 0) nCount= m_defectInfo.TotalSingleDefectCount + m_defectInfo.TotalDoubleDefectCount + m_defectInfo.TotalMultipleDefectCount;
		else nCount= m_defectInfo.TotalDoubleDefectCount + m_defectInfo.TotalMultipleDefectCount;
		
		if(nCount > DEFECT_COUNT_MAX) return;

		for(int i=0;i<nCount;i++)
		{
			if((m_param.nSingleSpec != 0) &&
			   m_DefPointTable[i].value == 1) continue;
			
			X = m_DefPointTable[i].x;
			//Y = m_bufferInfo.nHeight - m_DefPointTable[i].y;
			Y = m_DefPointTable[i].y;
			nsX = ((X-DRAWRECT)<0)?(0):((X-DRAWRECT));
			neX = ((X+DRAWRECT) >= m_bufferInfo.nWidth)?(m_bufferInfo.nWidth-1):((X+DRAWRECT));

			nsY = ((Y-DRAWRECT)<0)?(0):((Y-DRAWRECT));
			neY = ((Y+DRAWRECT) >= m_bufferInfo.nHeight)?(m_bufferInfo.nHeight-1):((Y+DRAWRECT));

			int nWidth= m_bufferInfo.nWidth;
			int nHeight= m_bufferInfo.nHeight;
			int	x,y;
			for(y=nsY;y<=neY;y++)
			{
				pBMPBuffer[((y)*nWidth+(nsX))*3+0] = 0;
				pBMPBuffer[((y)*nWidth+(nsX))*3+1] = 0;
				pBMPBuffer[((y)*nWidth+(nsX))*3+2] = 255;
			}
			for(y=nsY;y<=neY;y++)
			{
				pBMPBuffer[((y)*nWidth+(neX))*3+0] = 0;
				pBMPBuffer[((y)*nWidth+(neX))*3+1] = 0;
				pBMPBuffer[((y)*nWidth+(neX))*3+2] = 255;
			}
			for(x=nsX;x<=neX;x++)
			{
				pBMPBuffer[((nsY)*nWidth+(x))*3+0] = 0;
				pBMPBuffer[((nsY)*nWidth+(x))*3+1] = 0;
				pBMPBuffer[((nsY)*nWidth+(x))*3+2] = 255;
			}
			for(x=nsX;x<=neX;x++)
			{
				pBMPBuffer[((neY)*nWidth+(x))*3+0] = 0;
				pBMPBuffer[((neY)*nWidth+(x))*3+1] = 0;
				pBMPBuffer[((neY)*nWidth+(x))*3+2] = 255;
			}
		}

	}

	void LDPStdOperator::SaveLocationBufer(unsigned char *pBMPBuffer)
	{
		CString strDirPath;
		CString strFilePath;

		SYSTEMTIME st;
		GetLocalTime(&st);

		//!!!---2018.08.01 Chi-Jen.Liao Remove SFC
		/*
		if (uts.info.nShopFlowEn != 0)
		{
			GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
			OSUtil::CreateMultipleDirectory(uts.info.strShopFlowFilePath);
			strFilePath.Format(_T("%s\\%s-%s-%s"),
				OSUtil::GetNoBSPath(uts.info.strShopFlowFilePath),
				uts.info.strSN,
				gmsfc->sSFC_ServerTime,
				m_strOperatorName);
		}
		else
		{
		*/
			//CString strDirPath;
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
				m_strOperatorName,
				uts.info.strSN,
				st.wYear,
				st.wMonth, 
				st.wDay,
				st.wHour,
				st.wMinute, 
				st.wSecond);
		//}

		if (!uts.imgFile.SaveBmpFile(
			strFilePath,
			pBMPBuffer,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight))
		{
			CString strMsg;
			strMsg.Format(_T("SaveBmpFile Fail. Path = %s"), strFilePath);
			uts.log.Error(strMsg);
			AfxMessageBox(strMsg);
			return;
		}
	}




    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new LDPStdOperator);
    }
    //------------------------------------------------------------------------------
}
