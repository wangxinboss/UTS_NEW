#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_SaveRAW10.h"
#include "Algorithm.h"

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

		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);

		//------------------------------------------------------------------------------
		// 初始化结果
		*pnErrorCode = uts.errorcode.E_Pass;
		memset(&m_result,0,sizeof(OPERATOR_RESULT));
		//------------------------------------------------------------------------------
		// 抓图
		//------------------------------------------------------------------------------
		// 使用上次的抓图
		m_pDevice->GetCapturedBuffer(m_bufferObj);
		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		WORD *RAW10Image = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
		BYTE *RAW10Image_HL = new BYTE[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];

		CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_HL,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

		CString strName;
		SYSTEMTIME st;
		GetLocalTime(&st);
		//-------------------------------------------------------------------------
		// 输出Image file
		CString strFilePath;
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
		strFilePath.Format(_T("%s\\%s_%dx%d"),
			strDirPath,
			uts.info.strSN,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight);
		
		if (!uts.imgFile.SaveRawFile(strFilePath,RAW10Image_HL,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
		{
			*pnErrorCode = uts.errorcode.E_Fail;
		}


		RELEASE_ARRAY(RAW10Image);
		RELEASE_ARRAY(RAW10Image_HL);

		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);
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
		vecReturnValue.push_back(uts.errorcode.E_Linumance);
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

		strHeader = _T("Time,SN,TestTime(ms),Y_Avg,AWB_Result,")
			_T("dRatioR,dRatioB,dRatioB,")
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
