#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"


#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef struct _operator_param_
    {
        double dSoundVolume;
        int nSoundFreq;
        int nWaitTime;
        int nBypassTime;
        double dPeakValueSpec;
        double dPeakPositionSpec;
        double dSNRSpec;
    } OPERATOR_PARAM;

    typedef struct _operator_result_
    {
        double dMaxPeakValue;       // MaxPeak Value
        double dMaxPeakPosition;    // MaxPeak Position
        double dSNR;                // Total Harmonic Distortion
    } OPERATOR_RESULT;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        BOOL SaveRecordFile(LPCTSTR lpFileName, int nBufferSize, const double *pdSoundChannelBuffer, const double *pFFTResult);
        void CreateBufferByFreq(int iFrequence, int iSamplesPerSec, int iBufLen, short* psData);
		double VerifyAmplitude(short* pData,int iBufLen,double dFrequence,double &MaxStdev,double &MinStdev,double &AmpStdev);
		double VerifyAmplitude(double* pData,int iBufLen,double dFrequence,double &MaxStdev,double &MinStdev,double &AmpStdev);

    private:
        //------------------------------------------------------------------------------
        // 参数
        OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        OPERATOR_RESULT m_result;
        //------------------------------------------------------------------------------
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}

