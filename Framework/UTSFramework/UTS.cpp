#include "StdAfx.h"
#include "UTS.h"
#include "otpdb.h"

namespace UTS
{
    UTSGD &uts = UTSGD::GetInstance();

    UTSGD::UTSGD()
    {
    }

    UTSGD::~UTSGD()
    {
    }

    UTSGD& UTSGD::GetInstance()
    {
        static UTSGD instance;
        return instance;
    }

    BOOL UTSGD::InitializeUTS(
        HWND hMainWnd,
        HWND hDisplayWnd,
        HWND hInfoBoardWnd)
    {
        uts.info.bShowFPS = FALSE;
        uts.info.hMainWnd = hMainWnd;
        uts.info.hpDisplayWnd = hDisplayWnd;
        uts.info.hInfoboardWnd = hInfoBoardWnd;
        uts.info.nDeviceIndex = 0;
		uts.info.nMesCommit = FALSE;
        //------------------------------------------------------------------------------
        // Initialize list log
        if (!uts.log.Initialize())
        {
            AfxMessageBox(_T("Initialize list log error."), MB_ICONERROR);
            return FALSE;
        }

        //------------------------------------------------------------------------------
        // 初始化local config DB
        BOOL bRet = uts.dbCof.Initialize();
        if (!bRet)
        {
            uts.log.Error(_T("[%s]Local config DB Initialize Fail."), UTS_FN);
            AfxMessageBox(_T("Local config DB Initialize Fail."), MB_ICONERROR);
            return FALSE;
        }
        else
        {
            uts.log.Info(_T("[%s]Local config DB Initialize Success."), UTS_FN);
        }

#ifdef DEBUG
       uts.info.nUserType = USERTYPE_SOFTWARE;
        uts.info.strUserId = _T("0240897");
#else
        //------------------------------------------------------------------------------
        // 命令行
        CString strCmdLine = ::GetCommandLine();
        vector<CString> vecParam;
        SplitString(strCmdLine, vecParam, _T("|"), FALSE);
        if (vecParam.size() != 4)
        {
            AfxMessageBox(_T("Please run UTS by UTSLauncher."), MB_ICONERROR);
            CString strCmd;
            strCmd.Format(_T("%s%s"), uts.info.strBinPath, _T("UTSLauncher.exe"));
            OSUtil::ExcuteCommand(strCmd, FALSE);
            Sleep(1000);
            PostQuitMessage(0);
            return FALSE;
        }
        uts.info.nUserType = _ttoi(vecParam[1]);
        uts.info.strUserId = vecParam[2];

        //------------------------------------------------------------------------------
        // 检查密码
        CStringA strPassword;
        if (!uts.dbCof.GetUserPassword(uts.info.nUserType, strPassword))
        {
            AfxMessageBox(_T("Read DB error."), MB_ICONERROR);
            return FALSE;
        }
        USES_CONVERSION;
        if (vecParam[3] != A2T(strPassword))
        {
            AfxMessageBox(_T("Password wrong."), MB_ICONERROR);
            PostQuitMessage(0);
            return FALSE;
        }
#endif  

#ifndef CONFIG_FROM_FILE
        uts.dbCof.GetProjectSingleSetting(_T("nShopFlowEn"), uts.info.nShopFlowEn, 0, _T("0: Disable, 1: Enable"));
		uts.dbCof.GetProjectSingleSetting(_T("nMesEn"), uts.info.nMesEn, 0, _T("0: Disable, 1: Enable"));
		uts.dbCof.GetProjectSingleSetting(_T("nMesOffice"), uts.info.nMesOffice, 1, _T("1: Test at Office,0:Test at Production Line"));
        uts.dbCof.GetProjectSingleSetting(_T("strProjectType"), uts.info.strProjectType, _T("CCM"), _T("Project type[CCM]"));
        uts.dbCof.GetProjectSingleSetting(_T("ProjectName"), uts.info.strProjectName, _T("HNF1065"), _T("Name of Project."));
        uts.dbCof.GetProjectSingleSetting(_T("strLineName"), uts.info.strLineName, _T("Line001"), _T("Name of Line."));
        uts.dbCof.GetProjectSingleSetting(_T("strStationCode"), uts.info.strStationCode, _T(""), _T("Station code."));
        uts.dbCof.GetProjectSingleSetting(_T("strStationName"), uts.info.strStationName, _T("OTP"), _T("Name of Station."));
        uts.dbCof.GetProjectSingleSetting(_T("strPositionCode"), uts.info.strPositionCode, _T(""), _T("Position code."));
        uts.dbCof.GetProjectSingleSetting(_T("strPositionName"), uts.info.strPositionName, _T(""), _T("Position name."));
        uts.dbCof.GetProjectSingleSetting(_T("strShopFlowFilePath"), uts.info.strShopFlowFilePath, _T("D:\\ShopFlowFilePath\\"), _T("Path for save shopflow files."));

        uts.dbCof.GetProjectSingleSetting(_T("nDeviceId"), uts.info.nDeviceId, 1, _T("Id of Device. [1: UTDV_File 2: UTDV_V5U 3: UTDV_UVC]"));
		uts.dbCof.GetProjectSingleSetting(_T("nSensorType"), uts.info.nSensorType, 0, _T("0: Sensor_Virtual ,1:Sensor_AR1337,2:Sensor_GC2385,3:Sensor_GC5024,\
																						 4:Sensor_GC5025,5:Sensor_GC5034,6:Sensor_GC8024,6:Sensor_Hi546,7:Sensor_Hi556,\
																						 8:Sensor_Hi846,9:Sensor_Hynix1333,10:Sensor_Hynix843,11:Sensor_IMX214,\
																						 12:Sensor_IMX362,13:Sensor_IMX363,14:Sensor_IMX386,15:Sensor_IMX499,16:Sensor_IMX519,\
																						 17:Sensor_OV12870,18:Sensor_OV12A10,19:Sensor_OV13855,20:Sensor_OV13858,\
																						 21:Sensor_OV16885,22:Sensor_OV4688,23:Sensor_OV5648,24:Sensor_OV5675,25:Sensor_OV5695,\
																						 26:Sensor_OV8856,27:Sensor_OV9750,28:Sensor_S5K2L7,29:Sensor_S5K2P7,30:Sensor_S5K3L6,\
																						 31:Sensor_S5K3L8XX,32:Sensor_S5K3P3,33:Sensor_S5K3P8,34:Sensor_S5K3P9SP,35:Sensor_S5K3P9SX,\
																						 36:Sensor_S5K4H7YX,37:Sensor_S5K4H8YX,38:Sensor_S5K5E2YA,39:Sensor_S5K5E8YX,\
																						 40:Sensor_S5K5E9,41:Sensor_SP0A09,42:Sensor_SP2509V,43:Sensor_SP250A,44:Sensor_SUPPORT_NUM,"));
		uts.dbCof.GetProjectSingleSetting(_T("nOISType"), uts.info.nOISType, 0, _T("OIS type."));
		uts.dbCof.GetProjectSingleSetting(_T("nVCMType"), uts.info.nVCMType, 0, _T("0:Internal, 1:LC898214, 2:BU6420, 3:GT9762, 4:ZC533 ,5:AK737,6:DW9718S,\
																				   7:DW9800,8:DW9714V,9:CH9914,10:CN3927B,11:ZC524,12:EP5510E2"));
																						
		uts.dbCof.GetProjectSingleSetting(_T("nGetFrameWaitTime"), uts.info.nGetFrameWaitTime,50, _T("GetFrameWaitTime."));

        uts.dbCof.GetProjectSingleSetting(_T("nLTDD_DummyFrame"), uts.info.nLTDD_DummyFrame, 0, _T("Droped Frame count before capture in Light & Dark test."));
        uts.dbCof.GetProjectSingleSetting(_T("nLTDD_AvgFrame"), uts.info.nLTDD_AvgFrame, 1, _T("Average Frame count when capture in Light & Dark test."));
        uts.dbCof.GetProjectSingleSetting(_T("nSaveBmpFile"), uts.info.nSaveBmpFile, 0, _T("0: No 1:Yes"));
        uts.dbCof.GetProjectSingleSetting(_T("nSaveRawFile"), uts.info.nSaveRawFile, 0, _T("0: No 1:Yes"));
        uts.dbCof.GetProjectSingleSetting(_T("nSaveYuvBmpFile"), uts.info.nSaveYuvBmpFile, 0, _T("0: No 1:Yes"));

        uts.dbCof.GetProjectSingleSetting(_T("nZipFile"), uts.info.nZipFile, 0, _T("0: No 1:Yes"));
        uts.dbCof.GetProjectSingleSetting(_T("nStopOnFail"), uts.info.nStopOnFail, 1, _T("Stop test at once when fail. 0: No 1:Yes"));
        uts.dbCof.GetProjectSingleSetting(_T("strVersion"), uts.info.strVersion, _T("AppMain.AppSub.ProjectMain.ProjectSub.TestPlan"), _T("Application version shown on Title."));

        uts.dbCof.GetProjectSingleSetting(_T("nOtpDBType"), uts.info.nOtpDBType, -1, _T("-1: OFF, 0: MYSQL"));
        uts.dbCof.GetProjectSingleSetting(_T("strOtpDBHost"), uts.info.strOtpDBHost, _T("127.0.0.1"), _T("Otp DB host address"));
        uts.dbCof.GetProjectSingleSetting(_T("strOtpDBUser"), uts.info.strOtpDBUser, _T("root"), _T("Otp DB user"));
        uts.dbCof.GetProjectSingleSetting(_T("strOtpDBPassword"), uts.info.strOtpDBPassword, _T("123.abc"), _T("Otp DB password"));
        uts.dbCof.GetProjectSingleSetting(_T("strOtpDBName"), uts.info.strOtpDBName, _T("hnf1065"), _T("Otp DB name"));

        uts.dbCof.GetProjectSingleSetting(_T("nWaitKeyType"), uts.info.nWaitKeyType, 0, _T("0: Space / 1: Enter"));
        uts.dbCof.GetProjectSingleSetting(_T("nDeviceIndexInFileName"), uts.info.nDeviceIndexInFileName, 0, _T("0: No 1:Yes"));
        uts.dbCof.GetProjectSingleSetting(_T("nMsgBlinkEn"), uts.info.nMsgBlinkEn, 1, _T("0: Disable 1:Enable"));
        
		//MES-------------------------------------------------------------------------
		uts.dbCof.GetProjectSingleSetting(_T("nMesEn"), uts.info.nMesEn, 0, _T("0: Disable, 1: Enable"));
		uts.dbCof.GetProjectSingleSetting(_T("nMesOffice"), uts.info.nMesOffice, 1, _T("1: Test at Office,0:Test at Production Line"));

        //-------------------------------------------------------------------------
        // set position
        CString strMainWndPos, strInfoBoardWndPos;
        vector<int> vecMainWndPos, vecInfoBoardWndPos;
        uts.dbCof.GetProjectSingleSetting(_T("strMainWndPos"), strMainWndPos, _T("0,0,440,440"), _T("Left,Top,Width,Height"));
        uts.dbCof.GetProjectSingleSetting(_T("strInfoBoardWndPos"), strInfoBoardWndPos, _T("440,0,200,440"), _T("Left,Top,Width,Height"));
        SplitInt(strMainWndPos, vecMainWndPos);
        SplitInt(strInfoBoardWndPos, vecInfoBoardWndPos);
        SetWindowPos(uts.info.hMainWnd, HWND_TOP, vecMainWndPos[0], vecMainWndPos[1], vecMainWndPos[2], vecMainWndPos[3], SWP_SHOWWINDOW);
        SetWindowPos(uts.info.hInfoboardWnd, HWND_TOP, vecInfoBoardWndPos[0], vecInfoBoardWndPos[1], vecInfoBoardWndPos[2], vecInfoBoardWndPos[3], SWP_SHOWWINDOW);
        SendMessage(uts.info.hMainWnd, WM_SIZING, 0, 0);
        SendMessage(uts.info.hInfoboardWnd, WM_SIZING, 0, 0);
        SendMessage(uts.info.hInfoboardWnd, WM_BOARD_ADJUST, 0, 0);
#endif

#pragma region DumpInfo
#ifdef DEBUG
        uts.log.Debug(_T("Debug Log."));
        uts.log.Info(_T("Info Log."));
        uts.log.Warning(_T("Warning Log."));
        uts.log.Error(_T("Error Log."));
#endif
        uts.log.Debug(_T("[%s]App Start."), UTS_FN);
        uts.log.Debug(GetTimesString(_T("-"), 80));
        uts.log.Debug(_T("utsinfo:"));
        uts.log.Debug(_T("\t RootPath                = %s"), uts.info.strRootPath);
        uts.log.Debug(_T("\t BinPath                 = %s"), uts.info.strBinPath);
        uts.log.Debug(_T("\t ConfigPath              = %s"), uts.info.strConfigPath);
        uts.log.Debug(_T("\t ResultPath              = %s"), uts.info.strResultPath);
        uts.log.Debug(_T("\t ResultPath              = %s"), uts.info.strShopFlowFilePath);

        uts.log.Debug(_T("\t nShopFlowEn             = %d"), uts.info.nShopFlowEn);
		uts.log.Debug(_T("\t nMesEn                  = %d"), uts.info.nMesEn);
		uts.log.Debug(_T("\t nMesOffice              = %d"), uts.info.nMesOffice);
        uts.log.Debug(_T("\t strProjectType          = %s"), uts.info.strProjectType);
        uts.log.Debug(_T("\t ProjectName             = %s"), uts.info.strProjectName);
        uts.log.Debug(_T("\t LineName                = %s"), uts.info.strLineName);
        uts.log.Debug(_T("\t strStationCode          = %s"), uts.info.strStationCode);
        uts.log.Debug(_T("\t StationName             = %s"), uts.info.strStationName);
        uts.log.Debug(_T("\t strPositionCode         = %s"), uts.info.strPositionCode);
        uts.log.Debug(_T("\t strPositionName         = %s"), uts.info.strPositionName);

        uts.log.Debug(_T("\t nDeviceId               = %d"), uts.info.nDeviceId);
        uts.log.Debug(_T("\t nSensorType             = %d"), uts.info.nSensorType);
		uts.log.Debug(_T("\t nOISType				 = %d"), uts.info.nOISType);
		uts.log.Debug(_T("\t nVCMType				 = %d"), uts.info.nVCMType);
        uts.log.Debug(_T("\t nLTDD_DummyFrame        = %d"), uts.info.nLTDD_DummyFrame);
        uts.log.Debug(_T("\t nLTDD_AvgFrame          = %d"), uts.info.nLTDD_AvgFrame);
        uts.log.Debug(_T("\t nSaveBmpFile            = %d"), uts.info.nSaveBmpFile);
        uts.log.Debug(_T("\t nSaveRawFile            = %d"), uts.info.nSaveRawFile);
        uts.log.Debug(_T("\t nSaveYuvBmpFile         = %d"), uts.info.nSaveYuvBmpFile);
        uts.log.Debug(_T("\t nZipFile                = %d"), uts.info.nZipFile);
        uts.log.Debug(_T("\t nStopOnFail             = %d"), uts.info.nStopOnFail);
        uts.log.Debug(_T("\t strVersion              = %s"), uts.info.strVersion);
        uts.log.Debug(_T("\t nOtpDBType              = %d"), uts.info.nOtpDBType);
        uts.log.Debug(_T("\t strOtpDBHost            = %s"), uts.info.strOtpDBHost);
        uts.log.Debug(_T("\t strOtpDBUser            = %s"), uts.info.strOtpDBUser);
        uts.log.Debug(_T("\t strOtpDBPassword        = %s"), uts.info.strOtpDBPassword);
		uts.log.Debug(_T("\t strOtpDBName            = %s"), uts.info.strOtpDBName);
		uts.log.Debug(_T("\t nWaitKeyType            = %d"), uts.info.nWaitKeyType);
        uts.log.Debug(_T("\t nDeviceIndexInFileName  = %d"), uts.info.nDeviceIndexInFileName);
        uts.log.Debug(_T("\t nMsgBlinkEn             = %d"), uts.info.nMsgBlinkEn);
#ifdef CONFIG_FROM_FILE
        uts.log.Debug(_T("\t OperatorCount  = %d"), uts.info.nOperatorCount);
        int nOpIndex = 0;
        POSITION pos = uts.info.lstOperator.GetHeadPosition();
        while (nullptr != pos)
        {
            CString strOpName = uts.info.lstOperator.GetNext(pos);
            uts.log.Debug(_T("\t strOpName[%d]  = %s"), nOpIndex++, strOpName);
        }
#endif
        uts.log.Debug(GetTimesString(_T("-"), 80));
#pragma endregion

        //------------------------------------------------------------------------------
        // 初始化remote otp DB
        if (uts.info.nOtpDBType >= 0)
        {
            uts.otpdb = DDM::OtpDB::get_instance();
            DDM::OtpDBPara otpDBPara = {0};
            otpDBPara.dbType = uts.info.nOtpDBType;
            USES_CONVERSION;
            strcpy_s(otpDBPara.dbname, 0x20, T2A(uts.info.strOtpDBName));
            strcpy_s(otpDBPara.host, 0x20, T2A(uts.info.strOtpDBHost));
            strcpy_s(otpDBPara.passwd, 0x20, T2A(uts.info.strOtpDBPassword));
            strcpy_s(otpDBPara.user, 0x20, T2A(uts.info.strOtpDBUser));

            if (uts.otpdb->Connect(otpDBPara) < 0)
            {
                uts.log.Error(_T("[%s]Remote otp DB Coonect Fail."), UTS_FN);
                AfxMessageBox(_T("Remote otp DB Coonect Fail."), MB_ICONERROR);
                return FALSE;
            }
            else
            {
                uts.log.Info(_T("[%s]Remote otp DB Initialize Success."), UTS_FN);
            }
        }

        //------------------------------------------------------------------------------
        // 初始化测试流程
        bRet = uts.flow.Initialize(hDisplayWnd);
        if (!bRet)
        {
            AfxMessageBox(_T("Test flow Initialize Fail."), MB_ICONERROR);
            return FALSE;
        }
        else
        {
            uts.log.Info(_T("[%s]Test flow Initialize Success."), UTS_FN);
        }

        uts.board.Initialize(hInfoBoardWnd);
        uts.board.ShowStatus(eBoardStatus::Status_Initializing);
        uts.info.bAppRunning = TRUE;    // fix bug #1: 批处理图片后不能退出

        return TRUE;
    }

    void UTSGD::DestroyUTS()
    {
        uts.info.bAppRunning = FALSE;   // fix bug #1: 批处理图片后不能退出
        uts.flow.Stop();
        uts.flow.WaitTestFinish();
        uts.flow.Destroy();
        uts.log.Destroy();

        if (uts.info.nOtpDBType >= 0)
        {
            if (uts.otpdb != nullptr)
            {
                uts.otpdb->Close();
            }
        }
    }

    void UTSGD::MessageRestart(LPCTSTR lpMessage)
    {
        int nRet = AfxMessageBox(lpMessage, MB_YESNO | MB_ICONQUESTION);
        if (nRet == IDNO)
        {
            return;
        }
        else
        {
            PostQuitMessage(0);
            CString strLauncher = uts.info.strBinPath + _T("UTSLauncher.exe");
            OSUtil::ExcuteCommand(strLauncher, FALSE);
        }
    }

}
