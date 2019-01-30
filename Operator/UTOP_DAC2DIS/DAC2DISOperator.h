#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

using namespace UTS::Algorithm;
#ifdef DAC2DIS_OPERATOR_EXPORTS
#define DAC2DIS_OPERATOR_API __declspec(dllexport)
#else
#define DAC2DIS_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    const int Max_VCM_DAC = 1024;

    typedef struct _vcm_param_
    {
        int nMinDac;
        int nMaxDac;
        int nStepDac;
        int nType;
    } VCM_PARAM;

    typedef struct _dac2dis_operator_param_
    {
        COM_SERIES_PARAM comParam;
        VCM_PARAM vcmParam;
        int nMoveAfterTime;
        int nSamplingInterval;
        int nDummyCount;
        int nAvgCount;
        int nMeasureCurrentEn;
        int nCurrentPIN;
    } DAC2DIS_OPERATOR_PARAM;

    typedef struct _dac2dis_result_param_
    {
        double arrDisForward[Max_VCM_DAC];
        double arrDisBackward[Max_VCM_DAC];
        double arrCurrentForward[Max_VCM_DAC];
        double arrCurrentBackward[Max_VCM_DAC];
    } DAC2DIS_OPERATOR_RESULT;

    class DAC2DISOperator : public BaseOperator
    {
    public:
        DAC2DISOperator(void);
        ~DAC2DISOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
    
    private:
        BOOL ReadZValue(double &dZValue);
        BOOL ParseZValue(const char *pValueBuffer, double &dZValue);
        BOOL GetAvgValue(__in int nDummyCount, __in int nAvgCount, __out double &dAvgValue);
        BOOL SaveThroughData(LPCTSTR lpOperatorName);
        BOOL MeasureCurrentValue(double &dVCMCurrent);

    private:
        ComSeries m_com;
        //------------------------------------------------------------------------------
        // 参数
        DAC2DIS_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        DAC2DIS_OPERATOR_RESULT m_result;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        DAC2DIS_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
