#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

#include "ClientSocket.h"

#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _operator_param_
    {
        CString strRegStrForCheck;
        CString strHintMessage;
		CString IP;
		CString Port;
    } OPERATOR_PARAM;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

		CClientSocket m_ClientSocket;

		CString sIniFileName;
		int ColorTestEn_3100;
		int ColorTestEn_4000;
		int ColorTestEn_5100;

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);

		void OnSendbutton(CString strMsg); 

    private:
        OPERATOR_PARAM m_param;
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}
