#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_ColorUniformity.h"


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

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("CU_BlockX"), m_param.CU_BlockX, 11, _T("BlockX"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("CU_BlockY"), m_param.CU_BlockY, 9, _T("BlockY"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.CU_SpecCU, 100.0, _T("Max CU "));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.CU_SpecCUMaxMin, 100.0, _T("CU Max-Min"));


		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
	
		m_pDevice->GetBufferInfo(m_bufferInfo);

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
		//------------------------------------------------------------------------------
		// 测试
		double *RBRatioArray = new double[m_param.CU_BlockX * m_param.CU_BlockY];
		memset(RBRatioArray,0,sizeof(double)*m_param.CU_BlockX * m_param.CU_BlockY);
		m_dCU = 0;
		m_CUMaxMin = 0;


		if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
		{
			CtColorUniformity::CtColorUniformity(
				m_bufferObj.pBmpBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				m_param.CU_BlockX,
				m_param.CU_BlockY,
				RBRatioArray,
				m_dCU,
				m_CUMaxMin);
		}
		else
		{
			uts.log.Error(_T("buffer type error. type = %d"), m_bufferInfo.dwBufferType);
			*pnErrorCode = uts.errorcode.E_NoImage;
			goto end;
		}

		RELEASE_ARRAY(RBRatioArray);
		//------------------------------------------------------------------------------
		//判断规格
		if (m_dCU > m_param.CU_SpecCU
		 || m_CUMaxMin > m_param.CU_SpecCUMaxMin)
		{
		    *pnErrorCode = uts.errorcode.E_CU;
		}
		else
		{
		    *pnErrorCode = uts.errorcode.E_Pass;
		}


		//------------------------------------------------------------------------------
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

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
		vecReturnValue.push_back(uts.errorcode.E_CU);
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
			_T("CU,CUMaxMin,")
			_T("Version,OP_SN\n");
		 
		strData.Format(
			_T("%s,%s,%.1f,%s,")
			_T("%.2f,%.2f,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
			, m_dCU , m_CUMaxMin
			, strVersion, uts.info.strUserId);
	}
	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
