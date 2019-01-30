#pragma once
#include "Export.h"
#include "DBLocalConfig.h"

namespace UTS
{
    class UTS_FRAMEWORK_API ErrorCode
    {
    public:
        BOOL GetAllErrorCodeInfoFromDB();
        CString GetErrorMsg(LPCTSTR lpOperatorFileName, int nErrorcode);

        //------------------------------------------------------------------------------
        // error code define
    public:
		static const int E_PassA            = (-100);        // Pass A, 成功分级为A
		static const int E_PassB            = (-101);        // Pass B, 成功分级为B
		static const int E_PassC            = (-102);        // Pass C, 成功分级为C
		static const int E_Fail             = (900);        // Fail, 失败、其他错误
		static const int E_Pass             = (0 );          // Pass, 成功
		static const int E_DefectPixel      = (312 );          // Defect Pixel,壞點
		static const int E_FocusAdjustment  = (208 );          // Focus Adjustment,調焦不良
		static const int E_FocusCheck       = (206 );          // Focus Check,解析度不良
		static const int E_Blemish          = (301 );          // Blemish,污點 
		static const int E_WhitePixel       = (309 );          // White Pixel,白點
		static const int E_Current          = (104 );          // Current,?流?常
		static const int E_RI               = (318 );          // RI,暗角不良
		static const int E_ColorCheck       = (8 );          // Color Check,色彩
		static const int E_NVMDataConsist   = (509);          // NVM Data Consist,來料有數值
		static const int E_NVMWrite         = (518);          // NVM Write, ??體?入失敗
		static const int E_NVMCheck         = (515);          // NVM Check,??體檢查失敗
		static const int E_OC               = (316);          // OC,光學中心
		static const int E_RU               = (320);          // RU,亮度均勻度
		static const int E_CU               = (303);          // CU,?色均勻度
		static const int E_DarkNG           = (306);          // Dark NG,暗板
		static const int E_Tilt             = (16);          // Tilt ,傾斜
		static const int E_Glicth           = (17);          // Glicth, 抖動
		static const int E_VCM              = (18);          // VCM,音圈馬達不動
		static const int E_Peak             = (19);          // Peak,峰?不良
		static const int E_AWB              = (412);          // AWB,白平衡
		static const int E_ALS              = (21);          // ALS ,光感
		static const int E_Temperature      = (22);          // Temperature,溫度
		static const int E_ThermalBlemish   = (23);          // Thermal Blemish,熱汙點
		static const int E_NoImage          = (619);          // No Image,圖像不良（黑屏）
		static const int E_ColorCal         = (25);          // Color Cal,色彩校?
		static const int E_Ripple           = (26);          // Ripple,水紋
		static const int E_AbormalImage     = (27);          // Abormal Image,?色
		static const int E_DataLine         = (28);          // Data Line,數據丟失
		static const int E_Gray             = (29);          // Gray,灰階
		static const int E_DFOV             = (30);          // DFOV,?角
		static const int E_LED              = (31);          // LED,燈測?不良
		static const int E_MIC              = (32);          // MIC,?克風
		static const int E_Aligment         = (33);          // Alignment,對准測?
		static const int E_NoiseLine        = (34);          // Noise Line,?條
		static const int E_SNScan           = (534);          // SN Scan,序列?無法掃描
		static const int E_Band             = (535);          // Band,粗?條不良
		static const int E_Linumance        = (616);          // Linumance,Y值不良
		static const int E_OpenShort        = (38);          // Open/Short,開短路 
		static const int E_SNR              = (39);          // SNR ,傳感器雜?
		static const int E_FocusDelta       = (40);          // Focus Delta ,解像力不均勻
		static const int E_Separation       = (41);          // Separation , 曲?離散
		static const int E_InitSFR          = (42);          // Init SFR, 啟動解像力不良
		static const int E_DBConnect        = (501);          // DB Connect, 数据库连接失败
		static const int E_ReBurn           = (44);          // 模组已烧录, 不允许更新DB数据
		static const int E_InfoReady        = (45);          // 模组烧录时待烧录信息不全
		static const int E_TvDistortion     = (46);          // TvDistortion, 畸变不良
		static const int E_Rotate           = (47);          // Rotate, 回转不良
		static const int E_RelativeShift    = (48);          // RelativeShift, 相对位移不良
		static const int E_ColorLevel       = (49);          // ColorLevel, 色再现不良
		static const int E_IR_LED           = (50);          // IR_LED, IR-LED 点灯不良
		static const int E_AE               = (325);          // AE, AE不良
		static const int E_ColorShading     = (52);          // ColorShading, ColorShading不良
		static const int E_FocusDiff        = (53);          // FocusDiff, 解像度差不良
		static const int E_ColorLevelDiff   = (54);          // ColorLevelDiff, 色味差不良
		static const int E_NVM_LSC          = (314);
		static const int E_NVM_WB           = (409);
		static const int E_NVM_Begolden     = (57);
		static const int E_AE_TimeOut       = (58);
		static const int E_ERR_Rulcommlsc   = (314);
		static const int E_ERR_MTFDelta		= (67);
		static const int E_ERR_ThroughFocus	= (68);
		static const int E_DarkNoise		= (69);
		static const int E_FixPatternNoise	= (308);
		static const int E_CheckOCBin	    = (71);
		static const int E_GetSN            = (109); 
		static const int E_DarkLine         = (150);
		static const int E_DarkCorner       = (151);
		static const int E_LineDetect       = (152);
		static const int E_ColorRatioBG     = (410); 
		static const int E_ColorRatioRG     = (411); 
		static const int E_GetSensorID      = (522); 

		static const int E_XCal_Fail           = (200);        //  [8/14/2017 Cindy] XCalibration error code
		static const int E_XCal_Roll           = (201); 
		static const int E_XCal_Pitch          = (202); 
		static const int E_XCal_Yaw            = (203); 
		static const int E_XCal_Tilt           = (204); 
		static const int E_XCal_OCShift_X1     = (205); 
		static const int E_XCal_OCShift_Y1     = (206); 
		static const int E_XCal_OCShift_X2     = (207); 
		static const int E_XCal_OCShift_Y2     = (208); 
		static const int E_XCal_OCShift_TotalX = (209); 
		static const int E_XCal_OCShift_TotalY = (210); 
		static const int E_XCal_Tx             = (211); 

		static const int E_SFC_CheckRoute      = (531); 
		static const int E_SFC_CommitData      = (532); 

		static const int E_DIT_Shading            = (651);
		static const int E_DIT_ColorShading       = (652);
		static const int E_DIT_ColorRatio         = (653);
		static const int E_DIT_LU_Particle        = (654);
		static const int E_DIT_DeadPixel          = (655);
		static const int E_DIT_Blemish            = (656);
		static const int E_DIT_LightUniformity    = (657);
		static const int E_DIT_LensTilt           = (658);
		static const int E_DIT_HotPixel           = (659);
		static const int E_DIT_LU_LineDetect      = (660);
		static const int E_DIT_SFR_INF            = (661);
		static const int E_DIT_SFR_MUP            = (662);
		static const int E_DIT_SFR                = (663);
		static const int E_DIT_SFR_Circle         = (664);
		static const int E_DIT_Y_Check            = (665);
		static const int E_DIT_AF_Data_Check      = (666);
		static const int E_DIT_AFHorCaliCheck_INF = (667);
		static const int E_DIT_AFHorCaliCheck_MID = (668);
		static const int E_DIT_AFHorCaliCheck_MUP = (669);
		static const int E_DIT_PDAFCaliCheck      = (670);
		static const int E_DIT_OIS_H7cm_MID       = (671);
		static const int E_DIT_OIS_H14cm_MUP      = (672);
		static const int E_DIT_CheckDongle        = (673);
		static const int E_DIT_SetSN              = (674);

    public:
        vector<OPERATOR_ERRORCODE> m_vecAllSetting;
    };
}
