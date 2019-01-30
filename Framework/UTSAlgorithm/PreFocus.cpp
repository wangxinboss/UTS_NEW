#include "StdAfx.h"
#include "Algorithm.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace PreFocus
        {
            //#define OFFLINE_MODE
            const int MAX_DIVIDE_COUNT = 1024;

            namespace PreFocus_BigStepThrough
            {
                int PreThroughFocus_FindPeakRange(
                    __in const PRE_THROUGHFOCUS_PARAM &param,
                    __out PRE_THROUGHFOCUS_RESULT &result)
                {
                    result.nFixedMinDac = param.nMinDac;
                    result.nFixedMaxDac = param.nMaxDac;
                    result.dMinSfr = 100.0;
                    result.dMaxSfr = 0.0;

                    int nArrDac[MAX_DIVIDE_COUNT + 1] = {0};
                    if (param.nMaxDac - param.nMinDac < param.nDivideCount)
                    {
                        return PTE_INPUT_RANGE_TOO_NARROW;
                    }
                    if (param.lpfnGetSfrValue == nullptr || param.lpfnSetVcmMove == nullptr)
                    {
                        return PTE_FUNCTION_POINT_NULL;
                    }

                    double dStepDac = (double)(param.nMaxDac - param.nMinDac) / (double)param.nDivideCount;

                    nArrDac[0] = param.nMinDac;
                    nArrDac[param.nDivideCount] = param.nMaxDac;
                    for (int i = 1; i < param.nDivideCount; i++)
                    {
                        nArrDac[i] = DOUBLE2INT(nArrDac[0] + i * dStepDac);
                    }

                    int nPeakIndex = 0;
                    double dArrSfr[MAX_DIVIDE_COUNT + 1];
                    for (int i = 0; i <= param.nDivideCount; i++)
                    {
                        param.lpfnSetVcmMove(nArrDac[i]);
                        //Sleep(param.nStepSleepTime);
						UTS::Algorithm::Sleep(param.nStepSleepTime);

                        dArrSfr[i] = param.lpfnGetSfrValue(nArrDac[i]);
                        if (dArrSfr[i] > result.dMaxSfr)
                        {
                            result.dMaxSfr = dArrSfr[i];
                            nPeakIndex = i;
                        }
                        if (dArrSfr[i] < result.dMinSfr)
                        {
                            result.dMinSfr = dArrSfr[i];
                        }
                    }

                    result.nFixedMinDac = (nPeakIndex == 0 ? nArrDac[nPeakIndex] : nArrDac[nPeakIndex - 1]);
                    result.nFixedMaxDac = nArrDac[nPeakIndex + 1];
                    result.nPeakDac = nArrDac[nPeakIndex];

                    result.nFixedMinDac = result.nPeakDac - (((result.nPeakDac - result.nFixedMinDac) / param.nStepDac) + 1) * param.nStepDac;
                    result.nFixedMaxDac = result.nPeakDac + (((result.nFixedMaxDac - result.nPeakDac) / param.nStepDac) + 1) * param.nStepDac;
                    if (result.nFixedMinDac < param.nMinDac)
                    {
                        result.nFixedMinDac = param.nMinDac;
                    }
                    if (result.nFixedMaxDac > param.nMaxDac || result.nFixedMaxDac < result.nFixedMinDac)
                    {
                        result.nFixedMaxDac = result.nFixedMaxDac - param.nStepDac;
                    }
                    return PTE_OK;
                }

#ifdef OFFLINE_MODE
                void Offline_SetVcmMove(__in int nCurrentDac)
                {
                }

                double Offline_GetSfrValue(__in int nCurrentDac)
                {
                    double arrSfr[MAX_DIVIDE_COUNT];
                    memset(arrSfr, 0, sizeof(arrSfr));
                    arrSfr[100] = 0.0;
                    arrSfr[139] = 10.0;
                    arrSfr[178] = 20.0;
                    arrSfr[217] = 30.0;
                    arrSfr[256] = 40.0;
                    arrSfr[294] = 50.0;
                    arrSfr[333] = 40.0;
                    arrSfr[372] = 30.0;
                    arrSfr[411] = 20.0;
                    arrSfr[450] = 1.0;
                    double dSfr = arrSfr[nCurrentDac];
                    return dSfr;
                }
#endif

                int PreThroughFocus_FindTurnRange(
                    __in const PRE_THROUGHFOCUS_PARAM &inParam,
                    __out PRE_THROUGHFOCUS_RESULT &result)
                {
                    PRE_THROUGHFOCUS_PARAM param;
                    memcpy(&param, &inParam, sizeof(PRE_THROUGHFOCUS_PARAM));
#ifdef OFFLINE_MODE
                    param.lpfnSetVcmMove = Offline_SetVcmMove;
                    param.lpfnGetSfrValue = Offline_GetSfrValue;
                    param.dPeakMin = 20.0;
                    param.dDownPersent = 0.80;
                    param.nDivideCount = 9;
                    param.nMinDac = 100;
                    param.nMaxDac = 450;
                    param.nStepDac = 4;
#endif
                    result.nFixedMinDac = param.nMinDac;
                    result.nFixedMaxDac = param.nMaxDac;
                    result.dMinSfr = 100.0;
                    result.dMaxSfr = 0.0;

                    int nArrDac[MAX_DIVIDE_COUNT + 1] = {0};
                    if (param.nMaxDac - param.nMinDac < param.nDivideCount)
                    {
                        return PTE_INPUT_RANGE_TOO_NARROW;
                    }
                    if (param.lpfnGetSfrValue == nullptr || param.lpfnSetVcmMove == nullptr)
                    {
                        return PTE_FUNCTION_POINT_NULL;
                    }

                    double dStepDac = (double)(param.nMaxDac - param.nMinDac) / (double)param.nDivideCount;

                    nArrDac[0] = param.nMinDac;
                    nArrDac[param.nDivideCount] = param.nMaxDac;
                    for (int i = 1; i < param.nDivideCount; i++)
                    {
                        nArrDac[i] = DOUBLE2INT(nArrDac[0] + i * dStepDac);
                    }

                    int nPeakIndex = 0;
                    double dArrSfr[MAX_DIVIDE_COUNT + 1];
                    for (int i = 0; i <= param.nDivideCount; i++)
                    {
                        param.lpfnSetVcmMove(nArrDac[i]);
                        //Sleep(param.nStepSleepTime);
						UTS::Algorithm::Sleep(param.nStepSleepTime);

                        dArrSfr[i] = param.lpfnGetSfrValue(nArrDac[i]);
                        if (dArrSfr[i] > result.dMaxSfr)
                        {
                            result.dMaxSfr = dArrSfr[i];
                            nPeakIndex = i;
                        }
                        if (dArrSfr[i] < result.dMinSfr)
                        {
                            result.dMinSfr = dArrSfr[i];
                        }
                    }

                    int nDownIndex = 0;
                    for (int i = nPeakIndex/*1*/; i <= param.nDivideCount; i++)
                    {
                        if (dArrSfr[i - 1] > 0)
                        {
                            if ((dArrSfr[nPeakIndex/*i - 1*/] >= param.dPeakMin)
                             && (dArrSfr[i] / dArrSfr[nPeakIndex/*i - 1*/] <= (1 - param.dDownPersent)))
                            {
                                nDownIndex = i;
                                break;
                            }
                        }
                    }

					if(dArrSfr[nPeakIndex] < param.dPeakMin)
					{
						return PTE_INPUT_RANGE_TOO_NARROW;
					}

					//!!--- 2018.08.27 Chi-Jen.Liao 依据Cindy方式，关闭此功能
					#if 1
					/*
                    if (nDownIndex >= 2)
                    {
                        double dPeakValue = 0.0;
                        for (int i = 0; i <= (nDownIndex - 1); i++)
                        {
                            if (dArrSfr[nDownIndex - i] > dPeakValue)
                            {
                                dPeakValue = dArrSfr[nDownIndex - i];
                                nPeakIndex = nDownIndex - i;
                            }
                        }
                        result.dMaxSfr = dPeakValue;
                        result.nFixedMaxDac = DOUBLE2INT(nArrDac[nDownIndex] - (nArrDac[nDownIndex] - nArrDac[nPeakIndex]) * (1 - (param.dDownPersent) / (1 - dArrSfr[nDownIndex] / dPeakValue)));
                        result.nFixedMinDac = DOUBLE2INT(result.nFixedMaxDac - 2 * dStepDac);
                        result.nFixedMaxDac = DOUBLE2INT(nArrDac[nDownIndex]);
                    }else
					{
						result.nFixedMinDac = DOUBLE2INT(nArrDac[nPeakIndex] - 2* dStepDac);
						result.nFixedMaxDac = DOUBLE2INT(nArrDac[nPeakIndex] + 2* dStepDac);
					}
					*/
					
					result.nFixedMinDac = DOUBLE2INT(nArrDac[nPeakIndex] - 2* dStepDac);
					result.nFixedMaxDac = DOUBLE2INT(nArrDac[nPeakIndex] + 2* dStepDac);
					
					#else
					
                    if (nDownIndex >= 2)
                    {
                        double dPeakValue = 0.0;
                        for (int i = 0; i <= 2; i++)
                        {
                            if (dArrSfr[nDownIndex - i] > dPeakValue)
                            {
                                dPeakValue = dArrSfr[nDownIndex - i];
                                nPeakIndex = nDownIndex - i;
                            }
                        }
                        result.dMaxSfr = dPeakValue;
                        result.nFixedMaxDac = DOUBLE2INT(nArrDac[nDownIndex] - (nArrDac[nDownIndex] - nArrDac[nPeakIndex]) * (1 - (param.dDownPersent) / (1 - dArrSfr[nDownIndex] / dPeakValue)));
                        result.nFixedMinDac = DOUBLE2INT(result.nFixedMaxDac - 2 * dStepDac);
                        result.nFixedMaxDac = DOUBLE2INT(nArrDac[nDownIndex]);
                    }else
					{
						result.nFixedMinDac = DOUBLE2INT(nArrDac[nPeakIndex] - 2* dStepDac);
						result.nFixedMaxDac = DOUBLE2INT(nArrDac[nPeakIndex] + 2* dStepDac);
					}
					#endif
					/*
                    else if (nDownIndex >= 1)
                    {
                        result.nFixedMinDac = nArrDac[nDownIndex - 1];
                        result.nFixedMaxDac = nArrDac[nDownIndex];
                    }
                    else
                    {
                        result.nFixedMinDac = nArrDac[nDownIndex];
                        result.nFixedMaxDac = nArrDac[nDownIndex];
                    }
					*/

                    result.nPeakDac = nArrDac[nPeakIndex];
                    
                    result.nFixedMinDac = result.nPeakDac - (((result.nPeakDac - result.nFixedMinDac) / param.nStepDac) + 1) * param.nStepDac;
                    result.nFixedMaxDac = result.nPeakDac + (((result.nFixedMaxDac - result.nPeakDac) / param.nStepDac) + 1) * param.nStepDac;
                    if (result.nFixedMinDac < param.nMinDac)
                    {
                        result.nFixedMinDac = param.nMinDac;
                    }
                    if (result.nFixedMaxDac > param.nMaxDac || result.nFixedMaxDac < result.nFixedMinDac)
                    {
                        //result.nFixedMaxDac = result.nFixedMaxDac - param.nStepDac;
						result.nFixedMaxDac = param.nMaxDac;
                    }
                    return PTE_OK;
                }
            }
        }
    }
}
