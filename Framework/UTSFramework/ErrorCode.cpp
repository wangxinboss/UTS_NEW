#include "StdAfx.h"
#include "ErrorCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace UTS
{
    BOOL ErrorCode::GetAllErrorCodeInfoFromDB()
    {
        m_vecAllSetting.clear();

		char ErrString[100][20];

		strcpy(ErrString[1],"E_Pass");              
		strcpy(ErrString[2],"E_DefectPixel");       
		strcpy(ErrString[3],"E_FocusAdjustment");   
		strcpy(ErrString[4],"E_FocusCheck");        
		strcpy(ErrString[5],"E_Blemish");           
		strcpy(ErrString[6],"E_WhitePixel");        
		strcpy(ErrString[7],"E_Current");           
		strcpy(ErrString[8],"E_RI");                
		strcpy(ErrString[9],"E_ColorCheck");        
		strcpy(ErrString[10],"E_NVMDataConsist");    
		strcpy(ErrString[11],"E_NVMWrite");          
		strcpy(ErrString[12],"E_NVMCheck");          
		strcpy(ErrString[13],"E_OC");                
		strcpy(ErrString[14],"E_RU");                
		strcpy(ErrString[15],"E_CU");                
		strcpy(ErrString[16],"E_DarkNG");            
		strcpy(ErrString[17],"E_Tilt");              
		strcpy(ErrString[18],"E_Glicth");            
		strcpy(ErrString[19],"E_VCM");               
		strcpy(ErrString[20],"E_Peak");              
		strcpy(ErrString[21],"E_AWB");               
		strcpy(ErrString[22],"E_ALS");               
		strcpy(ErrString[23],"E_Temperature");       
		strcpy(ErrString[24],"E_ThermalBlemish");    
		strcpy(ErrString[25],"E_NoImage");           
		strcpy(ErrString[26],"E_ColorCal");          
		strcpy(ErrString[27],"E_Ripple");            
		strcpy(ErrString[28],"E_AbormalImage");      
		strcpy(ErrString[29],"E_DataLine");          
		strcpy(ErrString[30],"E_Gray");              
		strcpy(ErrString[31],"E_DFOV");              
		strcpy(ErrString[32],"E_LED");               
		strcpy(ErrString[33],"E_MIC");               
		strcpy(ErrString[34],"E_Aligment");          
		strcpy(ErrString[35],"E_NoiseLine");         
		strcpy(ErrString[36],"E_SNScan");            
		strcpy(ErrString[37],"E_Band");              
		strcpy(ErrString[38],"E_Linumance");         
		strcpy(ErrString[39],"E_OpenShort");         
		strcpy(ErrString[40],"E_SNR");               
		strcpy(ErrString[41],"E_FocusDelta");        
		strcpy(ErrString[42],"E_Separation");        
		strcpy(ErrString[43],"E_InitSFR");           
		strcpy(ErrString[44],"E_DBConnect");         
		strcpy(ErrString[45],"E_ReBurn");            
		strcpy(ErrString[46],"E_InfoReady");         
		strcpy(ErrString[47],"E_TvDistortion");      
		strcpy(ErrString[48],"E_Rotate");            
		strcpy(ErrString[49],"E_RelativeShift");     
		strcpy(ErrString[50],"E_ColorLevel");        
		strcpy(ErrString[51],"E_IR_LED");            
		strcpy(ErrString[52],"E_AE");                
		strcpy(ErrString[53],"E_ColorShading");      
		strcpy(ErrString[54],"E_FocusDiff");         
		strcpy(ErrString[55],"E_ColorLevelDiff");    
		strcpy(ErrString[56],"E_NVM_LSC");          
		strcpy(ErrString[57],"E_NVM_WB");           
		strcpy(ErrString[58],"E_NVM_Begolden");    
		strcpy(ErrString[59],"E_AE_TimeOut");       
		strcpy(ErrString[67],"E_ERR_Rulcommlsc");   
		strcpy(ErrString[68],"E_ERR_MTFDelta");		
		strcpy(ErrString[69],"E_ERR_ThroughFocus");	
		strcpy(ErrString[72],"E_DarkCorner");
        //------------------------------------------------------------------------------
        // 从DB中读取规格
        for (size_t i = 0; i < uts.info.vecReturnValueItem.size(); i++)
        {
            OPERATOR_ERRORCODE oe = {0};
            oe.strOperatorFileName = uts.info.vecReturnValueItem[i].strDllFilename;
            for (size_t j = 0; j < uts.info.vecReturnValueItem[i].vecReturnValue.size(); j++)
            {
                SINGLE_ERRORCODE_INFO sei = {0};
                sei.nReturnValue = uts.info.vecReturnValueItem[i].vecReturnValue[j];
                sei.strErrorCode.Format(_T("NoDef %d"), sei.nReturnValue);
                oe.vecSingleErrorcodeInfo.push_back(sei);
            }
            if (uts.info.vecReturnValueItem[i].vecReturnValue.size() > 0)
            {
                //-------------------------------------------------------------------------
                // fix 改善 #5: operator spec修改为显示时默认收起列表，并按字母排序
                vector<OPERATOR_ERRORCODE>::iterator itor;
                BOOL bAdd = FALSE;
                for (itor = m_vecAllSetting.begin(); itor != m_vecAllSetting.end(); itor++)
                {
                    if (oe.strOperatorFileName.Compare((*itor).strOperatorFileName) < 0)
                    {
                        m_vecAllSetting.insert(itor, oe);
                        bAdd = TRUE;
                        break;
                    }
                }
                if (!bAdd)
                {
                    m_vecAllSetting.push_back(oe);
                }
                //-------------------------------------------------------------------------
            }
        }
        BOOL bRet = uts.dbCof.GetErrorcodeAllSetting(m_vecAllSetting);
        if (!bRet)
        {
            AfxMessageBox(_T("Get Errorcode Setting Fail."));
            return FALSE;
        }
        return TRUE;
    }

    CString ErrorCode::GetErrorMsg(LPCTSTR lpOperatorFileName, int nErrorcode)
    {
        CString strErrorMsg;
#ifdef CONFIG_FROM_FILE
        CString strErrorFile;
        strErrorFile.Format(_T("%s%s"), uts.info.strConfigPath, _T("ErrorCode.ini"));
        IniFile inifile(strErrorFile);
        CString strKey;
        strKey.Format(_T("%d"), nErrorcode);
        strErrorMsg = inifile.ReadValue(lpOperatorFileName, strKey, EMPTY_STR);
        if (strErrorMsg == EMPTY_STR)
        {
            strErrorMsg.Format(_T("Unknown%d"), nErrorcode);
        }
#else
        for (size_t i = 0; i < m_vecAllSetting.size(); i++)
        {
            if (0 == m_vecAllSetting[i].strOperatorFileName.CompareNoCase(lpOperatorFileName))
            {
                for (size_t j = 0; j < m_vecAllSetting[i].vecSingleErrorcodeInfo.size(); j++)
                {
                    if (m_vecAllSetting[i].vecSingleErrorcodeInfo[j].nReturnValue == nErrorcode)
                    {
                        strErrorMsg = m_vecAllSetting[i].vecSingleErrorcodeInfo[j].strErrorCode;
                    }
                }
            }
        }
#endif
        return strErrorMsg;
    }
}
