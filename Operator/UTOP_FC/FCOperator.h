#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"
#include "SfrStackQueue.h"

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::CrossHair;
using namespace UTS::Algorithm::SFR::SfrCalc;
using namespace UTS::Algorithm::SFR::SfrPlus;

#ifdef FC_OPERATOR_EXPORTS
#define FC_OPERATOR_API __declspec(dllexport)
#else
#define FC_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    #define Max_VCM_DAC		1024

    //-------------------------------------------------------------------------
    // 参数
    typedef struct _vcm_param_
    {
        int nVCM_ThroughDriveEn;
        int nVCM_ThroughROIIndex;
        int nVCM_TypeSelect;
        int nVCM_StepTime;
        int nVCM_FinalDelayTime;
        int nVCM_GetDelayTime;
        int nVCM_DACMin;
        int nVCM_DACMax;
        int nVCM_DACStep;
        int nVCM_MeasureCurrentEn;
        int nVCM_CurrentPIN;
        int nVCM_NotMovingThreshold;
        int nGlitch_DetectEn;
        int nGlitch_SmoothEn;
        int nGlitch_Threshold;
    } VCM_PARAM;

    typedef struct _crosshair_info_
    {
        SIZE sizeDisFromCT;
        int nROISize;
        double dCodeVariation;
        double dField; // 定位十字交点所在视场
    } CROSSHAIR_INFO;

    typedef struct _whitepatch_info_
    {
        SIZE sizeDisFromCT;
        int nROISize;
    } WHITEPATCH_INFO;

    typedef struct _weight_info_
    {
        int nHiEnable;
        double dHiSpec;
        double dHiDiffSpec;
    } WEIGHT_INFO;

    typedef struct _divide_spec_
    {
        int nEnable;
        int nFieldIndex; 
        int nSpecA;
        int nSpecB;
        int nSpecC;
    } DIVIDE_SPEC;

    typedef struct _single_sfr_roi_info_
    {
        int nIndex;             // 索引号
        double dROIField;       // ROI中心点所在视场
        double dROIAngle;       // ROI中心点的极坐标角度(0-359.9999)
        CString strROIName;     // ROI名
    } SINGLE_SFR_ROI_INFO;

    typedef struct _single_field_info_
    {
        int nIndex;             // 索引号
        int nEnable;            // 启用
        double dHiSpec;         // HiFrq规格
        double dHiDeltaSpec;    // HiFrq极差规格
        double dWeight;         // 权重
        double dDeltaWeight;    // 极差权重
        vector<int> vecSfrIndex;// sfr列表
    } SINGLE_FIELD_INFO;

    typedef struct _sfr_info_
    {
        int nSfrType;       // 0:Real, 1:AllAverage, 2: MidAverage
        int nSfrStackCount;
        SIZE sizeSFR_ROI;
        double dFrqNum[SFR_Frq_SIZES];
        double dSFRScore;

        int nSfrRoiCount;
        vector<SINGLE_SFR_ROI_INFO> vecSfrRoi; // sfr roi 列表
        int nFieldCount;                    // 启用视场个数
        vector<SINGLE_FIELD_INFO> vecField; // 视场列表
    } SFR_INFO;

    typedef struct _sfr_plus_operator_param_
    {
        int nFixRoiEn;
        int nManualThroughFocusEn;
        int nManualThroughFocusFrameCount;
        CROSSHAIR_INFO crosshair;
        WHITEPATCH_INFO whitePatch;
        WEIGHT_INFO weight;
        DIVIDE_SPEC divideSpec;
        SFR_INFO sfr;
        VCM_PARAM vcm;
    } SFR_PLUS_OPERATOR_PARAM;

    //-------------------------------------------------------------------------
    // 结果
    typedef struct _single_field_result_
    {
        double dMinSfr[SFR_Frq_SIZES];
        double dAvgSfr[SFR_Frq_SIZES];
        double dSfrDelta[SFR_Frq_SIZES];
    } SINGLE_FIELD_RESULT;

    typedef struct _sfr_plus_operator_result_
    {
        SFRPLUS_RESULT stSfrResult;
        vector<SINGLE_FIELD_RESULT> vecFieldResult;
        double dYvalue;
        double dCenterYavg;
        BOOL bIsCenterYExp;
        RECT rcWhitePatch;
        double dWeightSfr[SFR_Frq_SIZES];
        double dWeightMax[SFR_Frq_SIZES];
        double dWeightDiff[SFR_Frq_SIZES];
        double dThroughVCMCurrent[Max_VCM_DAC];
        double dSFRHiThroughValue[SFR_PLUS_MAX_ROI][Max_VCM_DAC];
        double dCTSFRHiSmoothValue[Max_VCM_DAC];
        double dWeightThroughValue[Max_VCM_DAC];
        BOOL bIsVCMMove;            // VCM是否Move
        BOOL bHaveGlitch;           // 存在Glitch与否
        int nGlitchDac;             // Glitch位置
        double dMaxGlitch;          // 最大Glitch值
        int nThroughPeakDac;        // 峰值位置
        double dThroughHiSFRMax;    // ThroughFocus最大SFR
        double dThroughHiSFRMin;    // ThroughFocus最小SFR
    } SFR_PLUS_OPERATOR_RESULT;

    class FCOperator : public BaseOperator
    {
    public:
        FCOperator(void);
        ~FCOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark);
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);

    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData);

    private:
        CString m_strModuleFile;
        CSfrPlus m_SfrPlus;
        CSfrStackQueue m_SfrQueue;
        BOOL m_bEnableStack;
        HighPrecisionTimer m_ThroughFocusTimeCount;
        //------------------------------------------------------------------------------
        // 参数
        SFR_PLUS_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        SFR_PLUS_OPERATOR_RESULT m_result;
        //------------------------------------------------------------------------------

        void SaveThroughFocus(LPCTSTR lpOperatorName);
        BOOL DoImageSFRTest(int *pnErrorCode);
        BOOL ThroughFocus(BOOL *pbIsRunning, int *pnErrorCode);
        BOOL MoveToBestSFR(int nMaxSFRDac, int *pnErrorCode);
        void SetVCM_Move(int nVCM_CurrentDAC);
        double GetDacValue(int nDac);
        double GetCurrentValue();
    };

    extern "C"
    {
        FC_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
