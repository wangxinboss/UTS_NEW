#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_Otp.h"
#include "CommonFunc.h"

#include "otpdb.h"
#include "VCM.h"

#pragma comment(lib, "UTS_VCM.lib")
#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "CommonFunc.lib")


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

        // Common
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strOTPTestOpt"), m_param.strOTPTestOpt, _T("3"), _T("0:ProgOTP, 1:CheckOTP, 2:LSC, 3:WB, 6:LOADLSC ,\
																								   7:LOADWB , 8:DUMP , 9:SPC,10:DCC0 , 11:DCC1,\
																								 14:OffChk ,17:PDAFVerify ,19:LoadFW ,20:CheckFW , \
																								 21:HallCali ,22:GyroCali , 23:OISCheck ,24:SENSORSPC,\
																								 25:GyroCaliNonShaker,29:OtpCaliOpt_LSCMTK,30:OtpCaliOpt_CrossTalk"));
        
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSensorBlackLevel"), m_param.nSensorBlackLevel,16, _T("SensorBlackLevel"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nGROUP_FOR_USE"), m_param.nGROUP_FOR_USE, 0, _T("GROUP_FOR_USE"));

		//EEProm
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nEEPRomType"), m_param.nEEPRomType, 13, _T("0:EEPROM_TYPE_Virtual,\
																								  1:EEPROM_TYPE_24C16,2:EEPROM_TYPE_24C32,\
			3:EEPROM_TYPE_24C256,4:EEPROM_TYPE_DW9761,5:EEPROM_TYPE_24C64,6:EEPROM_TYPE_ZC533,7:EEPROM_TYPE_FM24C64D,8:EEPROM_TYPE_BL24C64,\
			9:EEPROM_TYPE_CAT24C64,10:EEPROM_TYPE_24P64,11:EEPROM_TYPE_GT9767,12:EEPROM_TYPE_SIZE,13:EEPROM_TYPE_M24C64"));
	
        // WB
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nWB_Type"), m_param.nWB_Type, 0, _T("0:5100K,1:4000K,2:3100K"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nWB_ROI_Width"), m_param.nWB_ROI_Width, -1, _T("WB_ROI_Width"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nWB_ROI_Height"), m_param.nWB_ROI_Height, -1, _T("WB_ROI_Height"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dWB_RG_TARGET"), m_param.dWB_RG_TARGET, 1.0, _T("WB_RG_TARGET"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dWB_BG_TARGET"), m_param.dWB_BG_TARGET, 1.0, _T("WB_BG_TARGET"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dWB_RG_DELTA_SPEC"), m_param.dWB_RG_DELTA_SPEC, 0.1, _T("WB_RG_DELTA_SPEC"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dWB_BG_DELTA_SPEC"), m_param.dWB_BG_DELTA_SPEC, 0.1, _T("WB_BG_DELTA_SPEC"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dWB_GOLDEN_SPEC"), m_param.dWB_GOLDEN_SPEC, 0.3, _T("WB_GOLDEN_SPEC"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dWB_BEGODLEN_SPEC"), m_param.dWB_BEGODLEN_SPEC, 0.1, _T("WB_BEGODLEN_SPEC"));


        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nWB_GODLEN_ValidTime"), m_param.nWB_GODLEN_ValidTime, 30, _T("WB_GODLEN_ValidTime"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nWB_GODLEN_WarnTime"), m_param.nWB_GODLEN_WarnTime, 24, _T("WB_GODLEN_WarnTime"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dWB_RG_MULTI"), m_param.dWB_RG_MULTI, 1.0, _T("dWB_RG_MULTI"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dWB_BG_MULTI"), m_param.dWB_BG_MULTI, 1.0, _T("dWB_BG_MULTI"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strWB_GOLDEN_OFFLINE_RGrGbB"), m_param.strWB_GOLDEN_OFFLINE_RGrGbB, _T("0,0,0,0"), _T("strWB_GOLDEN_OFFLINE_RGrGbB"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strWB_RGrGbB_L"), m_param.strWB_RGrGbB_L, _T("0,0,0,0"), _T("strWB_RGrGbB_L=Rmin,Grmin,Gbmin,Bmin"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strWB_RGrGbB_H"), m_param.strWB_RGrGbB_H, _T("255,255,255,255"), _T("strWB_RGrGbB_H=Rmax,Grmax,Gbmax,Bmax"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("strGoldenSampleName"), m_param.strGoldenSampleName, EMPTY_STR, _T("GoldenSampleName"));
		//add by wx

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strWB_LightConfident"), m_param.strWB_LightConfident, _T("1000,1000"), _T("iRG,iBG"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strWB_RGrGbB_Golden"), m_param.strWB_RGrGbB_G, _T("0,0,0,0"), _T("strWB_RGrGbB_G=R_g,Gr_g,Gb_g,B_g"));
		//LSC
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("LSC_ROI_Width"), m_param.nLSC_ROI_Width, -1, _T("LSC_ROI_Width"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("LSC_ROI_Height"),m_param.nLSC_ROI_Height, -1, _T("LSC_ROI_Height"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("LSC_Param"), m_param.lsc_param.cali_target, 50, _T("LSC_Param"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nShadingPower"), m_param.lsc_param.nShadingValue, 64, _T("nShadingPower"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("LSC_Spec"), m_param.lsc_param.ri_target_l, 0.1, _T("LSC_Spec"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("LSC_Spec_H"), m_param.lsc_param.ri_target_h, 0.8, _T("LSC_Spec_H"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("LSC_Delta_Spec"), m_param.lsc_param.ri_delta_target, 0.1, _T("LSC_Delta_Spec"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLSC_GOLDEN_SPEC"), m_param.dLSC_GOLDEN_SPEC, 0.25, _T("LSC_GOLDEN_SPEC"));

		//offset AF dac
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AF_OffsetStart"), m_param.af_param.off_sta, 0, _T("AF_OffsetStart"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AF_OffsetInf"), m_param.af_param.off_inf, 0, _T("AF_OffsetInf"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AF_OffsetMac"), m_param.af_param.off_mac, 0, _T("AF_OffsetMac"));
		
		//Check
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AF_SPEC_START_L"),m_param.otp_off_param.spec_af_sta_l, 0, _T("AF_SPEC_START_L"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AF_SPEC_START_H"),m_param.otp_off_param.spec_af_sta_h, 1023, _T("AF_SPEC_START_H"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AF_SPEC_INF_L"),m_param.otp_off_param.spec_af_inf_l, 0, _T("AF_SPEC_INF_L"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AF_SPEC_INF_H"),m_param.otp_off_param.spec_af_inf_h, 1023, _T("AF_SPEC_INF_H"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AF_SPEC_MAC_L"),m_param.otp_off_param.spec_af_mac_l, 0, _T("AF_SPEC_MAC_L"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AF_SPEC_MAC_H"),m_param.otp_off_param.spec_af_mac_h, 1023, _T("AF_SPEC_MAC_H"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("WB_Spec_RG_L"), m_param.otp_off_param.spec_r_g_l, 0.0, _T("WB_Spec_RG_L"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("WB_Spec_RG_H"),m_param.otp_off_param.spec_r_g_h, 5.0, _T("WB_Spec_RG_H"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("WB_Spec_BG_L"), m_param.otp_off_param.spec_b_g_l, 0.0, _T("WB_Spec_BG_L"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("WB_Spec_BG_H"), m_param.otp_off_param.spec_b_g_h, 5.0, _T("WB_Spec_BG_H"));

		//Gyro
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("bAutoControlShaker"), m_param.ois_param.nAutoControlShaker, 0, _T("bAutoControlShaker"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OIS_Param_ROISize"), m_param.ois_param.nROISize, 200, _T("OIS_Param_ROISize"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OIS_Param_nThresholdA"), m_param.ois_param.nThresholdA, 180, _T("OIS_Param_nThresholdA"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OIS_Param_nThresholdB"), m_param.ois_param.nThresholdB, 200, _T("OIS_Param_nThresholdB"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OIS_Param_dOIS_MinX_db"), m_param.ois_param.dOIS_MinX_db, 20.0, _T("OIS_Param_dOIS_MinX_db"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OIS_Param_dOIS_MinY_db"), m_param.ois_param.dOIS_MinY_db, 20.0, _T("OIS_Param_dOIS_MinY_db"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dGyroTurnStartX"), m_param.ois_param.dGyroTurnStartX,0.85,_T("OIS_Param_fGyroTurnStartX"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dGyroTurnEndX")  , m_param.ois_param.dGyroTurnEndX,0.95,_T("OIS_Param_fGyroTurnEndX"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dGyroTurnGapX")  , m_param.ois_param.dGyroTurnGapX,0.01,_T("OIS_Param_fGyroTurnGapX"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dGyroTurnStartY"), m_param.ois_param.dGyroTurnStartY,-0.95,_T("OIS_Param_fGyroTurnStartY"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dGyroTurnEndY")  , m_param.ois_param.dGyroTurnEndY,-0.85,_T("OIS_Param_fGyroTurnEndY"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dGyroTurnGapY")  , m_param.ois_param.dGyroTurnGapY,0.01,_T("OIS_Param_fGyroTurnGapY"));
		
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OISCheckHz"), m_param.ois_param.OISCheckHz, _T("2,6"), _T("OISCheckHz(2Hz,6Hz)"));
		
		
		//Qualcommm
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dWB_GOLDEN_SPEC_GBGR"), m_param.dWB_GOLDEN_SPEC_GBGR, 0.05, _T("Qualcomm WB_GOLDEN_SPEC_GBGR"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strLSC_GoldenSampleName"), m_param.strLSCGoldenSampleName, EMPTY_STR, _T("LSCGoldenSampleName"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nPDToleranceError"), m_param.pdaf_param.pdafverify.PDToleranceError, 30, _T("nPDToleranceError"));

		CString strComment,strValue;
		vector<int> vecValue;
		strComment.Format(_T("ROI(左,上,宽,高)"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("OISROIInfo"), strValue, _T("1916,1340,800,800"), strComment);
		SplitInt(strValue, vecValue);
		m_param.ois_param.OISROI.left = vecValue[0];
		m_param.ois_param.OISROI.top = vecValue[1];
		m_param.ois_param.OISROI.right = vecValue[0] + vecValue[2];
		m_param.ois_param.OISROI.bottom = vecValue[1] + vecValue[3];
		vecValue.clear();

		//OIS FW
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("FWFilePath"), m_param.ois_param.FWFilePath, _T("../FW/"), _T("FW Path"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("iFW_CorrectVer"), m_param.ois_param.nFW_CorrectVer, 0x050D, _T("FW ver"));

		//NonShaker
		vector<double> vecValue2;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("fltCode"), strValue, _T("130.0,230.0"), _T("(fltCodeMin,fltCodeMax)"));
		SplitDouble(strValue, vecValue2);
		m_param.ois_param.dfltCodeMin = vecValue2[0];
		m_param.ois_param.dfltCodeMax = vecValue2[1];
		vecValue2.clear();

		//FPN
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("FPN_dFPNSpec"), m_param.fpn_param.dFPNSpec, 0.5, _T("dFPNSpec"));
		vector<double> vecValue3;
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("FPN_dLTSpec"), strValue, _T("100.0,140.0"), _T("(FPN_LTMin,FPN_LTMax)"));
		SplitDouble(strValue, vecValue3);
		m_param.fpn_param.dltMinY = vecValue3[0];
		m_param.fpn_param.dltMaxY = vecValue3[1];
		vecValue3.clear();
		//Capture
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 0, _T("nReCapture"));
		
		//CCT
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nOTP_CCT_Ev"), m_param.otp_cct.EV, 500, _T("CCT EV"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nOTP_Cie_X"), m_param.otp_cct.Cie_x, 0, _T("CCT Cie X"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nOTP_Cie_Y"), m_param.otp_cct.Cie_y, 0, _T("CCT Cie Y"));


        return TRUE;
    }

    BOOL ImplOperator::SetOtpParam()
    {
        vector<int> vecChMin;
        vector<int> vecChMax;
        vector<int> vecOfflineColor;
		vector<int> vecChGolden;
		vector<int> vecChLight;
		//Check WB multi
		char strbuf[0x100];
		WCHAR hostname[MAX_COMPUTERNAME_LENGTH+1];

		unsigned long s = sizeof(hostname);
		if (!GetComputerName(hostname, &s)) {
			return FALSE;
		}
		
		USES_CONVERSION;

		//Get WB multi from DB
		if(uts.info.nOtpDBType == 0)
		{
			if (uts.otpdb->IsOtpDbConnected())
			{
				if (compare_float(m_param.dWB_RG_MULTI, 0) == 0) 
				{
					sprintf(strbuf, "%s-WB_RG_MULTI", T2A(hostname));
					if(uts.otpdb->setting_get(strbuf, &m_param.dWB_RG_MULTI) < 0)
					{
						uts.log.Error(_T("Set WB_RG_MULTI Error!"));
						return FALSE;
					}
				}

				if (compare_float(m_param.dWB_BG_MULTI, 0) == 0) 
				{
					sprintf(strbuf, "%s-WB_BG_MULTI", T2A(hostname));

					if (uts.otpdb->setting_get(strbuf, &m_param.dWB_BG_MULTI) < 0) 
					{
						uts.log.Error(_T("Set WB_RG_MULTI Error!"));
						return FALSE;
					}
				}
			}
			else
				return FALSE;
		}

		uts.log.Debug(_T("WB_RG_MULTI : %.2f"),m_param.dWB_BG_MULTI);
		uts.log.Debug(_T("WB_RG_MULTI : %.2f"),m_param.dWB_RG_MULTI);

        // Common
        SplitInt(m_param.strOTPTestOpt, m_vecOtpTestOpt);
        if (m_vecOtpTestOpt.size() <= 0)
        {
            uts.log.Error(_T("OtpTestOpt error. [strOTPTestOpt = %s]"), m_param.strOTPTestOpt);
            return FALSE;
        }
		
        m_pOtp->otp_param.SensorBlack_level = m_param.nSensorBlackLevel;
        m_pOtp->otp_param.group_for_use = m_param.nGROUP_FOR_USE;
		m_pOtp->otp_param.nEEPRomType  = m_param.nEEPRomType;


        // WB
        m_pOtp->otp_param.wb_param.roi.cx = m_param.nWB_ROI_Width;
        m_pOtp->otp_param.wb_param.roi.cy = m_param.nWB_ROI_Height;
        m_pOtp->otp_param.wb_param.rg_target = m_param.dWB_RG_TARGET;
        m_pOtp->otp_param.wb_param.bg_target = m_param.dWB_BG_TARGET;
        m_pOtp->otp_param.wb_param.rg_delta_spec = m_param.dWB_RG_DELTA_SPEC;
        m_pOtp->otp_param.wb_param.bg_delta_spec = m_param.dWB_BG_DELTA_SPEC;
        m_pOtp->otp_param.wb_param.golden_rate_spec = m_param.dWB_GOLDEN_SPEC;
		m_pOtp->otp_param.wb_param.golden_rate_spec_gbgr = m_param.dWB_GOLDEN_SPEC_GBGR;
        m_pOtp->otp_param.wb_param.golden_be_spec = m_param.dWB_BEGODLEN_SPEC;

        m_pOtp->otp_param.wb_param.goldenValidTime = m_param.nWB_GODLEN_ValidTime;
        m_pOtp->otp_param.wb_param.goldenWarningTime = m_param.nWB_GODLEN_WarnTime;
        m_pOtp->otp_param.wb_param.rg_multi = m_param.dWB_RG_MULTI;
        m_pOtp->otp_param.wb_param.bg_multi = m_param.dWB_BG_MULTI;

        SplitInt(m_param.strWB_GOLDEN_OFFLINE_RGrGbB, vecOfflineColor);

		if (vecOfflineColor.size() != sizeof(m_pOtp->otp_param.wb_param.off_golden))
        {
            uts.log.Error(_T("strWB_GOLDEN_OFFLINE_RGrGbB error. [strWB_GOLDEN_OFFLINE_RGrGbB = %s]"),
                m_param.strWB_GOLDEN_OFFLINE_RGrGbB);
            return FALSE;
        }
		
		SplitInt(m_param.strWB_RGrGbB_L, vecChMin);
        SplitInt(m_param.strWB_RGrGbB_H, vecChMax);
		SplitInt(m_param.strWB_RGrGbB_G, vecChGolden);
		SplitInt(m_param.strWB_LightConfident, vecChLight);
        if (vecChMin.size() != ARRAY_SIZE(m_pOtp->otp_param.wb_param.RGrGbB_min)
			|| vecChLight.size() != 2 
			|| vecChGolden.size() != ARRAY_SIZE(m_pOtp->otp_param.wb_param.RGrGbB_g)
			|| vecChMax.size() != ARRAY_SIZE(m_pOtp->otp_param.wb_param.RGrGbB_max))
        {
            uts.log.Error(_T("RGrGbB_min || RGrGbB_max error. [strWB_RGrGbB_L = %s][strWB_RGrGbB_H = %s]"),
                m_param.strWB_RGrGbB_L, m_param.strWB_RGrGbB_H);
            return FALSE;
        }
		if(vecChLight[0]<950||vecChLight[0]>1050||vecChLight[1]<950||vecChLight[1]>1050){
			uts.log.Debug(_T("光源系数有问题，请重新填写"));
			return FALSE;
		}
		m_pOtp->otp_param.LightConfident_RG = vecChLight[0];
		m_pOtp->otp_param.LightConfident_BG = vecChLight[1];
		m_pOtp->otp_param.nWB_Type  = m_param.nWB_Type;

        for (int i = 0; i < ARRAY_SIZE(m_pOtp->otp_param.wb_param.RGrGbB_min); i++)
        {
            m_pOtp->otp_param.wb_param.RGrGbB_min[i] = vecChMin[i];
            m_pOtp->otp_param.wb_param.RGrGbB_max[i] = vecChMax[i];
			m_pOtp->otp_param.wb_param.RGrGbB_g[i] = vecChGolden[i];
        }
        _tcscpy_s(m_pOtp->otp_param.wb_param.goldenSampleName,
            ARRAY_SIZE(m_pOtp->otp_param.wb_param.goldenSampleName),
            m_param.strGoldenSampleName.GetBuffer());
		
		memset(&m_pOtp->otp_param.wb_param.wb_data_c2,0,sizeof(WB_DATA_SHORT));
		memset(&m_pOtp->otp_param.wb_param.wb_data_c,0,sizeof(WB_DATA_UCHAR));
		memset(&m_pOtp->otp_param.wb_param.ratio,0,sizeof(WB_RATIO));
        memset(&m_pOtp->otp_param.wb_param.wb_data_c_aftercali, 0, sizeof(m_pOtp->otp_param.wb_param.wb_data_c_aftercali));
        m_pOtp->otp_param.wb_param.rg_delta = m_pOtp->otp_param.wb_param.bg_delta = 0;
        m_pOtp->otp_param.wb_param.ratio.g_a = m_pOtp->otp_param.wb_param.ratio_g.g_a = -1;
        memset(&m_pOtp->otp_param.wb_param.ratio_m, 0, sizeof(&m_pOtp->otp_param.wb_param.ratio_m));
        m_pOtp->otp_param.wb_param.delta_rgVSG = 0;
		m_pOtp->otp_param.wb_param.delta_bgVSG = 0;
		m_pOtp->otp_param.wb_param.delta_gbgrVSG = 0;
		
		//LSC
		m_pOtp->otp_param.lsc_param = m_param.lsc_param;
		m_pOtp->otp_param.lsc_param.roi.cx = m_param.nLSC_ROI_Width;
		m_pOtp->otp_param.lsc_param.roi.cy = m_param.nLSC_ROI_Height;
		m_pOtp->otp_param.lsc_param.golden_rate_spec = m_param.dLSC_GOLDEN_SPEC;
		memset(&m_pOtp->otp_param.lsc_param.riResult, 0, sizeof(m_pOtp->otp_param.lsc_param.riResult));

		_tcscpy_s(m_pOtp->otp_param.lsc_param.goldenSampleName,
			ARRAY_SIZE(m_pOtp->otp_param.lsc_param.goldenSampleName),
			m_param.strLSCGoldenSampleName.GetBuffer());
		
		//af
		m_pOtp->otp_param.af = m_param.af_param;
		m_pOtp->otp_param.af.sta = m_pOtp->otp_param.af.inf = m_pOtp->otp_param.af.mac = -1;
		m_pOtp->otp_param.af.raw_sta = m_pOtp->otp_param.af.raw_inf = m_pOtp->otp_param.af.raw_mac = -1;
		m_pOtp->otp_param.wb_param.rg_delta_after_cali = m_pOtp->otp_param.wb_param.bg_delta_after_cali = 0;

		//Check
		m_pOtp->otp_param.otp_off_param = m_param.otp_off_param;
		
		//OIS
		m_pOtp->otp_param.ois_param = m_param.ois_param;
		memset(&m_param.ois_param.oiscalResult,0,sizeof(m_param.ois_param.oiscalResult));
		memset(&m_param.ois_param.oischeckResult,0,sizeof(m_param.ois_param.oischeckResult));
		memset(&m_param.ois_param.oiscal_NonShakerResult,0,sizeof(m_param.ois_param.oiscal_NonShakerResult));

		//pdaf
		m_pOtp->otp_param.pdaf_param = m_param.pdaf_param;
		memset(&m_param.pdaf_param.pdafcalResult,0,sizeof(m_param.pdaf_param.pdafcalResult));
		
		//FPN
		m_pOtp->otp_param.fpn_param.dFPNSpec=m_param.fpn_param.dFPNSpec;
		m_pOtp->otp_param.fpn_param.dltMaxY=m_param.fpn_param.dltMaxY;
		m_pOtp->otp_param.fpn_param.dltMinY=m_param.fpn_param.dltMinY;

		//nReCapture
		m_pOtp->otp_param.nReCapture = m_param.nReCapture;
		uts.log.Debug(_T("SetOtpParam end"));

		//CCT
		m_pOtp->otp_param.cct.EV = m_param.otp_cct.EV;
		m_pOtp->otp_param.cct.Cie_x = m_param.otp_cct.Cie_x;
		m_pOtp->otp_param.cct.Cie_y = m_param.otp_cct.Cie_y;

		vecChMin.clear();
		vecChMax.clear();
		vecChGolden.clear();
		vecChLight.clear();
		vecOfflineColor.clear();
		
        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {		
        m_TimeCounter.SetStartTime();
        m_vecOtpTestOpt.clear();

        CString strRegName;
		*pnErrorCode = uts.errorcode.E_Pass;

        //-------------------------------------------------------------------------
		// get OtpCali Instance
        m_pOtp = GetOtpInstance(uts.info.strProjectName,m_pDevice);

        if (nullptr == m_pOtp)
        {
            uts.log.Error(_T("GetOtpInstance error. [ProjectName = %s]"), uts.info.strProjectName);
            *pnErrorCode = uts.errorcode.E_Fail;
            return FALSE;
        }
	
		//-------------------------------------------------------------------------
		// Set Otp Param
		if (!SetOtpParam())
		{
			uts.log.Error(_T("SetOtpParam error."));
			*pnErrorCode = uts.errorcode.E_Fail;
			return FALSE;
		}

        //-------------------------------------------------------------------------
		// Initialize
		OTP_INIT_PARAM init_param ;
		init_param.stOtpParam =  m_pOtp->otp_param;
		init_param.sn = uts.info.strSN;
		init_param.nEEPRomType = m_pOtp->otp_param.nEEPRomType;

		DDM::OtpDBPara &otpDBPara = init_param.otpDBPara;
		otpDBPara.dbType = uts.info.nOtpDBType;
		memcpy(otpDBPara.dbname, uts.info.strOtpDBName, 32);
		memcpy(otpDBPara.host, uts.info.strOtpDBHost, 32);
		memcpy(otpDBPara.passwd, uts.info.strOtpDBPassword, 32);
		memcpy(otpDBPara.user, uts.info.strOtpDBUser, 32);
		if (m_pOtp->Initialize_param(init_param) < 0)
		{
			uts.log.Error(_T("m_pOtp->Initialize error."));
			 *pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}
		//-------------------------------------------------------------------------

		uts.log.Info(_T("m_vecOtpTestOpt Size = %d")  , m_vecOtpTestOpt.size());
        // action
        TCHAR cur_sec[0x40] = {0};
        for (size_t i = 0; i < m_vecOtpTestOpt.size(); i++)
        {
			uts.log.Info(_T("In On Test4"));
            OTP_OPTION *opt = m_pOtp->get_option(m_vecOtpTestOpt[i]);
            if (!opt) continue;

			uts.log.Info(_T("start to run %s..."), opt->name);
			/*if ((opt->attr & OTP_ATTR_UP) && m_pOtp->is_otp_data_locked(m_pOtp->get_mid()))
            {
                uts.log.Warning(_T("Module[%s] has been locked, %s omit!"), 
                    uts.info.strSN, opt->name);
                continue;
            }*/
            if (opt->section && _tcscmp(cur_sec, opt->section))
            {
                _tcscpy(cur_sec, opt->section);
                strRegName = cur_sec;
                if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
                    strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
                {
                    uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
                    *pnErrorCode = uts.errorcode.E_Fail;
                    goto end;
                }
                Sleep(200);
            }

            switch (m_vecOtpTestOpt[i])
            {
            case OtpCaliOpt_ProgOtp:
				*pnErrorCode = m_pOtp->prog_otp(pbIsRunning) ;
                break;

            case OtpCaliOpt_ChkOtp:
				uts.log.Info(_T("ImplOperator：OtpCaliOpt_ChkOtp"));
                *pnErrorCode = m_pOtp->check_otp(pbIsRunning);
                break;
            
			case OtpCaliOpt_LscCali:
				uts.log.Info(_T("ImplOperator:OtpCaliOpt_LscCali"));
				*pnErrorCode = m_pOtp->LscCali(pbIsRunning) ;
                break;
            
			case OtpCaliOpt_WBCali:
				*pnErrorCode = m_pOtp->WBCali(pbIsRunning) ;
                break;
			
			case OtpCaliOpt_LOADLSC:
				*pnErrorCode = m_pOtp->check_otp_off_lsc(pbIsRunning) ;
				break;

			case OtpCaliOpt_LOADWB:
				*pnErrorCode = m_pOtp->check_otp_off_wb(pbIsRunning) ;
				break;

			case OtpCaliOpt_DUMP:
				*pnErrorCode = m_pOtp->DumpOtp(pbIsRunning) ;
				break;
			
			case OtpCaliOpt_SPC:
				*pnErrorCode = m_pOtp->SPCCali(pbIsRunning) ;
				break;

			case OtpCaliOpt_DCC0:
				*pnErrorCode = m_pOtp->DCC0Cali(pbIsRunning) ;
				break;

			case OtpCaliOpt_DCC1:
				*pnErrorCode = m_pOtp->DCC1Cali(pbIsRunning) ;
				break;

			case OtpCaliOpt_OffChk:
				*pnErrorCode = m_pOtp->otp_off_chk(pbIsRunning) ;
				break;

			case OtpCaliOpt_PDAFVerify:
				*pnErrorCode = m_pOtp->PDAFVerify(pbIsRunning) ;
				break;
			
			case OtpCaliOpt_LOADFW:
				*pnErrorCode = m_pOtp->LoadFW(pbIsRunning) ;
				break;

			case OtpCaliOpt_CHECKFW:
				*pnErrorCode = m_pOtp->CheckFW(pbIsRunning) ;
				break;

			case OtpCaliOpt_HallCali:
				*pnErrorCode = m_pOtp->HallCali(pbIsRunning) ;
				break;

			case OtpCaliOpt_GyroCali:
				*pnErrorCode = m_pOtp->GyroCali(pbIsRunning) ;
				break;

			case OtpCaliOpt_OISCheck:
				*pnErrorCode = m_pOtp->OISCheck(pbIsRunning) ;
				break;
			
			case OtpCaliOpt_SENSORSPC:
				*pnErrorCode = m_pOtp->SensorSPCCali(pbIsRunning) ;
				break;

			case OtpCaliOpt_GyroCaliNonShaker:
				*pnErrorCode = m_pOtp->GyroCali_NonShaker(pbIsRunning) ;
				break;

			case OtpCaliOpt_WBRAW10Cali:
				  uts.log.Info(_T("Into Normal WB Raw 10 Caliration (5100k)."));
				  m_pOtp->otp_param.wb_param.WBType = 1;
				  *pnErrorCode = m_pOtp->WBCali_RAW10(pbIsRunning) ;
				  break;

			case OtpCaliOpt_WB2RAW10Cali:
				  uts.log.Info(_T("Into 4000k WB Raw 10 Caliration."));
				  m_pOtp->otp_param.wb_param.WBType = 2;
				  *pnErrorCode = m_pOtp->WBCali_RAW10(pbIsRunning) ;
				  break;

			case OtpCaliOpt_WB3RAW10Cali:
				 uts.log.Info(_T("Into 3100k WB Raw 10 Caliration."));
				 m_pOtp->otp_param.wb_param.WBType = 3;
				  *pnErrorCode = m_pOtp->WBCali_RAW10(pbIsRunning) ;
				break;

			case OtpCaliOpt_CrossTalk:
				uts.log.Info(_T("Into OtpCaliOpt_CrossTalk Caliration."));
				*pnErrorCode = m_pOtp->CrossTalk(pbIsRunning) ;
				break;

			case OtpCaliOpt_LSCMTK:
				uts.log.Info(_T("Into OTPDB_OTPTYPE_LSCMTK Caliration."));
				*pnErrorCode = m_pOtp->LSCMTK_Cail(pbIsRunning) ;
				break;

            default:
                *pnErrorCode = uts.errorcode.E_Fail;
                break;
            }
	
			*pnErrorCode = m_pOtp->GetErrorCode();

            if (*pnErrorCode != 0)
            {
                uts.log.Error(_T("OtpTestOpt RUN error. [OtpTestOpt = %d]"), m_vecOtpTestOpt[i]);
                goto end;
            }
            uts.log.Info(_T("succeed to run %s"), opt->name);
        }

end:
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
        vecReturnValue.push_back(uts.errorcode.E_AWB);
        vecReturnValue.push_back(uts.errorcode.E_NVMWrite);
		vecReturnValue.push_back(NVM_FW				 );
		vecReturnValue.push_back(NVM_HALLCal		 );
        vecReturnValue.push_back(NVM_DATA_CONSIST    );
        vecReturnValue.push_back(NVM_WRITE           );
        vecReturnValue.push_back(NVM_CHECK           );
        vecReturnValue.push_back(NVM_DB_CONNECT      );
        vecReturnValue.push_back(NVM_BURNED_BEFORE   );
        vecReturnValue.push_back(NVM_NOINFO          ); 
        vecReturnValue.push_back(NVM_LSC             );
        vecReturnValue.push_back(NVM_WB              );
        vecReturnValue.push_back(NVM_BEGOLDEN        );
        vecReturnValue.push_back(AE_TIMEOUT          );
        vecReturnValue.push_back(ERR_QULCOMMLSC      );
        vecReturnValue.push_back(ERR_SPC             );
        vecReturnValue.push_back(ERR_DCC             );
		/*vecReturnValue.push_back(ERR_AUTO_DEV        );
		vecReturnValue.push_back(ERR_HALLCALI        );
		vecReturnValue.push_back(ERR_GYROCALI        );
		vecReturnValue.push_back(ERR_OISCHECK        );*/
		vecReturnValue.push_back(ERR_CCT   );	
    }

    void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(_T("OTP_WBLSC_TEST"));
		vecRegister.push_back(_T("OTP_GyroCali_TEST"));
    }
    
    void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,SensorID,TestTime(ms),Result,");
        strData.Format(_T("%s,%s,%s,%.1lf,%s,"),
            lpTime, uts.info.strSN,uts.info.strSensorId,m_TimeCounter.GetPassTime(), strResult);

        for (size_t i = 0; i < m_vecOtpTestOpt.size(); i++)
        {
            if (m_vecOtpTestOpt[i] == OtpCaliOpt_WBCali)
            {

				if (0 == _tcscmp(uts.info.strProjectName, _T("MF86A"))
					||0 == _tcscmp(uts.info.strProjectName, _T("HNF1065"))){
						strHeader += _T("R,Gr,Gb,B,R-g,Gr-g,Gb-g,B-g,RGdelta,BGdelta,RG-Confident,BG-Confident,RGdtaVSG,BGdtaVSG,");
						WB_PARAM *pwb_param = &m_pOtp->otp_param.wb_param;
						strData.AppendFormat(
							_T("%d,%d,%d,%d,%.4f,%.4f,%d,%d,%d,%d,%d,%d,%.4f,%.4f,")
							, pwb_param->wb_data_c2.R
							, pwb_param->wb_data_c2.Gr
							, pwb_param->wb_data_c2.Gb
							, pwb_param->wb_data_c2.B
							, pwb_param->RGrGbB_g[0]
							, pwb_param->RGrGbB_g[1]
							, pwb_param->RGrGbB_g[2]
							, pwb_param->RGrGbB_g[3]
							, pwb_param->rg_delta
							, pwb_param->bg_delta
							, m_pOtp->otp_param.LightConfident_RG 
							, m_pOtp->otp_param.LightConfident_BG 
							, pwb_param->delta_rgVSG
							, pwb_param->delta_bgVSG
							
							);
				}else{
                strHeader += _T("R,Gr,Gb,B,R/G,B/G,R[C],Gr[C],Gb[C],B[C],RGdelta,BGdelta,Multi_R,Multi_Gr,Multi_Gb,Multi_B,RGdtaVSG,BGdtaVSG,");
                WB_PARAM *pwb_param = &m_pOtp->otp_param.wb_param;
                strData.AppendFormat(
                    _T("%d,%d,%d,%d,%.4lf,%.4lf,%d,%d,%d,%d,%.4f,%.4f,%d,%d,%d,%d,%.4f,%.4f,")
                    , pwb_param->wb_data_c.R
                    , pwb_param->wb_data_c.Gr
                    , pwb_param->wb_data_c.Gb
                    , pwb_param->wb_data_c.B
					, pwb_param->ratio.dr_g
					, pwb_param->ratio.db_g
                    , pwb_param->wb_data_c_aftercali.R
                    , pwb_param->wb_data_c_aftercali.Gr
                    , pwb_param->wb_data_c_aftercali.Gb
                    , pwb_param->wb_data_c_aftercali.B
                    , pwb_param->rg_delta
                    , pwb_param->bg_delta
					, pwb_param->raw_after_multi.R
					, pwb_param->raw_after_multi.Gr
					, pwb_param->raw_after_multi.Gb
					, pwb_param->raw_after_multi.B
					, pwb_param->delta_rgVSG
					, pwb_param->delta_bgVSG);
				}
            }
            else if (m_vecOtpTestOpt[i] == OtpCaliOpt_ProgOtp)
            {
                 strHeader += _T("DataInDB,DataInSensor,");
                 int len = 2 * m_pOtp->otp_data_len + 3;
                 char *data_in_db = (char *)malloc(len);
                 char *data_in_sensor = (char *)malloc(len);
                 Array2String(m_pOtp->otp_data_in_db, m_pOtp->otp_data_len, data_in_db, len, m_pOtp->otp_data_len, "", "");
                 Array2String(m_pOtp->otp_data_in_sensor, m_pOtp->otp_data_len, data_in_sensor, len, m_pOtp->otp_data_len, "", "");
                 USES_CONVERSION;
                 strData.AppendFormat(_T("%s,%s,"), A2T(data_in_db), A2T(data_in_sensor));
                 free(data_in_db);
                 free(data_in_sensor);
            }
			else if (m_vecOtpTestOpt[i] == OtpCaliOpt_ChkOtp)
            {
                 strHeader += _T("DataInSensor,");
                 int len = 2 * m_pOtp->otp_data_len + 3;
                 char *data_in_sensor = (char *)malloc(len);
                 Array2String(m_pOtp->otp_data_in_sensor, m_pOtp->otp_data_len, data_in_sensor, len, m_pOtp->otp_data_len, "", "");
                 USES_CONVERSION;
                 strData.AppendFormat(_T("%s"), A2T(data_in_sensor));
                 free(data_in_sensor);
            }
			else if (m_vecOtpTestOpt[i] == OtpCaliOpt_LscCali)
			{
				if (0 == _tcscmp(uts.info.strProjectName, _T("MA702"))
				 ||(0 == _tcscmp(uts.info.strProjectName, _T("MA862")))
				 ||(0 == _tcscmp(uts.info.strProjectName, _T("MA85V")))
				 ||(0 == _tcscmp(uts.info.strProjectName, _T("HNF1065")))
				 || (0 == _tcscmp(uts.info.strProjectName, _T("MA84F"))))
				{
					LSC_PARAM  *lsc = &m_pOtp->otp_param.lsc_param;
					WB_PARAM  *wb = &m_pOtp->otp_param.wb_param;
					
					/*
					strHeader += _T("LSCMaxdelta,RG,BG,GbGr,DeltaVSG_RG,DeltaVSG_BG,DeltaVSG_GbGr");
					strData.AppendFormat(
						_T("%.4f,%d,%d,%d,%.4f,%.4f,%.4f,")
						, lsc->maxdeltarate
						,wb->raw_after_multi2.RG,wb->raw_after_multi2.BG,wb->raw_after_multi2.GbGr
						,wb->delta_rgVSG,wb->delta_bgVSG,wb->delta_gbgrVSG);
					*/
					strHeader += _T("LSCMaxdelta,RG,BG,GbGr,DeltaVSG_RG,DeltaVSG_BG,DeltaVSG_GbGr,");
					strData.AppendFormat(
						_T("%.4f,%d,%d,%d,%.4f,%.4f,%.4f,")
						, lsc->maxdeltarate
						,wb->raw_after_multi2.RG,wb->raw_after_multi2.BG,wb->raw_after_multi2.GbGr
						,wb->delta_rgVSG,wb->delta_bgVSG,wb->delta_gbgrVSG);

					for (int i = 0 ;i<221 ;i++)
					{
						strHeader.AppendFormat(_T("LSC_R_%d,"),i);
						strData.AppendFormat(_T("%d,"),lsc->LSCTable.R[i]);
					}
					for (int i = 0 ;i<221 ;i++)
					{
						strHeader.AppendFormat(_T("LSC_GR_%d,"),i);
						strData.AppendFormat(_T("%d,"),lsc->LSCTable.GR[i]);
					}
					for (int i = 0 ;i<221 ;i++)
					{
						strHeader.AppendFormat(_T("LSC_Gb_%d,"),i);
						strData.AppendFormat(_T("%d,"),lsc->LSCTable.Gb[i]);
					}
					for (int i = 0 ;i<221 ;i++)
					{
						strHeader.AppendFormat(_T("LSC_B_%d,"),i);
						strData.AppendFormat(_T("%d,"),lsc->LSCTable.B[i]);
					}					
				}
				else if (0 == _tcscmp(uts.info.strProjectName, _T("CA225"))
					   ||(0 == _tcscmp(uts.info.strProjectName, _T("CA226"))))
				{
					strHeader += _T("TableResult,VerifyResult,RI,BIdelta,AvgCenter,");
					LSC_PARAM  *lsc = &m_pOtp->otp_param.lsc_param;

					CString str_TableResult = (lsc->bLSCTable? PASS_STR : FAIL_STR);
					CString str_VerifyResult = (lsc->bLSCVerify ? PASS_STR : FAIL_STR);

					strData.AppendFormat(
						_T("%s,%s,%.2f,%.2f,%d,")
						, str_TableResult
						, str_VerifyResult
						, lsc->riResult.dRI
						, lsc->riResult.dRIDelta
						, lsc->dAvgCenterG);

				}
				else
				{
					strHeader += _T("TableResult,VerifyResult,RI,BIdelta,");
					LSC_PARAM  *lsc = &m_pOtp->otp_param.lsc_param;

					CString str_TableResult = (lsc->bLSCTable? PASS_STR : FAIL_STR);
					CString str_VerifyResult = (lsc->bLSCVerify ? PASS_STR : FAIL_STR);

					strData.AppendFormat(
						_T("%s,%s,%.5f,%.5f,")
						, str_TableResult
						, str_VerifyResult
						, lsc->riResult.dRI
						, lsc->riResult.dRIDelta);
				}

			}
			else if(m_vecOtpTestOpt[i] == OtpCaliOpt_HallCali)
			{
				if (0 == _tcscmp(uts.info.strProjectName, _T("CA188")))
				{
					strHeader += _T("Hall_errorCode,");
					strData.AppendFormat(_T("0x%x,"), m_pOtp->unHall_errorCode);
					strHeader += _T("X_Center,"); //0
					strHeader += _T("X_Before Max,"); //1
					strHeader += _T("X_After Max,"); //2
					strHeader += _T("X_Before Min,"); //3
					strHeader += _T("X_After Min,"); //4
					strHeader += _T("X_Hall Bias,"); //5
					strHeader += _T("X_HallOffset,"); //6
					strHeader += _T("X_AD Hall Offset,"); //7
					strHeader += _T("X_Loop Gain,"); //8
					strHeader += _T("X_Gyro Offset,"); //9

					strHeader += _T("Y_Center,"); //10
					strHeader += _T("Y_Before Max,"); //11
					strHeader += _T("Y_After Max,"); //12
					strHeader += _T("Y_Before Min,"); //13
					strHeader += _T("Y_After Min,"); //14
					strHeader += _T("Y_Hall Bias,"); //15
					strHeader += _T("Y_HallOffset,"); //16
					strHeader += _T("Y_AD Hall Offset,"); //17
					strHeader += _T("Y_Loop Gain,"); //18
					strHeader += _T("Y_Gyro Offset,"); //19

					for (int i = 0 ;i < 20 ;i++)
					{
						strData.AppendFormat(_T("0x%x,"), m_pOtp->HallCaliData[i]);
					}
					
				}
				else if ((0 == _tcscmp(uts.info.strProjectName, _T("CA210")))||(0 == _tcscmp(uts.info.strProjectName, _T("CA211"))))
				{
					strHeader += _T("Hall_errorCode,");
					strData.AppendFormat(_T("0x%x,"), m_pOtp->unHall_errorCode);
					strHeader += _T("HlaBia,"); //0
					strHeader += _T("HlaOff,"); //1
					strHeader += _T("HlaMax,"); //2
					strHeader += _T("HlaMin,"); //3

					for (int i = 0 ;i < 4 ;i++)
					{
						strData.AppendFormat(_T("0x%x,"), m_pOtp->HallCaliData[i]);
					}
				}

			}
			else if(m_vecOtpTestOpt[i] == OtpCaliOpt_DCC0 || m_vecOtpTestOpt[i] == OtpCaliOpt_DCC1)
			{
				if (0 == _tcscmp(uts.info.strProjectName, _T("CA188"))
					||(0 == _tcscmp(uts.info.strProjectName, _T("CA210")))
					||(0 == _tcscmp(uts.info.strProjectName, _T("CA211"))))
				{
					PDAF_PAPRAM *pdaf = &m_pOtp->otp_param.pdaf_param;

					strHeader += _T("PD1,PD2,");
					strData.AppendFormat(
						_T("%d,%d,")
						,pdaf->pdafcalResult.PD1
						,pdaf->pdafcalResult.PD2);
				}
			}
			else if(m_vecOtpTestOpt[i] == OtpCaliOpt_PDAFVerify)
			{
				PDAFVERIFY_PAPRAM *pdaf = &m_pOtp->otp_param.pdaf_param.pdafverify;

				strHeader += _T("SobelDAC,PDAFInitDAC,VerifyDAC,PDAFVerifyError,MTKLinearityTest");
				strData.AppendFormat(
					_T("%d,%d,%d,%.2f,%.2f,")
					,pdaf->pdafResult.SobelDAC
					,pdaf->pdafResult.PDAFInitDAC
					,pdaf->pdafResult.PDAFVerifyDAC
					,pdaf->pdafResult.PDAFVerifyError
					,pdaf->pdafResult.PDAFMTKLinearity
					);
			}
			else if(m_vecOtpTestOpt[i] == OtpCaliOpt_GyroCali)
			{
				OIS_PAPRAM  *ois = &m_pOtp->otp_param.ois_param;
				CString temp;

				for (double x = ois->dGyroTurnStartX ; 
					x < ois->dGyroTurnEndX;  
					x +=ois->dGyroTurnGapX)
				{				
					temp.Format(_T("X=%.3f,"),x);
					strHeader += temp;
				}

				for (double y =  ois->dGyroTurnStartY ; 
					y <  ois->dGyroTurnEndY;  
					y += ois->dGyroTurnGapY)
				{		
					temp.Format(_T("Y=%.3f,"),y);
					strHeader += temp;
				}

				strHeader += _T("Calx,Caly,");

				for(int x=0 ; x < ois->oiscalResult.countX; x++)
				{						
					strData.AppendFormat(_T("%.2f,"), ois->oiscalResult.PointCountX[1][x]);
				}

				for(int y=0 ; y < ois->oiscalResult.countY; y++)
				{						
					strData.AppendFormat(_T("%.2f,"), ois->oiscalResult.PointCountY[1][y]);
				}

				strData.AppendFormat(_T("%.2f,"), ois->oiscalResult.uCalX);
				strData.AppendFormat(_T("%.2f,"), ois->oiscalResult.uCalY);
			}
			else if(m_vecOtpTestOpt[i] == OtpCaliOpt_OISCheck)
			{
				OIS_PAPRAM  *ois = &m_pOtp->otp_param.ois_param;

				//strHeader += _T("StaticCircleRECT.LEFT,StaticCircleRECT.RIGHT,StaticCircleRECT.TOP,StaticCircleRECT.BOTTOM,");
				strHeader += _T("StaticCircleWidth,StaticCircleHeight,");

				int TestHz;
				vector<int> vecOISCheck;
				SplitInt(ois->OISCheckHz, vecOISCheck);

				for (size_t i = 0 ; i< vecOISCheck.size() ; i++)
				{
					TestHz = vecOISCheck[i];

					strHeader.AppendFormat(_T("OISOFFCircleWidth_%dHz,OISOFFCircleHeight_%dHz,")
						,TestHz
						,TestHz);
					
					strHeader.AppendFormat(_T("DynamicCircleWidth_%dHz,DynamicCircleHeight_%dHz,")
						,TestHz
						,TestHz);
					
					strHeader += _T("Ratio_x,Ratio_y,DB_x,DB_y,");
				}
				strData.AppendFormat(_T("%d,%d,"),ois->oischeckResult.Vib[0].StaticCircleWidth
												 ,ois->oischeckResult.Vib[0].StaticCircleHeight);

				for (size_t i = 0 ; i< vecOISCheck.size() ; i++)
				{					
					strData.AppendFormat(_T("%d,%d,"),ois->oischeckResult.Vib[i].OISOFFCircleWidth
													 ,ois->oischeckResult.Vib[i].OISOFFCircleHeight);

					strData.AppendFormat(_T("%d,%d,"),ois->oischeckResult.Vib[i].DynamicCircleWidth
													 ,ois->oischeckResult.Vib[i].DynamicCircleHeight);

					strData.AppendFormat(_T("%.2f,%.2f,"),ois->oischeckResult.Vib[i].ratio_x
														 ,ois->oischeckResult.Vib[i].ratio_y);

					strData.AppendFormat(_T("%.2f,%.2f,"),ois->oischeckResult.Vib[i].DB_x
														 ,ois->oischeckResult.Vib[i].DB_y);
				}
			}
			else if(m_vecOtpTestOpt[i] == OtpCaliOpt_GyroCaliNonShaker)
			{
				OIS_PAPRAM  *ois = &m_pOtp->otp_param.ois_param;

				strHeader.AppendFormat(_T("Yaw_Dist,Pitch_Dist,Yaw_MoveDiff,Pitch_MoveDiff,Yaw_Gain,Pitch_Gain,RealGainX,RealGainY,"));
				strData.AppendFormat(_T("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,"),ois->oiscal_NonShakerResult.Yaw_Dist
																		 ,ois->oiscal_NonShakerResult.Pitch_Dist
																		 ,ois->oiscal_NonShakerResult.Yaw_MoveDiff
																		 ,ois->oiscal_NonShakerResult.Pitch_MoveDiff
																		 ,ois->oiscal_NonShakerResult.Yaw_Gain
																		 ,ois->oiscal_NonShakerResult.Pitch_Gain
																		 ,ois->oiscal_NonShakerResult.RealGainX
																		 ,ois->oiscal_NonShakerResult.RealGainY);
			}
			else if((m_vecOtpTestOpt[i] == OtpCaliOpt_WBRAW10Cali)||(m_vecOtpTestOpt[i] == OtpCaliOpt_WB2RAW10Cali)||(m_vecOtpTestOpt[i] == OtpCaliOpt_WB3RAW10Cali))
			{
				strHeader += _T("R,Gr,Gb,B,");
				WB_PARAM *pwb_param = &m_pOtp->otp_param.wb_param;
				strData.AppendFormat(
					_T("%d,%d,%d,%d,")
					, pwb_param->wb_data_c2.R
					, pwb_param->wb_data_c2.Gr
					, pwb_param->wb_data_c2.Gb
					, pwb_param->wb_data_c2.B);
			}

        }

        strHeader += _T("Version,OP_SN\n");
        strData.AppendFormat(_T("%s,%s\n"), strVersion, uts.info.strUserId);
    }

	void ImplOperator::OnPreviewStart()
	{
		uts.log.Debug(_T("Focus PreviewStart"));
		if (0 == _tcscmp(uts.info.strProjectName, _T("CA188")))
		{
			DLLSetVCM_Move(m_pDevice, uts.info.nVCMType, 0);
		}
		else if ((0 == _tcscmp(uts.info.strProjectName, _T("CA210")))|| 
			(0 == _tcscmp(uts.info.strProjectName, _T("CA211"))))
		{
			unsigned char tmp[6];
			uint16_t HallData[4];
			USES_CONVERSION;

			int ret = uts.otpdb->GetOtpByType(uts.otpdb->GetModuleID(T2A(uts.info.strSN)),11 , 
				(char *)tmp, sizeof(tmp));
			if (ret < 0)
			{
				uts.log.Error(_T("get Hall data error!!!"));
				return;
			}

			HallData[0] = get_le_val(tmp + 0, 1);
			HallData[1] = get_le_val(tmp + 1, 1);
			HallData[2] = get_le_val(tmp + 2, 2);
			HallData[3] = get_le_val(tmp + 4, 2);

			DLLSetVCM_Initial(m_pDevice, uts.info.nVCMType,HallData);
		}
		else
			DLLSetVCM_Initial(m_pDevice, uts.info.nVCMType,NULL);
	}

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
