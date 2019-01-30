#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#ifdef OSTOPERATOR_EXPORTS
#define OSTOPERATOR_API __declspec(dllexport)
#else
#define OSTOPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    class OSTOperator : public BaseOperator
    {
    public:
        OSTOperator(void);
        ~OSTOperator(void);

		virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);

	protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

	private:
		OST_CONFIG m_OSTConfig;
		OST_RESULT m_OSTResult;
    };

    extern "C"
    {
        OSTOPERATOR_API BaseOperator* GetOperator(void);
    }
}
