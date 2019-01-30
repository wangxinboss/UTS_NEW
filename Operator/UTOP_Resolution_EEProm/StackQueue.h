#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
using namespace UTS::Algorithm::SFR::SfrCalc;
using namespace UTS::Algorithm::SFR::SfrPlus;

typedef enum _enum_sfr_type
{
    SFR_TYPE_REAL,
    SFR_TYPE_TOTALAVERAGE,
    SFR_TYPE_MIDAVERAGE
} eSfrType;

class CSfrStackQueue
{
public:
    CSfrStackQueue(void);
    ~CSfrStackQueue(void);

    void Initialize(int nSfrRoiCount, int nSfrStackCount, int nSfrType);
    void AddSfrResult(const SFRPLUS_RESULT &stSfrResult);
    void GetSfrResult(SFRPLUS_RESULT &stSfrResult);

private:
    SFRPLUS_RESULT *m_pdSfrQueue;
    int m_nQueueIndex;
    int m_nSfrRoiCount;
    int m_nSfrStackCount;
    int m_nSfrType;
    SFRPLUS_RESULT m_stLastResult;
};

