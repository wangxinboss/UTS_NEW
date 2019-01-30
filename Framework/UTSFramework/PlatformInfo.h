#pragma once
#include <vector>
#include <map>
#include "Export.h"

namespace UTS
{
    typedef struct _return_value_item_
    {
        CString strDllFilename;
        std::vector<int> vecReturnValue;
    } RETURN_VALUE_ITEM;
    
    class UTS_FRAMEWORK_API PlatformInfo
    {
    public:
        PlatformInfo();
        ~PlatformInfo();

        // sfc
        int nShopFlowEn;
		int nMesEn; //EdwardChen add for MES 2018/04/15
		int nMesOffice; //EdwardChen add for MES 2018/04/15
		BOOL nMesCommit;//杨兵 add for MES 2018/04/27
        CString strProjectType;
        CString strLineName;
        CString strStationCode;
        CString strPositionCode;
        CString strPositionName;
        CString strShopFlowFilePath;

        // this 2 value will be used for SFC too
        CString strProjectName;
        CString strStationName;
    	CString strMESInsData; //EdwardChen add for MES 2018/04/15
    	BOOL	bMesResult; //EdwardChen add for MES 2018/04/15
		int		nErrorCode; //EdwardChen add for MES 2018/04/15
        
        //CString strDeviceName;
        int nDeviceId;
        int nSensorType;
		int nOISType;
		int nVCMType;
		int nGetFrameWaitTime;

        int nLTDD_DummyFrame;
        int nLTDD_AvgFrame;
        int nSaveBmpFile;
        int nSaveRawFile;
        int nSaveYuvBmpFile;
        int nZipFile;
        int nStopOnFail;
        CString strVersion;

		int nWaitKeyType;   // 0: Space / 1: Enter
        int nDeviceIndexInFileName;	// output device index in saved filename
        int nMsgBlinkEn;

#ifdef CONFIG_FROM_FILE
        int nOperatorCount;
        CStringList lstOperator;
        int nInitOperatorCount;
        CStringList lstInitOperator;
#endif

        //------------------------------------------------------------------------------
        // path
        CString strRootPath;
        CString strConfigPath;
        CString strBinPath;
        CString strResultPath;
        //CString strResultDataPath;
        //CString strResultImagePath;

        //-------------------------------------------------------------------------
        // otp DB
        int nOtpDBType;
        CString strOtpDBHost;
        CString strOtpDBUser;
        CString strOtpDBPassword;
        CString strOtpDBName;

        //------------------------------------------------------------------------------
        // runtime info
        BOOL bHasTestError;
        BOOL bAppRunning;   // fix bug #1: 批处理图片后不能退出
        int nUserType;
        CString strUserId;
        CString strSN;
		CString strSN_Single;
        CString strSensorId;
        BOOL bShowFPS;
        HWND hMainWnd;
        HWND hpDisplayWnd;
        HWND hInfoboardWnd;
        //int nArrDynamicCurrentOffset[6];
        //int nArrStandbyCurrentOffset[6];
        std::vector<RETURN_VALUE_ITEM> vecReturnValueItem;
        std::map<std::string, double> mapShared;
        int nDeviceIndex;
    };
}
