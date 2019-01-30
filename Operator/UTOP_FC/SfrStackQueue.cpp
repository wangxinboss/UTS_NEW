#include "StdAfx.h"
#include "SfrStackQueue.h"

CSfrStackQueue::CSfrStackQueue(void)
{
    m_pdSfrQueue = nullptr;
    m_nQueueIndex = 0;
}

CSfrStackQueue::~CSfrStackQueue(void)
{
    RELEASE_ARRAY(m_pdSfrQueue);
}

void CSfrStackQueue::Initialize(int nSfrRoiCount, int nSfrStackCount, int nSfrType)
{
    m_nSfrRoiCount = nSfrRoiCount;
    m_nSfrStackCount = nSfrStackCount;
    m_nSfrType = nSfrType;

    RELEASE_ARRAY(m_pdSfrQueue);
    m_pdSfrQueue = new SFRPLUS_RESULT[nSfrStackCount];
    for (int i = 0; i < nSfrStackCount; i++)
    {
        for (int j = 0; j < nSfrRoiCount; j++)
        {
            m_pdSfrQueue[i].dFrq[j] = 0.0;
            m_pdSfrQueue[i].dNq[j] = 0.0;
            m_pdSfrQueue[i].dSFR[j][SFR_Frq_Hi] = 0.0;
            m_pdSfrQueue[i].dSFR[j][SFR_Frq_Lo] = 0.0;
        }
    }
    m_nQueueIndex = 0;
}

void CSfrStackQueue::AddSfrResult(const SFRPLUS_RESULT &stSfrResult)
{
    for (int j = 0; j < m_nSfrRoiCount; j++)
    {
        m_pdSfrQueue[m_nQueueIndex].dFrq[j] = stSfrResult.dFrq[j];
        m_pdSfrQueue[m_nQueueIndex].dNq[j] = stSfrResult.dNq[j];
        m_pdSfrQueue[m_nQueueIndex].dSFR[j][SFR_Frq_Hi] = stSfrResult.dSFR[j][SFR_Frq_Hi];
        m_pdSfrQueue[m_nQueueIndex].dSFR[j][SFR_Frq_Lo] = stSfrResult.dSFR[j][SFR_Frq_Lo];
    }
    m_nQueueIndex++;
    if (m_nQueueIndex >= m_nSfrStackCount)
    {
        m_nQueueIndex = 0;
    }
    memcpy(&m_stLastResult, &stSfrResult, sizeof(SFRPLUS_RESULT));
}

void CSfrStackQueue::GetSfrResult(SFRPLUS_RESULT &stSfrResult)
{
    SFRPLUS_RESULT minResult;
    SFRPLUS_RESULT maxResult;
    for (int j = 0; j < m_nSfrRoiCount; j++)
    {
        stSfrResult.dFrq[j] = 0.0;
        stSfrResult.dNq[j] = 0.0;
        stSfrResult.dSFR[j][SFR_Frq_Hi] = 0.0;
        stSfrResult.dSFR[j][SFR_Frq_Lo] = 0.0;

        minResult.dFrq[j] = 99999.0;
        minResult.dNq[j] = 99999.0;
        minResult.dSFR[j][SFR_Frq_Hi] = 99999.0;
        minResult.dSFR[j][SFR_Frq_Lo] = 99999.0;

        maxResult.dFrq[j] = 0.0;
        maxResult.dNq[j] = 0.0;
        maxResult.dSFR[j][SFR_Frq_Hi] = 0.0;
        maxResult.dSFR[j][SFR_Frq_Lo] = 0.0;
    }
    memcpy(&stSfrResult.roi, &m_stLastResult.roi, sizeof(RECT));

    if (m_nSfrType == eSfrType::SFR_TYPE_REAL)
    {
        memcpy(&stSfrResult, &m_stLastResult, sizeof(SFRPLUS_RESULT));
    }
    else
    {
        for (int i = 0; i < m_nSfrStackCount; i++)
        {
            for (int j = 0; j < m_nSfrRoiCount; j++)
            {
                stSfrResult.dFrq[j] += m_pdSfrQueue[i].dFrq[j];
                stSfrResult.dNq[j] += m_pdSfrQueue[i].dNq[j];
                stSfrResult.dSFR[j][SFR_Frq_Hi] += m_pdSfrQueue[i].dSFR[j][SFR_Frq_Hi];
                stSfrResult.dSFR[j][SFR_Frq_Lo] += m_pdSfrQueue[i].dSFR[j][SFR_Frq_Lo];

                minResult.dFrq[j] = min(minResult.dFrq[j], m_pdSfrQueue[i].dFrq[j]);
                minResult.dNq[j] = min(minResult.dNq[j], m_pdSfrQueue[i].dNq[j]);
                minResult.dSFR[j][SFR_Frq_Hi] = min(minResult.dSFR[j][SFR_Frq_Hi], m_pdSfrQueue[i].dSFR[j][SFR_Frq_Hi]);
                minResult.dSFR[j][SFR_Frq_Lo] = min(minResult.dSFR[j][SFR_Frq_Lo], m_pdSfrQueue[i].dSFR[j][SFR_Frq_Lo]);

                maxResult.dFrq[j] = max(maxResult.dFrq[j], m_pdSfrQueue[i].dFrq[j]);
                maxResult.dNq[j] = max(maxResult.dNq[j], m_pdSfrQueue[i].dNq[j]);
                maxResult.dSFR[j][SFR_Frq_Hi] = max(maxResult.dSFR[j][SFR_Frq_Hi], m_pdSfrQueue[i].dSFR[j][SFR_Frq_Hi]);
                maxResult.dSFR[j][SFR_Frq_Lo] = max(maxResult.dSFR[j][SFR_Frq_Lo], m_pdSfrQueue[i].dSFR[j][SFR_Frq_Lo]);
            }
        }
        if (m_nSfrType == eSfrType::SFR_TYPE_TOTALAVERAGE)
        {
            for (int j = 0; j < m_nSfrRoiCount; j++)
            {
                stSfrResult.dFrq[j] /= m_nSfrStackCount;
                stSfrResult.dNq[j] /= m_nSfrStackCount;
                stSfrResult.dSFR[j][SFR_Frq_Hi] /= m_nSfrStackCount;
                stSfrResult.dSFR[j][SFR_Frq_Lo] /= m_nSfrStackCount;
            }
        }
        else    // eSfrType::SFR_TYPE_MIDAVERAGE
        {
            for (int j = 0; j < m_nSfrRoiCount; j++)
            {
                stSfrResult.dFrq[j] -= (minResult.dFrq[j] + maxResult.dFrq[j]);
                stSfrResult.dNq[j] -= (minResult.dNq[j] + maxResult.dNq[j]);
                stSfrResult.dSFR[j][SFR_Frq_Hi] -= (minResult.dSFR[j][SFR_Frq_Hi] + maxResult.dSFR[j][SFR_Frq_Hi]);
                stSfrResult.dSFR[j][SFR_Frq_Lo] -= (minResult.dSFR[j][SFR_Frq_Lo] + maxResult.dSFR[j][SFR_Frq_Lo]);

                stSfrResult.dFrq[j] /= (m_nSfrStackCount - 2);
                stSfrResult.dNq[j] /= (m_nSfrStackCount - 2);
                stSfrResult.dSFR[j][SFR_Frq_Hi] /= (m_nSfrStackCount - 2);
                stSfrResult.dSFR[j][SFR_Frq_Lo] /= (m_nSfrStackCount - 2);
            }
        }
    }
}
