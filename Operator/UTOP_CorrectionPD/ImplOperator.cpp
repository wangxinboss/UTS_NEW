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

void Get3L8PDAFDefectTable(DefectPoint* PD_L_POS ,DefectPoint* PD_R_POS )
{
	uint16_t pd_block_r_x[] =
	{ 28, 80, 44, 64,32,76,48,60,48,60,32,76,44,64,28,80};
	uint16_t pd_block_r_y[] =  
	{35,35,39,39,47,47,51,51,67,67,71,71,79,79,83,83};
	uint16_t pd_block_l_x[] =
	{ 28, 80, 44, 64,32,76,48,60,48,60,32,76,44,64,28,80};
	uint16_t pd_block_l_y[] =
	{ 31,31,35,35,51,51,55,55,63,63,67,67,83,83, 87,87};

	for (int i = 0 ;i<16 ;i++)
	{
		PD_L_POS[i].x = pd_block_l_x[i];
		PD_L_POS[i].y = pd_block_l_y[i];
		PD_R_POS[i].x = pd_block_r_x[i];
		PD_R_POS[i].y = pd_block_r_y[i];
	}

}

void GetOV13855PDAFDefectTable(DefectPoint* PD_L_POS ,DefectPoint* PD_R_POS )
{
	uint16_t pd_block_r_x[] =
	{ 6, 6, 14, 14, 22, 22, 30, 30};
	uint16_t pd_block_r_y[] =  
	{10, 26, 6, 22, 10, 26,  6, 22};
	uint16_t pd_block_l_x[] =
	{ 6, 6, 14, 14, 22, 22, 30, 30};
	uint16_t pd_block_l_y[] =
	{ 14, 30, 2, 18, 14, 30,  2, 18};

	for (int i = 0 ;i<8 ;i++)
	{
		PD_L_POS[i].x = pd_block_l_x[i];
		PD_L_POS[i].y = pd_block_l_y[i];
		PD_R_POS[i].x = pd_block_r_x[i];
		PD_R_POS[i].y = pd_block_r_y[i];
	}
}

void GetS5K3P9SXPDAFDefectTable(DefectPoint* PD_L_POS ,DefectPoint* PD_R_POS )
{
	uint16_t pd_block_l_x[] =
	{ 4, 12, 20, 28};
	uint16_t pd_block_l_y[] =  
	{3,15,3,15};
	uint16_t pd_block_r_x[] =
	{ 4, 12, 20, 28};
	uint16_t pd_block_r_y[] =
	{11,7,11,7};

	for (int i = 0 ;i<4 ;i++)
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
		
// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCorrectionType"), m_param.nCorrectionType, 0, _T("0:NULL 1:Qualcomm 2:Sensor"));
// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("FWFilePath"), m_param.QualcommPatternPath, _T("./"), _T("PatternPath Path"));
// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Qualcomm_GlobalShift_X"), m_param.Qualcomm_GlobalShift_X, 12, _T("Qualcomm_GlobalShift_X"));
// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Qualcomm_GlobalShift_Y"), m_param.Qualcomm_GlobalShift_Y, 12, _T("Qualcomm_GlobalShift_Y"));
// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Qualcomm_X_Step"), m_param.Qualcomm_X_Step, 64, _T("Qualcomm_X_Step"));
// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Qualcomm_Y_Step"), m_param.Qualcomm_Y_Step, 64, _T("Qualcomm_Y_Step"));

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
		
		DefectPoint* PDAFDefectTableL,*PDAFDefectTableR;
		int TotalPDcount = 0;
		int PDLength = 0;
/*
		PDAFDefectTableL = new DefectPoint[16];
		memset(PDAFDefectTableL,0,sizeof(DefectPoint)* 16);
		PDAFDefectTableR = new DefectPoint[16];
		memset(PDAFDefectTableR,0,sizeof(DefectPoint)* 16);

		Get3L8PDAFDefectTable(PDAFDefectTableL,PDAFDefectTableR);

		int DefectCount = CalculateDPAFDefectTable(64,64,65,48,PDAFDefectTableL,16,PDAFDefectTableR,16,nullptr);
		DefectPoint* PDAFDefectTable = new DefectPoint[DefectCount];
		memset(PDAFDefectTable,0,sizeof(DefectPoint)*DefectCount);

		CalculateDPAFDefectTable(64,64,65,48,PDAFDefectTableL,16,PDAFDefectTableR,16,PDAFDefectTable);
*/
		PDAFDefectTableL = new DefectPoint[4];
		memset(PDAFDefectTableL,0,sizeof(DefectPoint)* 4);
		PDAFDefectTableR = new DefectPoint[4];
		memset(PDAFDefectTableR,0,sizeof(DefectPoint)* 4);

		GetS5K3P9SXPDAFDefectTable(PDAFDefectTableL,PDAFDefectTableR);

		int DefectCount = CalculateDPAFDefectTable(16,16,144,216,PDAFDefectTableL,4,PDAFDefectTableR,4,nullptr);
		DefectPoint* PDAFDefectTable = new DefectPoint[DefectCount];
		memset(PDAFDefectTable,0,sizeof(DefectPoint)*DefectCount);

		CalculateDPAFDefectTable(16,16,144,216,PDAFDefectTableL,4,PDAFDefectTableR,4,PDAFDefectTable);


		DefectPixelCorrection(
			m_bufferObj.pRaw8Buffer,
			m_bufferInfo.nWidth,m_bufferInfo.nHeight,
			PDAFDefectTable,
			DefectCount);

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
