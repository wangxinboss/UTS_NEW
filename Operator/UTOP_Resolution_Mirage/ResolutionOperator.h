#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"
#include "CommonFunc.h"
#include "StackQueue.h"

using namespace UTS::Algorithm::Image;
using namespace UTS::Algorithm::SFR::SfrCalc;
using namespace UTS::Algorithm::SFR::SfrPlus;
using namespace UTS::Algorithm::SFRBlock;

#ifdef RESOLUTION_OPERATOR_EXPORTS
#define RESOLUTION_OPERATOR_API __declspec(dllexport)
#else
#define RESOLUTION_OPERATOR_API __declspec(dllimport)
#endif

namespace UTS
{
    //-------------------------------------------------------------------------
    typedef struct _whitepatch_info_
    {
        SIZE sizeDisFromCT;
        int nROISize;
    } WHITEPATCH_INFO;

    typedef struct _single_sfr_roi_info_
    {
		int nSfrType;       // 0:Real, 1:AllAverage, 2: MidAverage
		int nSfrStackCount;

        int nIndex;             // 索引号
        double dROIField;       // ROI中心点所在视场
        double dROIAngle;       // ROI中心点的极坐标角度(0-359.9999)
        CString strROIName;     // ROI名
    } SINGLE_SFR_ROI_INFO;

    typedef struct _single_field_info_
    {
        //int nIndex;             // 索引号
        //int nEnable;            // 启用
        double dHiSpec;         // HiFrq规格
        double dHiDeltaSpec;    // HiFrq极差规格
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
        WHITEPATCH_INFO whitePatch;
        SFR_INFO sfr;
		SIZE DetectSFRROI;
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
        //double dYvalue;
        double dCenterYavg;
        BOOL bIsCenterYExp;
        RECT rcWhitePatch;
    } SFR_PLUS_OPERATOR_RESULT;

    class ResolutionOperator : public BaseOperator
    {
    public:
        ResolutionOperator(void);
        ~ResolutionOperator(void);

        virtual BOOL OnReadSpec();
        virtual BOOL OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark);
		virtual void OnPreviewStart();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
        virtual void OnGetRegisterList(vector<CString> &vecRegister);
    protected:
        virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

    private:
        CSfrPlus m_SfrPlus;
        CSfrStackQueue m_SfrQueue;
		BOOL m_bEnableStack;
        //------------------------------------------------------------------------------
        // 参数
        SFR_PLUS_OPERATOR_PARAM m_param;
        //------------------------------------------------------------------------------
        // 结果
        SFR_PLUS_OPERATOR_RESULT m_result;
        //------------------------------------------------------------------------------

        void SaveThroughFocus(LPCTSTR lpOperatorName);
        BOOL DoImageSFRTest(int *pnErrorCode);
    };

    extern "C"
    {
        RESOLUTION_OPERATOR_API BaseOperator* GetOperator(void);
    }
}
