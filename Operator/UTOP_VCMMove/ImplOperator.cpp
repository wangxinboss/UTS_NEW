#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_VCMMove.h"
#include "VCM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "UTS_VCM.lib")

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
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nVCMDAC"), m_param.nVCMDAC, 512, _T("VCM Move DAC"));

		OnPreviewStart();

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
		//------------------------------------------------------------------------------
		// 抓图
		if (m_param.nReCapture != 0)
		{
			// 重新设定Sensor序列
			CString strRegName = m_strOperatorName;
			if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_WRITE_SENSOR_REGISTER,
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
		// 测试
		//DLLSetVCM_Initial(m_pDevice, uts.info.nVCMType,NULL);
		DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, m_param.nVCMDAC);
		uts.log.Debug(_T("Move2Dac[%d]"), m_param.nVCMDAC);
		//------------------------------------------------------------------------------
		// 判断规格


	

end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		//------------------------------------------------------------------------------
		

		//------------------------------------------------------------------------------
		// 保存数据文件
		SaveData();

		return m_bResult;
	}

	void ImplOperator::OnPreviewStart()
	{
		uts.log.Debug(_T("SetVCMMOVE PreviewStart"));
	
		//DLLSetVCM_Initial(m_pDevice, uts.info.nVCMType,NULL);
	}


	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
	}

	void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
	}

	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{

	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
