#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_Focus.h"
#include "VCM.h"
#include "AlgorithmNintendo.h"
#include "EEProm.h"
#include "otpdb.h"
#include "CommonFunc.h"

#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "CommonFunc.lib")
#pragma comment(lib, "UTS_EEProm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "UTSAlgorithm_Nintendo.lib")

using namespace UTS::Algorithm;
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
		
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iDAC_Start"), m_param.iDAC_Start, 0, _T("VCM Move Start"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iDAC_End"), m_param.iDAC_End, 1023, _T("VCM Move End"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iMove_DAC"), m_param.iMove_DAC, 16, _T("VCM Move DAC"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iDelayTime"), m_param.iDelayTime, 100, _T("VCM Move DelayTime"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iFinalDelayTime"), m_param.iFinalDelayTime, 2000, _T("VCM Move Final DelayTime"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iReadyMaxCountSpec"), m_param.iReadyMaxCountSpec, 3, _T("Ready Max Count Spec"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dSobelDropRange"), m_param.dSobelDropRange, 0.1, _T("dSobelDropRange"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iROIMethod"), m_param.iROIMethod, 0, _T("iROIMethod: 0 - Assign ROI 1- Chessboard Chaer"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iFocusMethod"), m_param.iFocusMethod, 0, _T("iFocusMethod: 0 - Sobel 1 : SFR"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iSaveDACMethod"), m_param.iSaveDAC, 0, _T("iSaveDAC: 0 - NULL / 1 : Save / 2:Read"));
		CString strValue;
		vector<int> vecValue;
		vector<double> vecDoubleValue;

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("FocusROI"), strValue,  _T("100,100,100,100"), _T("ROI(左,上,宽,高)"));
		SplitInt(strValue, vecValue);
		m_param.FocusROI.left = vecValue[0];
		m_param.FocusROI.top = vecValue[1];
		m_param.FocusROI.right = vecValue[0] + vecValue[2];
		m_param.FocusROI.bottom = vecValue[1] + vecValue[3];
		
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dMinFocusSpec"), m_param.dMinFocusSpec, 0.3, _T("dMinFocusSpec"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dVCMInitMethod"), m_param.dVCMInitMethod, 1, _T("1:DB 2:EEProm"));

		return TRUE;
	}

	void ImplOperator::AutoROI()
	{
		if(m_param.iROIMethod != 0)
		{
			vector<POINTFLOAT> vecPt;

			CImageProc::GetInstance().Cal_RGBtoYBuffer(
				m_bufferObj.pBmpBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_bufferObj.pYBuffer);

			if (!ChessBoard::GetChessboardPos(
				m_bufferObj.pYBuffer, 
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight, 7, 4, vecPt))
			{
				uts.log.Error(_T("GetChessboardPos error. "));
			}
	
			int ROIWidth = m_param.FocusROI.right - m_param.FocusROI.left;
			int ROIheight = m_param.FocusROI.bottom - m_param.FocusROI.top;

			m_param.FocusROI.left = DOUBLE2LONG(vecPt[10].x - ROIWidth/2);
			m_param.FocusROI.right = m_param.FocusROI.left + ROIWidth;
			m_param.FocusROI.top = DOUBLE2LONG(((m_bufferInfo.nHeight - vecPt[10].y) + (m_bufferInfo.nHeight - vecPt[17].y))/2 - ROIheight/2) ;
			m_param.FocusROI.bottom = m_param.FocusROI.top + ROIheight;
			
		}
	}

	void ImplOperator::OnPreviewStart()
	{
		uts.log.Debug(_T("Focus PreviewStart"));
		if (0 == _tcscmp(uts.info.strProjectName, _T("CA188")))
		{
			DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, 0);
		}else if ((0 == _tcscmp(uts.info.strProjectName, _T("CA210")))|| 
			          (0 == _tcscmp(uts.info.strProjectName, _T("CA211"))))
		{
			unsigned char tmp[6];
			uint16_t HallData[4];
			USES_CONVERSION;

			if(m_param.dVCMInitMethod == 1)
			{
				int ret = uts.otpdb->GetOtpByType(uts.otpdb->GetModuleID(T2A(uts.info.strSN)),11 , 
					(char *)tmp, sizeof(tmp));
				if (ret < 0)
				{
					uts.log.Error(_T("get Hall data error!!!"));
					return;
				}
			}else if(m_param.dVCMInitMethod == 2)
			{
				UTS::EEPROMDriver *eeprom;
				eeprom = GetEEPromDriverInstance(m_pDevice, 5);
				eeprom->Read(0x0018, tmp, sizeof(tmp));
			}

			HallData[0] = get_le_val(tmp + 0, 1);
			HallData[1] = get_le_val(tmp + 1, 1);
			HallData[2] = get_be_val(tmp + 2, 2);
			HallData[3] = get_be_val(tmp + 4, 2);
			DLLSetVCM_Initial(m_pDevice, uts.info.nVCMType,HallData);
		}else
			DLLSetVCM_Initial(m_pDevice, uts.info.nVCMType,NULL);
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
		// OSD绘制
		UI_TEXT text;
		UI_MARK uiMark;
		UI_RECT rc;

		// 重新设定Sensor序列
		CString strRegName = m_strOperatorName;
		if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
			strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
		{
			uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
			*pnErrorCode = uts.errorcode.E_Fail;
		}

		Sleep(1000);

		*pnErrorCode = uts.errorcode.E_Pass;
		
		if(m_param.iSaveDAC == 2) //Read
		{ 
			m_result.iMaxDAC = (int) uts.info.mapShared["Focus"] ;
			DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, m_result.iMaxDAC);
			Algorithm::Sleep(m_param.iFinalDelayTime);

			// 抓图
			if (!m_pDevice->Recapture(
				m_bufferObj,
				uts.info.nLTDD_DummyFrame,
				uts.info.nLTDD_AvgFrame))
			{
				uts.log.Error(_T("Recapture error. "));
			}

			// 画图
			m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);
			uiMark.clear();

			goto end;
		}

		m_pDevice->GetBufferInfo(m_bufferInfo);

		m_result.dMaxFocusValue = 0.0;
		m_result.iMaxDAC = m_param.iDAC_Start;

		int ROIWidth = m_param.FocusROI.right - m_param.FocusROI.left;
		int ROIheight = m_param.FocusROI.bottom - m_param.FocusROI.top;

		unsigned char* YBuffer = new unsigned char[ROIWidth* ROIheight];
		memset(YBuffer,0,sizeof(unsigned char)*ROIWidth* ROIheight);
		unsigned char *_CenterBMPBuffer = new unsigned char[ROIWidth* ROIheight*3];								 
		memset(_CenterBMPBuffer,0,sizeof(unsigned char)*(ROIWidth* ROIheight)*3);

		unsigned char* bmpYroi = new unsigned char[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
		memset(bmpYroi,0,sizeof(unsigned char)*m_bufferInfo.nWidth* m_bufferInfo.nHeight);

		int iReadyMaxCount = 0;
		//Start->End
		int _DAC = m_param.iDAC_Start;
		
		while ((_DAC <= m_param.iDAC_End))
		{				
			DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, _DAC);

			Algorithm::Sleep(m_param.iDelayTime);

			// 抓图
			if (!m_pDevice->Recapture(
				m_bufferObj,
				uts.info.nLTDD_DummyFrame,
				uts.info.nLTDD_AvgFrame))
			{
				uts.log.Error(_T("Recapture error. "));
			}

			AutoROI();

			if(m_param.iFocusMethod == 0)
			{
				CImageProc::GetInstance().GetBMPBlockBuffer(
					m_bufferObj.pBmpBuffer, _CenterBMPBuffer,
					m_bufferInfo.nWidth,
					m_bufferInfo.nHeight,
					m_param.FocusROI.left , m_param.FocusROI.top,
					ROIWidth, ROIheight);

				CImageProc::GetInstance().Cal_RGBtoYBuffer(
					_CenterBMPBuffer,
					ROIWidth, ROIheight,
					YBuffer);

				Algorithm::SobelFocus::CalImageSobel(YBuffer,ROIWidth, ROIheight,m_result.dFocusValue);
			}else if(m_param.iFocusMethod == 1)
			{
				double dLoSFR ;
				Algorithm::SFR::SfrCalc::GetBlockSFR(
					m_bufferObj.pBmpBuffer,
					m_bufferInfo.nWidth,
					m_bufferInfo.nHeight,
					m_param.FocusROI,
					0.125,
					0.25,
					m_result.dFocusValue,
					dLoSFR);
			}if(m_param.iFocusMethod == 2)
			{
				CImageProc::GetInstance().Cal_RGBtoYBuffer(
					m_bufferObj.pBmpBuffer,
					m_bufferInfo.nWidth,
					m_bufferInfo.nHeight,
					bmpYroi);

				Algorithm::MTF::MtfCalc::GetYBlockMTF(bmpYroi, m_bufferInfo.nWidth,
					m_bufferInfo.nHeight, m_param.FocusROI, m_result.dFocusValue);
			}else if(m_param.iFocusMethod == 3)
			{
				CImageProc::GetInstance().GetBMPBlockBuffer(
					m_bufferObj.pBmpBuffer, _CenterBMPBuffer,
					m_bufferInfo.nWidth,
					m_bufferInfo.nHeight,
					m_param.FocusROI.left , m_param.FocusROI.top,
					ROIWidth, ROIheight);

				CImageProc::GetInstance().Cal_RGBtoYBuffer(
					_CenterBMPBuffer,
					ROIWidth, ROIheight,
					YBuffer);

				Algorithm::SobelFocus::CalImageSobelX(YBuffer,ROIWidth, ROIheight,m_result.dFocusValue);
			}

			//Get Max 
			if(m_result.dMaxFocusValue < m_result.dFocusValue)
			{
				m_result.dMaxFocusValue = m_result.dFocusValue;
				m_result.iMaxDAC = _DAC;
				iReadyMaxCount = 0;
			}else if((m_result.dMaxFocusValue - m_result.dFocusValue) > m_param.dSobelDropRange)
				iReadyMaxCount ++;

			if(iReadyMaxCount >= m_param.iReadyMaxCountSpec) break;

			uts.log.Debug(_T("DAC = 0x%x FocuslValue = %.2f"),_DAC,m_result.dFocusValue);
			_DAC += m_param.iMove_DAC;

			rc.color = text.color = COLOR_BLUE;
			memcpy(&rc.rcPos, &m_param.FocusROI, sizeof(RECT));
			uiMark.vecUiRect.push_back(rc);

			text.ptPos = CPoint(m_param.FocusROI.left, m_param.FocusROI.bottom + 30);
			text.strText.Format(_T("%.1lf "),m_result.dFocusValue);
			text.color = COLOR_BLUE;
			uiMark.vecUIText.push_back(text);

			// 画图
			m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
			uiMark.clear();
		}

		uts.log.Debug(_T("MaxDAC = 0x%x MaxFocusValue = %.2f"),m_result.iMaxDAC,m_result.dMaxFocusValue);
		DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, m_result.iMaxDAC);
		Algorithm::Sleep(m_param.iFinalDelayTime);

		// 抓图
		if (!m_pDevice->Recapture(
			m_bufferObj,
			uts.info.nLTDD_DummyFrame,
			uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("Recapture error. "));
		}

		AutoROI();

		if(m_param.iFocusMethod == 0)
		{
			CImageProc::GetInstance().GetBMPBlockBuffer(
				m_bufferObj.pBmpBuffer, _CenterBMPBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_param.FocusROI.left , m_param.FocusROI.top,
				ROIWidth, ROIheight);

			CImageProc::GetInstance().Cal_RGBtoYBuffer(
				_CenterBMPBuffer,
				ROIWidth, ROIheight,
				YBuffer);

			Algorithm::SobelFocus::CalImageSobel(YBuffer,ROIWidth, ROIheight,m_result.dFocusValue);
		}else if(m_param.iFocusMethod == 1)
		{
			double dLoSFR ;
			Algorithm::SFR::SfrCalc::GetBlockSFR(
				m_bufferObj.pBmpBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_param.FocusROI,
				0.125,
				0.25,
				m_result.dFocusValue,
				dLoSFR);
		}if(m_param.iFocusMethod == 2)
		{
			CImageProc::GetInstance().Cal_RGBtoYBuffer(
				m_bufferObj.pBmpBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				bmpYroi);

			Algorithm::MTF::MtfCalc::GetYBlockMTF(bmpYroi, m_bufferInfo.nWidth,
				m_bufferInfo.nHeight, m_param.FocusROI, m_result.dFocusValue);
		}else if(m_param.iFocusMethod == 3)
		{
			CImageProc::GetInstance().GetBMPBlockBuffer(
				m_bufferObj.pBmpBuffer, _CenterBMPBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_param.FocusROI.left , m_param.FocusROI.top,
				ROIWidth, ROIheight);

			CImageProc::GetInstance().Cal_RGBtoYBuffer(
				_CenterBMPBuffer,
				ROIWidth, ROIheight,
				YBuffer);

			Algorithm::SobelFocus::CalImageSobelX(YBuffer,ROIWidth, ROIheight,m_result.dFocusValue);
		}

		uts.log.Debug(_T("DAC = 0x%x FocuslValue = %.2f"),m_result.iMaxDAC,m_result.dFocusValue);
		_DAC += m_param.iMove_DAC;

		rc.color = text.color = COLOR_GREEN;
		memcpy(&rc.rcPos, &m_param.FocusROI, sizeof(RECT));
		uiMark.vecUiRect.push_back(rc);

		text.ptPos = CPoint(m_param.FocusROI.left, m_param.FocusROI.bottom + 30);
		text.strText.Format(_T("%.1lf "),m_result.dFocusValue);
		text.color = COLOR_BLUE;
		uiMark.vecUIText.push_back(text);

		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		if(m_result.dMaxFocusValue < m_param.dMinFocusSpec)
		{
			*pnErrorCode = uts.errorcode.E_Fail;
		}

		RELEASE_ARRAY(YBuffer);
		RELEASE_ARRAY(_CenterBMPBuffer);
		RELEASE_ARRAY(bmpYroi);

		//save DAC
		if(m_param.iSaveDAC == 1) //Write
		uts.info.mapShared["Focus"]  = m_result.iMaxDAC;


		//------------------------------------------------------------------------------
		// 保存数据文件
		SaveData();
		SaveImage();
end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		return m_bResult;
	}

	BOOL ImplOperator::OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark)
	{

		return TRUE;
	}

	
	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NVMWrite);
	}

	void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		vecRegister.push_back(m_strOperatorName);
	}


	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		strHeader = _T("Time,SN,TestTime(ms),_Result,")
			_T("FocusValue,MaxFocusValue,MaxDAC,")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%s,")
			_T("%.1f,%.1f,%d,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
			, m_result.dFocusValue, m_result.dMaxFocusValue,m_result.iMaxDAC
			, strVersion, uts.info.strUserId);
	}
	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
