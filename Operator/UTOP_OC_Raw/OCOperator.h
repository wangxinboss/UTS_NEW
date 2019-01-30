#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm::OC::OC_Std;

#ifdef OC_STD_OPERATOR_EXPORTS
#define OC_STD_OPERATOR_API __declspec(dllexport)
#else
#define OC_STD_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _oc_operator_param_
    {
        double dLTMinY;         // 白板测试最小Y值
        double dLTMaxY;         // 白板测试最大Y值
        int nReCapture;         // 0：不动作  1：进行更换序列、抓图片、保存的动作
        int nPixelOffsetSpec;
		int nCommit_En;
    } OC_OPERATOR_PARAM;

    class OCOperator : public BaseOperator
    {
    public:
        OCOperator(void);
        ~OCOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);
		
		BOOL  is_otp_data_locked(int module_id);
		BOOL is_otp_programed(int module_id);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        //------------------------------------------------------------------------------
        // 参数
        OC_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        double m_dYvalue;
        double m_Offset[4];
        POINT m_ptOcCenter[4];
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        OC_STD_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
