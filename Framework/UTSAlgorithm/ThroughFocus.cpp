#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::PreFocus;

namespace UTS
{
    namespace Algorithm
    {
        namespace ThroughFocus
        {
            namespace ThroughFocus_Std
            {
                const THROUGHFOCUS_PARAM *g_pParam;
                //THROUGHFOCUS_RESULT *g_pResult;
                
                void LogOut(LPCTSTR lpFormat, ...)
                {
                    va_list argList;
                    va_start(argList, lpFormat);

                    CString strMsg;
                    strMsg.FormatV(lpFormat, argList);
                    if (g_pParam->lpfnLogOut != nullptr)
                    {
                        g_pParam->lpfnLogOut(strMsg);
                    }

                    va_end(argList);
                }
                
                double DoStepSfr_PhasePrefocus(__in int nCurrentDac)
                {
                    double dKeySfr = 0.0;
                    if (g_pParam != nullptr)
                    {
                        g_pParam->lpfnDoStepSfr(TFP_PRE_FOCUS, nCurrentDac, dKeySfr);
                    }
                    return dKeySfr;
                }
                
                int DoThroughFocus(
                    __in BOOL *pbIsRunning,
                    __inout THROUGHFOCUS_PARAM &param)
                {
                    //-------------------------------------------------------------------------
                    // init
                    double dThroughSFRMin = 100.0;
                    double dThroughSFRMax = 0.0;
                    g_pParam = &param;
                    int nThroughPeakDac = param.nMinDac;

                    //-------------------------------------------------------------------------
                    // check param
                    if (nullptr == param.lpfnDoStepSfr
                        || nullptr == param.lpfnSetVcmMove
                        || param.nMinDac > param.nMaxDac
                        || param.nDetailStepDac > param.nMaxDac - param.nMinDac
                        || param.dPreFocusDownRate > 1.0
                        || param.dPreFocusDownRate < 0.0
                        || param.dPreFocusPeakMin > 100.0
                        || param.dPreFocusPeakMin < 0.0)
                    {
                        return TFE_PARAM_ERROR;
                    }

                    LogOut(_T("Through focus start."));
                    param.lpfnSetVcmMove(0);

                    //-------------------------------------------------------------------------
                    // Pre focus
                    if (param.nPreFocusEn != 0)
                    {
                        LogOut(_T("Prefocus Phase--------------------"));
                        int nRet = 0;
                        PreFocus_BigStepThrough::PRE_THROUGHFOCUS_PARAM preFocusParam;
                        preFocusParam.nMinDac = param.nMinDac;
                        preFocusParam.nMaxDac = param.nMaxDac;
                        preFocusParam.nStepDac = param.nDetailStepDac;
                        preFocusParam.nStepSleepTime = param.nPreFocusStepTime;
                        preFocusParam.nDivideCount = param.nPreFocusStepCount;
                        preFocusParam.dDownPersent = param.dPreFocusDownRate;
                        preFocusParam.dPeakMin = param.dPreFocusPeakMin;
                        preFocusParam.lpfnSetVcmMove = param.lpfnSetVcmMove;
                        preFocusParam.lpfnGetSfrValue = DoStepSfr_PhasePrefocus;
                        PreFocus_BigStepThrough::PRE_THROUGHFOCUS_RESULT result = {0};
                        nRet = PreFocus_BigStepThrough::PreThroughFocus_FindTurnRange(preFocusParam, result);
                        if (nRet != PreFocus_BigStepThrough::PTE_OK)
                        {
                            LogOut(_T("PreFocus Error[%d]"), nRet);
                            return TFE_PRE_FOCUS_ERROR;
                        }
                        LogOut(_T("Peak Range[%d - %d]"), result.nFixedMinDac, result.nFixedMaxDac);

                        param.nMinDac = result.nFixedMinDac;
                        param.nMaxDac = result.nFixedMaxDac;
                        dThroughSFRMin = result.dMinSfr;
                        dThroughSFRMax = result.dMaxSfr;
                        nThroughPeakDac = result.nPeakDac;
                    }
                    
                    //-------------------------------------------------------------------------
                    // Detail Through Focus Process
                    LogOut(_T("Detail focus Phase--------------------"));
                    int nStartDac = 0;
                    int nEndDac = 0;
                    int nStepDac = 0;
                    int nCondition = 0;
                    if (param.nDetailReverseThroughEn == 0)
                    {
                        //-------------------------------------------------------------------------
                        // Ìøµ½0
                        param.lpfnSetVcmMove(0);
                        nStartDac = param.nMinDac;
                        nEndDac = param.nMaxDac;
                        nStepDac = param.nDetailStepDac;
                        nCondition = 1;
                    }
                    else
                    {
                        nStartDac = param.nMaxDac;
                        nEndDac = param.nMinDac;
                        nStepDac = -param.nDetailStepDac;
                        nCondition = -1;
                    }
                    int nCurrentDac = 0;
                    for (nCurrentDac = nStartDac;
                        (nCurrentDac * nCondition) <= (nEndDac * nCondition);
                        nCurrentDac += nStepDac)
                    {
                        if (!(*pbIsRunning))
                        {
                            LogOut(_T("User Stop."));
                            return TFE_USER_STOP;
                        }

                        param.lpfnSetVcmMove(nCurrentDac);
                        if (nCurrentDac == nStartDac)
                        {
                            //Sleep(param.nSuperStepTime);
							UTS::Algorithm::Sleep(param.nSuperStepTime);
                        }
                        else
                        {
                            //Sleep(param.nDetailStepTime);
							UTS::Algorithm::Sleep(param.nDetailStepTime);
                        }

                        double dCurrentDacValue = 0.0;
                        BOOL bRet = param.lpfnDoStepSfr(TFP_DETAIL_FOCUS, nCurrentDac, dCurrentDacValue);
                        if (!bRet)
                        {
                            LogOut(_T("Do step sfr error(Detail focus Phase)."));
                            return TFE_DO_STEP_SFR_ERROR;
                        }

                        if (dCurrentDacValue > dThroughSFRMax)
                        {
                            dThroughSFRMax = dCurrentDacValue;
                            nThroughPeakDac = nCurrentDac;
                        }
                        if (dCurrentDacValue < dThroughSFRMin)
                        {
                            dThroughSFRMin = dCurrentDacValue;
                        }
                    }

                    //-------------------------------------------------------------------------
                    // Find Best
                    double dLastBestSFR = 0.0;
                    LogOut(_T("Find Best SFR Phase--------------------"));
					LogOut(_T("Expected Peak Dac = %d, Peak Value = %.1f"), nThroughPeakDac, dThroughSFRMax);
//Add by Spac @20170811 For SFR issue
					if(nThroughPeakDac<param.nMinDac - 2*param.nDetailStepDac)
					{
						LogOut(_T("Through Peak Dac too low"));
						return TFE_DO_STEP_SFR_ERROR;
					}
					if(nThroughPeakDac>param.nMaxDac + 2*param.nDetailStepDac)
					{
						LogOut(_T("Through Peak Dac too High"));
						return TFE_DO_STEP_SFR_ERROR;
					}
					param.lpfnSetVcmMove(0);
                    int nBeforeDac = nThroughPeakDac - 5 * param.nDetailStepDac;
//                      if (nBeforeDac < param.nMinDac)
//                      {
//                          nBeforeDac = param.nMinDac;
//                      }  
					 if (nBeforeDac < 0)
					 {
						 nBeforeDac = 0;
					 } 
//					 param.lpfnSetVcmMove(0);
//                    int nBeforeDac = nThroughPeakDac - 5 * param.nDetailStepDac;
//                    if (nBeforeDac < 0)
//                    {
//                        nBeforeDac = 0;
//                    }
//                     if (nBeforeDac < 0)
//                     {
//                         nBeforeDac = 0;
//                     }
//End
                    for (int i = 1; i <= 5; i++)
                    {
                        nCurrentDac = nBeforeDac + i * param.nDetailStepDac;
                        param.lpfnSetVcmMove(nCurrentDac);
                        if (i == 1)
                        {
                            //Sleep(param.nSuperStepTime);
							UTS::Algorithm::Sleep(param.nSuperStepTime);
                        }
                        else
                        {
                            //Sleep(param.nDetailStepTime);
							UTS::Algorithm::Sleep(param.nDetailStepTime);
                        }
//Add by Spac @20170811 for SFR issue
                        BOOL bRet;
						if(param.nSfrType)
						{
							double TempSFR=0;

							for(int j=0;j<param.nSfrStackCount;j++)
							{
								bRet|=param.lpfnDoStepSfr(TFP_FIND_BEST, nCurrentDac, dLastBestSFR);
								TempSFR+=dLastBestSFR;
							}
							dLastBestSFR=TempSFR/param.nSfrStackCount;
						}
						else
						{
							bRet=param.lpfnDoStepSfr(TFP_FIND_BEST, nCurrentDac, dLastBestSFR);
						}
//						BOOL bRet=param.lpfnDoStepSfr(TFP_FIND_BEST, nCurrentDac, dLastBestSFR);
//End
                        if (!bRet)
                        {
                            LogOut(_T("Do step sfr error(Find Best SFR Phase)."));
                            return TFE_DO_STEP_SFR_ERROR;
                        }
                    }

                    LogOut(_T("Through focus end."));
                    return TFE_OK;
                }
            }   // namespace ThroughFocus_Std
        }   // namespace ThroughFocus
    }   // namespace Algorithm
}   // namespace UTS
