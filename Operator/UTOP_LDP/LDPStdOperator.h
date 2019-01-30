#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::DefectPixel::DefectPixelStd;

#ifdef LDP_STD_OPERATOR_EXPORTS
#define LDP_STD_OPERATOR_API __declspec(dllexport)
#else
#define LDP_STD_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _ldp_operator_param_
    {
        double dLTMinY;             // 白板测试最小Y值
        double dLTMaxY;             // 白板测试最大Y值
        int nReCapture;             // 0：不动作  1：进行更换序列、抓图片、保存的动作   
        double dThreshold;
        SIZE sizeROI;
        int nSingleSpec;
        int nDoubleSpec;
        int nMultipleSpec;
        int nDefectPixel3ChannelEn;
    } LDP_OPERATOR_PARAM;

    class LDPStdOperator : public BaseOperator
    {
    public:
        LDPStdOperator(void);
        ~LDPStdOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        //------------------------------------------------------------------------------
        // 内部函数
        void SaveLocationData(LPCTSTR lpOperatorName);
		void DrawLocationBuffer(unsigned char *pBmpBuffer);
		void SaveLocationBufer(unsigned char *pBMPBuffer);
        //------------------------------------------------------------------------------
        // 参数
        LDP_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        H4ISPDefectInfo m_defectInfo;
        AA_TwoDPoint m_DefPointTable[DEFECT_COUNT_MAX];
        double m_dYvalue;
    };

    extern "C"
    {
        LDP_STD_OPERATOR_API BaseOperator* GetOperator(void);
    }
}

