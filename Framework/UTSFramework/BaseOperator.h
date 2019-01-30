#pragma once
#include "Export.h"
#include "BaseDevice.h"
//!! --- 2018.07.31 Chi-Jen.Liao
struct TestItemResult
{
	CString sItemName;
	CString sHeader;
	CString sData;
	CString sSFC_Filter;
};
//---------------------------------
namespace UTS
{
    //-------------------------------------------------------------------------
    // Macro: OPERATOR_INIT
    //      1. init m_strModuleFile
    //      2. init m_strOperatorName
    #define OPERATOR_INIT \
        TCHAR szFileName[MAX_PATH] = {0};\
        GetModuleFileName(theApp.m_hInstance, szFileName, MAX_PATH);\
        m_strModuleFile = szFileName;\
        TCHAR szOperatorName[MAX_PATH] = {0};\
        _stscanf(OSUtil::GetMainFileName(OSUtil::GetFileName(m_strModuleFile)), _T("UTOP_%s"), szOperatorName);\
        m_strOperatorName = szOperatorName;\


    //-------------------------------------------------------------------------
    // BaseOperator
    class UTS_FRAMEWORK_API BaseOperator
    {
    public:
        BaseOperator(void);
        virtual ~BaseOperator(void);

        virtual BOOL OnReadSpec();
        virtual void OnPreviewStart();
        virtual BOOL OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark);
        virtual BOOL OnPreview(const TEST_BUFFER &bufferObj, int nWidth, int nHeight, UI_MARK &uiMark);
        virtual void OnPreviewStop();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode) = 0;
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);
        void SetDevice(BaseDevice *pDevice);
        BaseDevice* GetDevice(void);
        TEST_BUFFER& GetTestBuffer(void);
        BUFFER_INFO& GetBufferInfo(void);
        BOOL IsMustStopOnFail(void);   // fix bug #11: SN、LightOn失败不受StopOnFail限制。

    protected:
        // 保存图像(if lpName is nullptr, use operator name instead)
        virtual void SaveImage(LPCTSTR lpName = nullptr);
        // 保存测试数据
        virtual void SaveData(void);
        // 取得数据内容
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

        BOOL GetSensorOutMode(int &nOutMode);
        CString GetResultCSVFilename(LPCTSTR lpName, const SYSTEMTIME &st);

        BaseDevice *m_pDevice;
        TEST_BUFFER m_bufferObj;
        BUFFER_INFO m_bufferInfo;
        UTS::HighPrecisionTimer m_TimeCounter;
        BOOL m_bResult;
        CString m_strModuleFile;    // Operator dll名
        CString m_strOperatorName;
        BOOL m_bMustStopOnFail;     // fix bug #11: SN、LightOn失败不受StopOnFail限制。
    };
}
