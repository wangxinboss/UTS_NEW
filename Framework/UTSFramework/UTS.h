#pragma once
#include "Export.h"
#include "ListLog.h"
#include "PlatformInfo.h"
#include "TestFlow.h"
#include "UIBoardProtocol.h"
#include "ErrorCode.h"
#include "Keyboard.h"
#include "DBLocalConfig.h"
#include "ImageFile.h"

namespace DDM { class OtpDB; }

namespace UTS
{
    class UTS_FRAMEWORK_API UTSGD
    {
    public:
        static UTSGD& GetInstance();
        BOOL InitializeUTS(
            HWND hMainWnd,
            HWND hDisplayWnd,
            HWND hInfoBoardWnd);
        void DestroyUTS();
        void MessageRestart(LPCTSTR lpMessage);


        ListLog log;
        PlatformInfo info;
        TestFlow flow;
        UIBoardProtocol board;
        ErrorCode errorcode;
        Keyboard keyboard;
        DBLocalConfig dbCof;
        DDM::OtpDB *otpdb;
        ImageFile imgFile;
        std::map<std::string, void*> mapUserMemory;

		//Com
		ComSeries COM_RS232;

    private:
        UTSGD();
        UTSGD(const UTSGD&);
        UTSGD& operator = (const UTSGD&);
        ~UTSGD();
    };

    UTS_FRAMEWORK_API extern UTSGD &uts;
}
