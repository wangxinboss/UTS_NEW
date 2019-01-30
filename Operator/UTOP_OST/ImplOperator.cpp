#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_OST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    OSTOperator::OSTOperator(void)
    {
        OPERATOR_INIT;
        m_bMustStopOnFail = TRUE;   // fix bug #11: SN、LightOn失败不受StopOnFail限制。
    }

    OSTOperator::~OSTOperator(void)
    {
    }

	BOOL OSTOperator::OnReadSpec()
    {
		//CString strKey, strComment, strValue;
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        int nValue = 0;

		_tcscpy(m_OSTConfig.PinName[0], __T("DOVDD"));
		_tcscpy(m_OSTConfig.PinName[1], __T("DVDD"));
		_tcscpy(m_OSTConfig.PinName[2], __T("AVDD"));
		_tcscpy(m_OSTConfig.PinName[3], __T("VPP"));
		_tcscpy(m_OSTConfig.PinName[4], __T("AFVDD"));
		_tcscpy(m_OSTConfig.PinName[5], __T("GND1"));
		_tcscpy(m_OSTConfig.PinName[6], __T("GND2"));
		_tcscpy(m_OSTConfig.PinName[7], __T("GND3"));
		_tcscpy(m_OSTConfig.PinName[8], __T("GND4"));

		_tcscpy(m_OSTConfig.PinName[9], __T("RESET"));
		_tcscpy(m_OSTConfig.PinName[10], __T("PWDN"));
		_tcscpy(m_OSTConfig.PinName[11], __T("MCLK"));
		_tcscpy(m_OSTConfig.PinName[12], __T("SCL"));
		_tcscpy(m_OSTConfig.PinName[13], __T("SDA"));

		_tcscpy(m_OSTConfig.PinName[14], __T("MCP"));
		_tcscpy(m_OSTConfig.PinName[15], __T("MCN"));
		_tcscpy(m_OSTConfig.PinName[16], __T("MD0P"));
		_tcscpy(m_OSTConfig.PinName[17], __T("MD0N"));
		_tcscpy(m_OSTConfig.PinName[18], __T("MD1P"));
		_tcscpy(m_OSTConfig.PinName[19], __T("MD1N"));
		_tcscpy(m_OSTConfig.PinName[20], __T("MD2P"));
		_tcscpy(m_OSTConfig.PinName[21], __T("MD2N"));
		_tcscpy(m_OSTConfig.PinName[22], __T("MD3P"));
		_tcscpy(m_OSTConfig.PinName[23], __T("MD3N"));

		_tcscpy(m_OSTConfig.PinName[43], __T("IO1"));
		_tcscpy(m_OSTConfig.PinName[44], __T("IO2"));
		_tcscpy(m_OSTConfig.PinName[45], __T("IO3"));
		_tcscpy(m_OSTConfig.PinName[46], __T("IO4"));

		m_OSTConfig.OSTPinMask[0] = 0;
		m_OSTConfig.OSTPinMask[1] = 0;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("DOVDD"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 0);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("DVDD"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 1);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AVDD"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 2);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("VPP"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 3);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AFVDD"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 4);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("GND1"), nValue, 0, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 5);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("GND2"), nValue, 0, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 6);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("GND3"), nValue, 0, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 7);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("GND4"), nValue, 0, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 8);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("RESET"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 9);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("PWDN"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 10);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MCLK"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 11);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("SCL"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 12);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("SDA"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 13);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MCP"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 14);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MCN"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 15);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MD0P"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 16);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MD0N"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 17);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MD1P"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 18);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MD1N"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 19);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MD2P"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 20);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MD2N"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 21);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MD3P"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 22);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("MD3N"), nValue, 1, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[0] = m_OSTConfig.OSTPinMask[0] | (nValue << 23);

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("IO0"), nValue, 0, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[1] = m_OSTConfig.OSTPinMask[1] | (nValue << 11);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("IO1"), nValue, 0, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[1] = m_OSTConfig.OSTPinMask[1] | (nValue << 12);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("IO2"), nValue, 0, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[1] = m_OSTConfig.OSTPinMask[1] | (nValue << 13);
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("IO3"), nValue, 0, _T("0:Disable / 1:Enable"));
		m_OSTConfig.OSTPinMask[1] = m_OSTConfig.OSTPinMask[1] | (nValue << 14);
		
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OSTCurrent"), m_OSTConfig.OSTCurrent[0], 2, _T("mA"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OSTOpenSpec"), m_OSTConfig.OSTOpenSpec[0], 1000, _T("mV"));
		
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OSTShortSpec"), m_OSTConfig.OSTShortSpec[0], 100, _T("mV"));

		return TRUE;
	}

    BOOL OSTOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        uts.board.ShowErrorMsg(EMPTY_STR);  // fix bug #8: 结果fail显示PASS，上一次的Errorcode没有刷新
        uts.board.ShowStatus(eBoardStatus::Status_Testing);

        if (nullptr == m_pDevice)
        {
            *pnErrorCode = uts.errorcode.E_Fail;
            return FALSE;
        }

		if (!m_pDevice->OST(m_OSTConfig, &m_OSTResult))
        {
            *pnErrorCode = uts.errorcode.E_Fail;
		} else {
			*pnErrorCode = uts.errorcode.E_Pass;
		}

		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		SaveData();

		return m_bResult;
    }

    void OSTOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
    }

    void OSTOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

	void OSTOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult;
		CString strLog;
		int count;

        strHeader = _T("Time,SN,TestTime(ms),OSTResult,Data(mV),")
			_T("Version,OP_SN\n");

		strLog.Empty();
		for (count = 0; count < 64; count++) {
			if (count < 32) {
				if (0 == (m_OSTConfig.OSTPinMask[0] & (1 << count))) {
					continue;
				}
			} else {
				if (0 == (m_OSTConfig.OSTPinMask[1] & (1 << (count - 32)))) {
					continue;
				}
			}

			strResult.Format(_T("%s=%d "), m_OSTConfig.PinName[count], m_OSTResult.OSTMinusVoltage[count]);
			strLog = strLog + strResult;
		}
		strLog = strLog + _T(",");

		strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strData.Format(
            _T("%s,%s,%.1f,%s,%s")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult, strLog
            , strVersion, uts.info.strUserId);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new OSTOperator);
    }
    //------------------------------------------------------------------------------
}
