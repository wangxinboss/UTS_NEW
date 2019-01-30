#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_MeasureCurrent.h"
#include "serialport.h"

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

		CString strValue;
		vector<int> vecValue;

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nMeasureMethod"), m_param.nMeasureMethod, 0, _T("0 : NULL 1:FLUKE 2:PROVA"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Spec.dCurrentSpec"), strValue, _T("0,5000"), _T("CurrentSpec(Min,Max)"));
		SplitInt(strValue, vecValue);
		m_param.stCurrentSpec.min = vecValue[0];
		m_param.stCurrentSpec.max = vecValue[1];
		vecValue.clear();

		return TRUE;
	}

	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		m_TimeCounter.SetStartTime();
		
		*pnErrorCode = uts.errorcode.E_Pass;

		m_result.m_current = 0.0;

		// check 串口
		if (!uts.COM_RS232.GetComOpened())
		{
			uts.log.Error(_T("RS232 open fail"));
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}
	
		char buff[256] = {0},tmp_cmd[256] = {0};
		bool bstrartbit_read = false , bstrartbit_current = false;
		char tmp_current[50]={0};

		for (int trycount = 0 ; trycount <= 10; trycount ++)
		{
			if (m_param.nMeasureMethod == 1)
			{
				uts.COM_RS232.WriteSyncPort("read?\n", strlen("read?\n"));
				Sleep(200);
				uts.COM_RS232.ReadSyncPort(buff, 256);

				uts.log.Debug(_T("s_Current: %d  %d  %d  %d"),buff[0],buff[1],buff[2],buff[3]);
				uts.log.Debug(_T("s_Current: %s"),buff);
				m_result.m_current = atof(buff);
				m_result.m_current *= 1000;
			}else if (m_param.nMeasureMethod == 2)
			{
				uts.COM_RS232.WriteSyncPort("?", strlen("?"));  //Start RS-232C
				Sleep(1200);
				uts.COM_RS232.ReadSyncPort(buff, 256);

				uts.log.Debug(_T("s_Current: %s"),buff);

				if(buff[0] == 0)
				{
					Sleep(1500);
					uts.COM_RS232.ReadSyncPort(buff, 256);
				}
	
				
				int length = strlen(buff);
				int j = 0;
				float _current;
				for (int i = 0 ;i< length;i++)
				{
					if(buff[i] == 'C' && buff[i+1] == 'H')
					{
						tmp_cmd[j] = buff[i];
						if(bstrartbit_read == false) 
						{
							j++; //start 
							bstrartbit_read = true; 
							continue;
						}
						else
						{
							bstrartbit_read = false;
							break;
						}
					}

					if(bstrartbit_read)
					{
						tmp_cmd[j] = buff[i];
						j++;
					}
				}
				tmp_cmd[j] = '\0';

				sscanf_s(tmp_cmd,"%s%s%f%s,",tmp_current,_countof(tmp_current),
										  tmp_current,_countof(tmp_current),
										  &_current,
										  tmp_current,_countof(tmp_current));

				uts.COM_RS232.WriteSyncPort("/", strlen("/")); //end RS-232C

				m_result.m_current = _current;
			}
			
			uts.log.Debug(_T("Current: %.4f"),m_result.m_current);
			if(m_result.m_current != 0.0) break;
		}

		
		if(m_result.m_current < m_param.stCurrentSpec.min || 
		   m_result.m_current > m_param.stCurrentSpec.max)
		{
			*pnErrorCode = uts.errorcode.E_Current;
		}

		//------------------------------------------------------------------------------
end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		// 保存数据文件
		SaveData();

		return m_bResult;
	}

	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_Current);
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
			_T("Current(mA),")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%s,")
			_T("%0.4f,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
			, m_result.m_current
			, strVersion, uts.info.strUserId);
	}
	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
