#include "StdAfx.h"
#include "ImplOperator.h"
#include "SensorDriver.h"
#include "UTOP_CorrectionPD.h"

#pragma comment(lib, "UTSAlgorithm.lib")
#pragma comment(lib, "UTS_Sensor.lib")

#include "otpdb.h"
#include "CommonFunc.h"

#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "CommonFunc.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace UTS::Algorithm::Correction_PD;

void Get2L8PDAFDefectTable(int &PDLength,DefectPoint* PD_L_POS = nullptr,DefectPoint* PD_R_POS = nullptr)
{
	uint16_t pd_block_l_x[] =
	{ 4, 4, 8, 8,20,20,24,24,36,36,40,40,52,52,56,56};
	uint16_t pd_block_l_y[] =  
	{11,59,23,47,15,55,27,43,27,43,15,55,23,47,11,59};
	uint16_t pd_block_r_x[] =
	{ 4, 4, 8, 8,20,20,24,24,36,36,40,40,52,52,56,56};
	uint16_t pd_block_r_y[] =
	{ 7,63,27,43,11,59,31,39,31,39,11,59,27,43, 7,63};
	
	PDLength = 16;

	if ((PD_L_POS==nullptr)||(PD_R_POS == nullptr))
	{
		return;
	}

	for (int i = 0 ;i<16 ;i++)
	{
		PD_L_POS[i].x = pd_block_l_x[i];
		PD_L_POS[i].y = pd_block_l_y[i];
		PD_R_POS[i].x = pd_block_r_x[i];
		PD_R_POS[i].y = pd_block_r_y[i];
	}

}


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
		
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCorrectionType"), m_param.nCorrectionType, 0, _T("0:NULL 1:Qualcomm 2:Sensor"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("FWFilePath"), m_param.QualcommPatternPath, _T("./"), _T("PatternPath Path"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Qualcomm_GlobalShift_X"), m_param.Qualcomm_GlobalShift_X, 12, _T("Qualcomm_GlobalShift_X"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Qualcomm_GlobalShift_Y"), m_param.Qualcomm_GlobalShift_Y, 12, _T("Qualcomm_GlobalShift_Y"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Qualcomm_X_Step"), m_param.Qualcomm_X_Step, 64, _T("Qualcomm_X_Step"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Qualcomm_Y_Step"), m_param.Qualcomm_Y_Step, 64, _T("Qualcomm_Y_Step"));

		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);

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

		USES_CONVERSION;
		
		if(m_param.nCorrectionType == 1)
		{
			DefectPoint* PDAFDefectTable;
			PDAFDefectTable = new DefectPoint[m_bufferInfo.nWidth*m_bufferInfo.nHeight];
			memset(PDAFDefectTable,0,sizeof(DefectPoint)*m_bufferInfo.nWidth*m_bufferInfo.nHeight);

			int TotalPDcount = 0;

			LPCTSTR  str = (LPCTSTR)m_param.QualcommPatternPath;
			TotalPDcount  = CalculateDPAFDefectTable_Qualcomm( 
				T2W(m_param.QualcommPatternPath.GetBuffer()) ,
				m_param.Qualcomm_GlobalShift_X ,
				m_param.Qualcomm_GlobalShift_Y, 
				m_param.Qualcomm_X_Step , 
				m_param.Qualcomm_Y_Step,
				m_bufferInfo.nWidth,m_bufferInfo.nHeight,
				PDAFDefectTable);

			DefectPixelCorrection(
				m_bufferObj.pRaw8Buffer,
				m_bufferInfo.nWidth,m_bufferInfo.nHeight,
				PDAFDefectTable,
				TotalPDcount);

			int nOutMode = 0;

			if (!m_pDevice->ReadValue(
				eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
				&nOutMode, sizeof(nOutMode)))
			{
				uts.log.Error(_T("GetSensorOutMode error."));
				*pnErrorCode = uts.errorcode.E_NoImage;
				goto end;
			}

			UTS::Algorithm::Image::CImageProc::GetInstance().RawToBmp(nOutMode, 
				m_bufferObj.pRaw8Buffer, m_bufferObj.pBmpBuffer, 
				m_bufferInfo.nWidth, m_bufferInfo.nHeight);

			RELEASE_ARRAY(PDAFDefectTable);
		}else if(m_param.nCorrectionType == 2)
		{
			UTS::SensorDriver *sensor;

			u8 temp_spc[126];

			if (uts.otpdb->GetOtpByType(uts.otpdb->GetModuleID(T2A(uts.info.strSN)), 12, (char *)&temp_spc, 126) < 0)
			{
				*pnErrorCode = uts.errorcode.E_DBConnect;
				goto end;
			}

			sensor = GetSensorInstance(m_pDevice,uts.info.nSensorType);
			sensor->spc_writeback(temp_spc,63);
		}else if(m_param.nCorrectionType == 3)
		{
			DefectPoint* PDAFDefectTableL,*PDAFDefectTableR,*PDAFDefectTable;
			int TotalPDcount = 0;
			int PDLength = 0;

			PDAFDefectTableL = new DefectPoint[m_bufferInfo.nWidth*m_bufferInfo.nHeight];
			memset(PDAFDefectTableL,0,sizeof(DefectPoint)* m_bufferInfo.nWidth*m_bufferInfo.nHeight);
			PDAFDefectTableR = new DefectPoint[m_bufferInfo.nWidth*m_bufferInfo.nHeight];
			memset(PDAFDefectTableR,0,sizeof(DefectPoint)* m_bufferInfo.nWidth*m_bufferInfo.nHeight);
			PDAFDefectTable = new DefectPoint[m_bufferInfo.nWidth*m_bufferInfo.nHeight];
			memset(PDAFDefectTable,0,sizeof(DefectPoint)* m_bufferInfo.nWidth*m_bufferInfo.nHeight);

			Get2L8PDAFDefectTable(PDLength,PDAFDefectTableL,PDAFDefectTableR);
		
			int _BlockNumX = (m_bufferInfo.nWidth - m_param.Qualcomm_GlobalShift_X)/m_param.Qualcomm_X_Step;
			int _BlockNumY = (m_bufferInfo.nHeight- m_param.Qualcomm_GlobalShift_Y)/m_param.Qualcomm_Y_Step;		

			int Totalcount = CalculateDPAFDefectTable_Qualcomm(
				m_param.Qualcomm_GlobalShift_X ,
				m_param.Qualcomm_GlobalShift_Y, 
				m_param.Qualcomm_X_Step , 
				m_param.Qualcomm_Y_Step,
				_BlockNumX,_BlockNumY,
				PDAFDefectTableL,PDLength,PDAFDefectTableR,PDLength,PDAFDefectTable);

			DefectPixelCorrection(
				m_bufferObj.pRaw8Buffer,
				m_bufferInfo.nWidth,m_bufferInfo.nHeight,
				PDAFDefectTable,
				TotalPDcount);

			int nOutMode = 0;

			if (!m_pDevice->ReadValue(
				eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
				&nOutMode, sizeof(nOutMode)))
			{
				uts.log.Error(_T("GetSensorOutMode error."));
				*pnErrorCode = uts.errorcode.E_NoImage;
				goto end;
			}

			UTS::Algorithm::Image::CImageProc::GetInstance().RawToBmp(nOutMode, 
				m_bufferObj.pRaw8Buffer, m_bufferObj.pBmpBuffer, 
				m_bufferInfo.nWidth, m_bufferInfo.nHeight);

			RELEASE_ARRAY(PDAFDefectTableL);
			RELEASE_ARRAY(PDAFDefectTableR);
			RELEASE_ARRAY(PDAFDefectTable);

		}
		//------------------------------------------------------------------------------
end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		//------------------------------------------------------------------------------
		// 保存图片文件
		SaveImage();
		
		return m_bResult;
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
			
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%s,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
			, strVersion, uts.info.strUserId);
	}
	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
