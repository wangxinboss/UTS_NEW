#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_UVC_LEDTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace UTS::Algorithm::Image;

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("Set property"), _T("Message shown when previewing"));

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDarckLedOnDelay"), m_param.nDarckLedOnDelay, 0, _T("time delay after LED on."));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDarkLedTestThresholdHigh"), m_param.dDarkLedTestThresholdHigh, 200.0, _T("test image Y threshold high"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDarkLedTestThresholdLow"), m_param.dDarkLedTestThresholdLow, 100.0, _T("test image Y threshold low"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDarkLedTestR_GHigh"), m_param.dDarkLedTestR_GDeltaHigh, 0.30, _T("test image R/G delta High threshold"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDarkLedTestB_GHigh"), m_param.dDarkLedTestB_GDeltaHigh, 0.30, _T("test image B/G delta High threshold"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDarkLedTestR_GLow"), m_param.dDarkLedTestR_GDeltaLow, 0.10, _T("test image R/G delta Low threshold"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDarkLedTestB_GLow"), m_param.dDarkLedTestB_GDeltaLow, 0.10, _T("test image B/G delta Low threshold"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nDarkLedTestBlinkFrame"), m_param.nDarkLedTestBlinkFrame, 30, _T("Test Blink Frame"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dDarkLedTestBlinkDelta"), m_param.dDarkLedTestBlinkDeltaSpec, 15, _T("TestBlinkDelta(Max - Min)"));

		CString strComment,strValue;
		vector<int> vecValue;
		strComment.Format(_T("ROI(左,上,宽,高)"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("LedTestROIInfo"), strValue, _T("420,210,60,60"), strComment);
		SplitInt(strValue, vecValue);
		m_param.TestROI.left = vecValue[0];
		m_param.TestROI.top = vecValue[1];
		m_param.TestROI.right = vecValue[0] + vecValue[2];
		m_param.TestROI.bottom = vecValue[1] + vecValue[3];
		vecValue.clear();

        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		UI_RECT rc;

		m_TimeCounter.SetStartTime();
		UTS::HighPrecisionTimer timeCounter;

		memset(&m_result,0,sizeof(OPERATOR_RESULT));
 		CString strRegName = m_strOperatorName;
		m_pDevice->GetBufferInfo(m_bufferInfo);
		int ledValue = 0;

		// 重新设定Sensor序列
		if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
			strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
		{
			uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}

		//Draw Test ROI
		// 画图
		// OSD绘制
		rc.color = COLOR_GREEN;

		memcpy(&rc.rcPos, &m_param.TestROI, sizeof(RECT));
		uiMark.vecUiRect.push_back(rc);
		
		// open LED
		ledValue = 1;
		if(TRUE != m_pDevice->WriteValue(DWVT_SET_LED,&ledValue,sizeof(ledValue)))
			goto end;	

		timeCounter.SetStartTime();
		do
        {
            if (!m_pDevice->Recapture(m_bufferObj))
            {
                *pnErrorCode = uts.errorcode.E_NoImage;
            }
           m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

		} while (*pbIsRunning && timeCounter.GetPassTime() <  m_param.nDarckLedOnDelay);    

		BlinkTest(m_param.TestROI,
				  m_result.dLedBlinkMax,
				  m_result.dLedBlinkMin,
				  m_result.dLedBlinkDelta);

		LedTest(m_bufferObj.pBmpBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_param.TestROI,
				m_result.dLightValue,
				m_result.dR_GDelta,
				m_result.dB_GDelta);

		// close LED
		/*ledValue = 2;
		if(TRUE != m_pDevice->WriteValue(DWVT_SET_LED,&ledValue,sizeof(ledValue)))
			goto end;
		*/

		*pnErrorCode = uts.errorcode.E_Pass;

		if((m_result.dLightValue < m_param.dDarkLedTestThresholdLow)||
		   (m_result.dLightValue > m_param.dDarkLedTestThresholdHigh))
		{
			*pnErrorCode = uts.errorcode.E_LED;
		}

		if(m_result.dR_GDelta < m_param.dDarkLedTestR_GDeltaLow || 
		   m_result.dR_GDelta > m_param.dDarkLedTestR_GDeltaHigh )
		{
			*pnErrorCode = uts.errorcode.E_LED;
		}
	
		if(m_result.dB_GDelta < m_param.dDarkLedTestB_GDeltaLow || 
		   m_result.dB_GDelta > m_param.dDarkLedTestB_GDeltaHigh )
		{
			*pnErrorCode = uts.errorcode.E_LED;
		}

		if(m_result.dLightDelta > m_param.dDarkLedTestBlinkDeltaSpec)
		{
			*pnErrorCode = uts.errorcode.E_LED;
		}

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
        vecReturnValue.push_back(uts.errorcode.E_LED);
    }

    void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

	void ImplOperator::LedTest(  unsigned char *_pBmpImage,
		int Width,
		int Height,
		RECT    TestROI,
		double &lightValue,
		double &R_GDelta,
		double &B_GDelta)

	{
		double R_G = 0.0;
		double B_G = 0.0;
		RGBTRIPLE rgb;

		UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer, 
			m_bufferInfo.nWidth, 
			m_bufferInfo.nHeight, 
			0, TestROI, rgb);

		lightValue = YVALUE(rgb.rgbtBlue, rgb.rgbtGreen, rgb.rgbtRed);

		R_G = (double)rgb.rgbtRed/(double)rgb.rgbtGreen;
		B_G = (double)rgb.rgbtBlue/(double)rgb.rgbtGreen;
		R_GDelta = R_G - 1.0;
		B_GDelta = B_G - 1.0;

		if(R_GDelta<0)
			R_GDelta = 0 - R_GDelta;
		if(B_GDelta<0)
			B_GDelta = 0 -	B_GDelta;

		return ;
	}

	void ImplOperator::BlinkTest(RECT    TestROI,
		                         double &BlinkMax,
		                         double &BlinkMin,
		                         double &BlinkDelta)
	{
		RGBTRIPLE rgb;
		double Avg_Y;
		

		BlinkMax = 0;
		BlinkMin = 255;

		for (int i = 0;i < m_param.nDarkLedTestBlinkFrame;i++)
		{
			// 抓图
			if (!m_pDevice->Recapture(
				m_bufferObj,
				uts.info.nLTDD_DummyFrame,
				uts.info.nLTDD_AvgFrame))
			{
				uts.log.Error(_T("Recapture error. "));
				continue;
			}

			m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

			UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer, 
				m_bufferInfo.nWidth, 
				m_bufferInfo.nHeight, 
				0, TestROI, rgb);

			Avg_Y = YVALUE(rgb.rgbtBlue, rgb.rgbtGreen, rgb.rgbtRed);

			uts.log.Debug(_T("Light: %.2f"),Avg_Y);
			if (Avg_Y > BlinkMax)
			{
				BlinkMax = Avg_Y;
			}else if (Avg_Y < BlinkMin)
			{
				BlinkMin = Avg_Y;
			}

			Algorithm::Sleep(50);
		}

		BlinkDelta = BlinkMax - BlinkMin;

	}


void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),_Result,")
            _T("LightValue,R_GDelta,B_GDelta,")
			_T("BlinkMax,BlinkMin,BlinkDelta,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%s,")
            _T("%.1f,%.4f,%.4f,")
			 _T("%.1f,%.1f,%.1f,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
            , m_result.dLightValue, m_result.dR_GDelta, m_result.dB_GDelta
			, m_result.dLedBlinkMax,m_result.dLedBlinkMin,m_result.dLedBlinkDelta
            , strVersion, uts.info.strUserId);
    }
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
