#include "StdAfx.h"
#include "TestFlow.h"
//#include "Global_Memory_SFC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    int TestFlow::m_bIsRunning = FALSE;

    TestFlow::TestFlow(void)
    {
        m_vecOpList.clear();
        m_vecOpDllHandle.clear();
        m_vecInitOpList.clear();
        m_vecInitOpDllHandle.clear();
        m_vecFinalOpList.clear();
        m_vecFinalOpDllHandle.clear();
    }

    TestFlow::~TestFlow(void)
    {
        Destroy();
    }

    BOOL TestFlow::Initialize(HWND hDisplayWnd)
    {
        m_flowStatus = eFlowStatus::Initializing;
        m_bIsRunning = TRUE;

        if (!InitDevice(hDisplayWnd))
        {
            uts.log.Error(_T("[%s]InitDevice Fail!"), UTS_FN);
            return FALSE;
        }

        if (!InitInitOperator())
        {
            uts.log.Error(_T("[%s]InitInitOperator Fail!"), UTS_FN);
            return FALSE;
        }

        if (!InitTestOperator())
        {
            uts.log.Error(_T("[%s]InitTestOperator Fail!"), UTS_FN);
            return FALSE;
        }

        if (!InitFinalOperator())
        {
            uts.log.Error(_T("[%s]InitFinalOperator Fail!"), UTS_FN);
            return FALSE;
        }

        if (!uts.errorcode.GetAllErrorCodeInfoFromDB())
        {
            uts.log.Error(_T("GetAllErrorCodeInfoFromDB Fail."));
            return FALSE;
        }

        return TRUE;
    }

    BOOL TestFlow::InitDevice(HWND hDisplayWnd)
    {
        CString strDeviceFilename;
        if (!uts.dbCof.GetDeviceFilename(uts.info.nDeviceId, strDeviceFilename))
        {
            uts.log.Error(_T("[%s]GetDeviceFilename Fail. nDeviceId = %d"), UTS_FN, uts.info.nDeviceId);
            return FALSE;
        }
        //------------------------------------------------------------------------------
        // load device dll
        typedef BaseDevice* (*LPFN_GETDEVICE)(void);
        LPFN_GETDEVICE lpfnGetDevice;
        CString strDvDllName = uts.info.strBinPath + strDeviceFilename;
        HMODULE hModule = ::LoadLibrary(strDvDllName);
        if (nullptr != hModule)
        {
            m_hDvDllHandle = hModule;
            lpfnGetDevice = (LPFN_GETDEVICE)GetProcAddress(hModule, "GetDevice");
            if (nullptr != lpfnGetDevice)
            {
                m_pDevice = lpfnGetDevice();
                if (!m_pDevice->Initialize(hDisplayWnd))
                {
                    uts.log.Error(_T("[%s]m_pDevice->Initialize Fail!"), UTS_FN);
                    return FALSE;
                }
            }
            else
            {
                uts.log.Error(_T("[%s]Export %s function Fail!"), UTS_FN, _T("GetDevice"));
                return FALSE;
            }
        }
        else
        {
            uts.log.Error(_T("[%s]LoadLibrary(%s) Fail!"), UTS_FN, strDvDllName);
            return FALSE;
        }

        return TRUE;
    }

    BOOL TestFlow::InitInitOperator()
    {
        //------------------------------------------------------------------------------
        // load initial start operator dll
        vector<CString> vecInitOperatorList;
        if (!uts.dbCof.GetInitOperatorList(vecInitOperatorList))
        {
            uts.log.Error(_T("Get init operator list error."));
            return FALSE;
        }
        int nIndex = 0;
        typedef BaseOperator* (*LPFN_GETOPERATOR)(void);
        LPFN_GETOPERATOR lpfnGetOperator;
        for (size_t i = 0; i < vecInitOperatorList.size(); i++)
        {
            CString strOpName = vecInitOperatorList[i];
            CString strDllName = uts.info.strBinPath + strOpName;
            HMODULE hModule = ::LoadLibrary(strDllName);
            if (nullptr != hModule)
            {
                m_vecInitOpDllHandle.push_back(hModule);
                lpfnGetOperator = (LPFN_GETOPERATOR)GetProcAddress(hModule, "GetOperator");
                if (nullptr != lpfnGetOperator)
                {
                    // 初始化Operator
                    OPERATOR_INFO opInfo;
                    opInfo.nIndex = nIndex++;
                    opInfo.strFileName = strOpName;
                    opInfo.pInstance = lpfnGetOperator();
                    opInfo.pInstance->SetDevice(m_pDevice);
                    m_vecInitOpList.push_back(opInfo);
                    // 初始化DB
                    opInfo.pInstance->OnReadSpec();
                    // 初始化ReturnValue
                    CString strFileName = OSUtil::GetFileName(strDllName);
                    size_t i = 0;
                    for (; i < uts.info.vecReturnValueItem.size(); i++)
                    {
                        if (0 == uts.info.vecReturnValueItem[i].strDllFilename.CompareNoCase(strFileName))
                        {
                            break;
                        }
                    }
                    if (i == uts.info.vecReturnValueItem.size())
                    {
                        RETURN_VALUE_ITEM item = {0};
                        item.strDllFilename = strFileName;
                        opInfo.pInstance->OnGetErrorReturnValueList(item.vecReturnValue);
                        uts.info.vecReturnValueItem.push_back(item);
                    }
                }
                else
                {
                    uts.log.Error(_T("[%s]Export %s function Fail!"), UTS_FN, _T("GetOperator"));
                    return FALSE;
                }
            }
            else
            {
                uts.log.Error(_T("[%s]LoadLibrary(%s) Fail!"), UTS_FN, strDllName);
                return FALSE;
            }
        }

        return TRUE;
    }

    BOOL TestFlow::InitTestOperator()
    {
        //------------------------------------------------------------------------------
        // load test operator dll
        vector<CString> vecTestOperatorList;
        if (!uts.dbCof.GetTestOperatorList(vecTestOperatorList))
        {
            uts.log.Error(_T("Get test operator list error."));
            return FALSE;
        }
        int nIndex = 0;
        typedef BaseOperator* (*LPFN_GETOPERATOR)(void);
        LPFN_GETOPERATOR lpfnGetOperator;
        for (size_t i = 0; i < vecTestOperatorList.size(); i++)
        {
            CString strOpName = vecTestOperatorList[i];
            CString strDllName = uts.info.strBinPath + strOpName;
            HMODULE hModule = ::LoadLibrary(strDllName);
            if (nullptr != hModule)
            {
                m_vecOpDllHandle.push_back(hModule);
                lpfnGetOperator = (LPFN_GETOPERATOR)GetProcAddress(hModule, "GetOperator");
                if (nullptr != lpfnGetOperator)
                {
                    // 初始化Operator
                    OPERATOR_INFO opInfo;
                    opInfo.nIndex = nIndex++;
                    opInfo.strFileName = strOpName;
                    opInfo.pInstance = lpfnGetOperator();
                    opInfo.pInstance->SetDevice(m_pDevice);
                    m_vecOpList.push_back(opInfo);
                    // 初始化DB
                    opInfo.pInstance->OnReadSpec();
                    // 初始化ReturnValue
                    CString strFileName = OSUtil::GetFileName(strDllName);
                    size_t i = 0;
                    for (; i < uts.info.vecReturnValueItem.size(); i++)
                    {
                        if (0 == uts.info.vecReturnValueItem[i].strDllFilename.CompareNoCase(strFileName))
                        {
                            break;
                        }
                    }
                    if (i == uts.info.vecReturnValueItem.size())
                    {
                        RETURN_VALUE_ITEM item = {0};
                        item.strDllFilename = strFileName;
                        opInfo.pInstance->OnGetErrorReturnValueList(item.vecReturnValue);
                        uts.info.vecReturnValueItem.push_back(item);
                    }
                }
                else
                {
                    uts.log.Error(_T("[%s]Export %s function Fail!"), UTS_FN, _T("GetOperator"));
                    return FALSE;
                }
            }
            else
            {
                uts.log.Error(_T("[%s]LoadLibrary(%s) Fail!"), UTS_FN, strDllName);
                return FALSE;
            }
        }

        return TRUE;
    }

    BOOL TestFlow::InitFinalOperator()
    {
        //------------------------------------------------------------------------------
        // load Final operator dll
        vector<CString> vecFinalOperatorList;
        if (!uts.dbCof.GetFinalOperatorList(vecFinalOperatorList))
        {
            uts.log.Error(_T("Get Final operator list error."));
            return FALSE;
        }
        int nIndex = 0;
        typedef BaseOperator* (*LPFN_GETOPERATOR)(void);
        LPFN_GETOPERATOR lpfnGetOperator;
        for (size_t i = 0; i < vecFinalOperatorList.size(); i++)
        {
            CString strOpName = vecFinalOperatorList[i];
            CString strDllName = uts.info.strBinPath + strOpName;
            HMODULE hModule = ::LoadLibrary(strDllName);
            if (nullptr != hModule)
            {
                m_vecFinalOpDllHandle.push_back(hModule);
                lpfnGetOperator = (LPFN_GETOPERATOR)GetProcAddress(hModule, "GetOperator");
                if (nullptr != lpfnGetOperator)
                {
                    // 初始化Operator
                    OPERATOR_INFO opInfo;
                    opInfo.nIndex = nIndex++;
                    opInfo.strFileName = strOpName;
                    opInfo.pInstance = lpfnGetOperator();
                    opInfo.pInstance->SetDevice(m_pDevice);
                    m_vecFinalOpList.push_back(opInfo);
                    // 初始化DB
                    opInfo.pInstance->OnReadSpec();
                    // 初始化ReturnValue
                    CString strFileName = OSUtil::GetFileName(strDllName);
                    size_t i = 0;
                    for (; i < uts.info.vecReturnValueItem.size(); i++)
                    {
                        if (0 == uts.info.vecReturnValueItem[i].strDllFilename.CompareNoCase(strFileName))
                        {
                            break;
                        }
                    }
                    if (i == uts.info.vecReturnValueItem.size())
                    {
                        RETURN_VALUE_ITEM item = {0};
                        item.strDllFilename = strFileName;
                        opInfo.pInstance->OnGetErrorReturnValueList(item.vecReturnValue);
                        uts.info.vecReturnValueItem.push_back(item);
                    }
                }
                else
                {
                    uts.log.Error(_T("[%s]Export %s function Fail!"), UTS_FN, _T("GetOperator"));
                    return FALSE;
                }
            }
            else
            {
                uts.log.Error(_T("[%s]LoadLibrary(%s) Fail!"), UTS_FN, strDllName);
                return FALSE;
            }
        }

        return TRUE;
    }

    BOOL TestFlow::DoInitialOperatorList()
    {
        //------------------------------------------------------------------------------
        // execute initial start operator list
        BOOL bResult = TRUE;
        int nErrorCode = uts.errorcode.E_Fail;
        CString strLatestOpFileName;
        vector<OPERATOR_INFO>::iterator itorOp = m_vecInitOpList.begin();
        for (; itorOp != m_vecInitOpList.end(); itorOp++)
        {
            BaseOperator *pInstance = (*itorOp).pInstance;
            strLatestOpFileName = (*itorOp).strFileName;
            //nIndex = (*itorOp).nIndex;

            BOOL bStopOnFail = FALSE;
            if (pInstance->IsMustStopOnFail())
            {
                bStopOnFail = TRUE;
            }
            else
            {
                bStopOnFail = (uts.info.nStopOnFail != 0);
            }

            bResult = pInstance->OnReadSpec();
            if (!bResult)
            {
                if (bStopOnFail)
                {
                    break;
                }
            }
            bResult = pInstance->OnTest(&m_bIsRunning, &nErrorCode);
            // 先判断是否由于Stop结束
            if (!m_bIsRunning)
            {
				//!!---Chi-Jen.Liao取消SFC
//                 if (uts.info.nShopFlowEn != 0)
//                 {
//                     GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
//                     if (nullptr != gmsfc)
//                     {
//                         gmsfc->bOverAllResult = FALSE;
//                     }
//                 }
                uts.board.ShowStatus(eBoardStatus::Status_Stop);
                m_flowStatus = eFlowStatus::Stoping;
                return TRUE;
            }
            // 再判断结果
            if (!bResult)
            {
                //------------------------------------------------------------------------------
                // if Fail, show error message
                CString strErrorMsg = uts.errorcode.GetErrorMsg(strLatestOpFileName, nErrorCode);
                uts.board.ShowErrorMsg(strErrorMsg);
				//!!---Chi-Jen.Liao取消SFC
//                 if (uts.info.nShopFlowEn != 0)
//                 {
//                     GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
//                     if (nullptr != gmsfc)
//                     {
//                         gmsfc->sSFC_ErrorCode.Format(_T("%d"), nErrorCode);
//                         gmsfc->sSFC_ErrorDesc = strErrorMsg;
//                     }
//                 }
                if (bStopOnFail)
                {
                    break;
                }
            }
        }

        // pass fail
        if (bResult)
        {
            uts.board.ShowStatus(eBoardStatus::Status_Ready);
        }
        else
        {
            uts.board.ShowStatus(eBoardStatus::Status_Fail);
        }
        m_flowStatus = eFlowStatus::Ready;
        return TRUE;
    }

    BOOL TestFlow::DoFinalOperatorList()
    {
        //------------------------------------------------------------------------------
        // execute final operator list
        BOOL bResult = TRUE;
        int nErrorCode = uts.errorcode.E_Fail;
        CString strLatestOpFileName;
        vector<OPERATOR_INFO>::iterator itorOp = m_vecFinalOpList.begin();
        for (; itorOp != m_vecFinalOpList.end(); itorOp++)
        {
            BaseOperator *pInstance = (*itorOp).pInstance;
            strLatestOpFileName = (*itorOp).strFileName;
            //nIndex = (*itorOp).nIndex;

            BOOL bStopOnFail = FALSE;
            if (pInstance->IsMustStopOnFail())
            {
                bStopOnFail = TRUE;
            }
            else
            {
                bStopOnFail = (uts.info.nStopOnFail != 0);
            }

            uts.log.Debug(_T("Reading final operator spec: %s"), strLatestOpFileName);
            bResult = pInstance->OnReadSpec();
            if (!bResult)
            {
                if (bStopOnFail)
                {
                    break;
                }
            }
            uts.log.Debug(_T("Doing final operator test: %s"), strLatestOpFileName);
            bResult = pInstance->OnTest(&m_bIsRunning, &nErrorCode);
            // 先判断是否由于Stop结束
            if (!m_bIsRunning)
            {
                uts.log.Debug(_T("User stop final operator test: %s"), strLatestOpFileName);
                uts.board.ShowStatus(eBoardStatus::Status_Stop);
                m_flowStatus = eFlowStatus::Stoping;
				//!!---Chi-Jen.Liao取消SFC
//                 if (uts.info.nShopFlowEn != 0)
//                 {
//                     GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
//                     if (nullptr != gmsfc)
//                     {
//                         gmsfc->bOverAllResult = FALSE;
//                     }
//                 }
                return TRUE;
            }
            // 再判断结果
            if (!bResult)
            {
                //------------------------------------------------------------------------------
                // if Fail, show error message
                CString strErrorMsg = uts.errorcode.GetErrorMsg(strLatestOpFileName, nErrorCode);
                uts.board.ShowErrorMsg(strErrorMsg);
				uts.board.ShowStatus(eBoardStatus::Status_Fail); //2018/04/15 EdwardChen add for MES insert fail
				//!!---Chi-Jen.Liao取消SFC
//                 if (uts.info.nShopFlowEn != 0)
//                 {
//                     GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
//                     if (nullptr != gmsfc)
//                     {
//                         gmsfc->sSFC_ErrorCode.Format(_T("%d"), nErrorCode);
//                         gmsfc->sSFC_ErrorDesc = strErrorMsg;
//                     }
//                 }
                /*if (bStopOnFail)
                {
                    break;
                }*/
            }
        }

        // pass fail
        if (bResult)
        {
            //uts.board.ShowStatus(eBoardStatus::Status_Ready);
        }
        else
        {
            uts.board.ShowStatus(eBoardStatus::Status_Fail);
        }
        //m_flowStatus = eFlowStatus::Ready;
        uts.log.Debug(_T("Final operator[%s] test end."), strLatestOpFileName);
        return TRUE;
    }

    /*
    Op中增加Initialize
    此处增加Preview
    起线程
    逐个调用OP的Preview
    按下Test后，停止该线程
    */
    void TestFlow::Test()
    {
        CSingleLock sl(&m_cs, TRUE);
        if (m_vecOpList.size() > 0)
        {
            m_flowStatus = eFlowStatus::Testing;

            m_bIsRunning = TRUE;
            m_pThread = AfxBeginThread(TestThreadProc, this);
            m_pThread->m_bAutoDelete = FALSE;
        }
    }

    //-------------------------------------------------------------------------
    // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
    void TestFlow::SaveThroughResult(LPCTSTR lpThroughResult)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        //-------------------------------------------------------------------------
        // 输出Data file
        CString strDirPath;
        strDirPath.Format(_T("%s\\Data-%s-%s-%s-%04d%02d%02d\\"),
            uts.info.strResultPath,
            uts.info.strProjectName,
            uts.info.strLineName,
            uts.info.strStationName,
            st.wYear,
            st.wMonth, 
            st.wDay);
        UTS::OSUtil::CreateMultipleDirectory(strDirPath);
        CString strFilePath;
        strFilePath.Format(_T("%s\\%s-%04d%02d%02d.csv"),
            strDirPath,
            _T("ThroughResult"),
            st.wYear,
            st.wMonth, 
            st.wDay);

        FILE* fp = NULL;
        long fileLength ;
        errno_t etRet = _tfopen_s(&fp, strFilePath, _T("a+"));
        if (0 != etRet)
        {
            uts.log.Error(_T("Open file Fail. path = %s"), strFilePath);
            return;
        }
        fseek(fp, 0, SEEK_END);
        fileLength = ftell(fp);

        CString strTime;
        strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d:%03d"),
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        //------------------------------------------------------------------------------
        // 输出文件头
        if (fileLength < 3)
        {	
            _ftprintf_s(fp, _T("Time,SN,Result\n"));
        }

        //------------------------------------------------------------------------------
        // 输出文件内容
        _ftprintf_s(fp, _T("%s,%s,%s\n"), strTime, uts.info.strSN, lpThroughResult);
        uts.log.Debug(_T("%s,%s"), uts.info.strSN, lpThroughResult);
        fclose(fp);
    }
    //-------------------------------------------------------------------------

    UINT TestFlow::TestThreadProc(PVOID param)
    {
        int nRet = 0;
        BOOL bResult = TRUE;
		uts.info.bHasTestError = FALSE;
        int nErrorCode = uts.errorcode.E_Fail;
        TestFlow *pThis = (TestFlow *)param;
		uts.info.nErrorCode=uts.errorcode.E_Pass;
        //-------------------------------------------------------------------------
        // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
        CString strThroughResultHead;
        CString strThroughResultBody;
        CString strThroughResult;
        int nPassCode = uts.errorcode.E_Pass;
        //-------------------------------------------------------------------------
        BOOL bAlreadyShowErrorMsg = FALSE;  // fix bug #2: Stop on fail模式为0的时候，最终显示结果为最后一项，应该为第一项；Errorcode保留优先级最高的
        int nIndex = 0;
        CString strLatestOpFileName;
        BOOL bFirstOp = FALSE;
        HighPrecisionTimer tc;
        vector<OPERATOR_INFO>::iterator itorOp = pThis->m_vecOpList.begin();
        uts.info.mapShared["Enable_Jump"] = -1.0;
        for (; itorOp != pThis->m_vecOpList.end(); itorOp++)
        {
            bFirstOp = (pThis->m_vecOpList.begin() == itorOp);
            BaseOperator *pInstance = (*itorOp).pInstance;
            strLatestOpFileName = (*itorOp).strFileName;
            nIndex = (*itorOp).nIndex;
            
            //-------------------------------------------------------------------------
            // 判断特殊OP
            if (uts.info.mapShared["Enable_Jump"] > 0)
            {
                uts.info.mapShared["Enable_Jump"] = -1.0;
                int nNodeNumber = DOUBLE2INT(uts.info.mapShared["Jump_To"]);
                CString strJumpToNodeName;
                strJumpToNodeName.Format(_T("UTOP_NODE_%d"), nNodeNumber);
                for (;
                    itorOp != pThis->m_vecOpList.end() && OSUtil::GetMainFileName((*itorOp).strFileName).CompareNoCase(strJumpToNodeName) != 0;
                    itorOp++);
                if (itorOp == pThis->m_vecOpList.end()) break;

                bFirstOp = (pThis->m_vecOpList.begin() == itorOp);
                pInstance = (*itorOp).pInstance;
                strLatestOpFileName = (*itorOp).strFileName;
                nIndex = (*itorOp).nIndex;
            }
            else if (strLatestOpFileName.MakeUpper().Find(_T("UTOP_END")) == 0)
            {
                break;
            }
            
            strThroughResultBody.AppendFormat(_T(",%s:"), strLatestOpFileName);   // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表

            BOOL bStopOnFail = FALSE;
            if (pInstance->IsMustStopOnFail())
            {
                bStopOnFail = TRUE;
            }
            else
            {
                bStopOnFail = (uts.info.nStopOnFail != 0);
            }
            uts.board.SetItemTesting(nIndex);
            bResult = pInstance->OnReadSpec();
            if (!bResult)
            {
                strThroughResultBody.Append(FAIL_STR);  // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
                if (bFirstOp)
                {
                    tc.SetStartTime();
                    strThroughResultBody.Append(_T(":0"));
                }
                else
                {
                    strThroughResultBody.AppendFormat(_T(":%.1f"), tc.GetPassTime());
                }
                if (bStopOnFail)
                {
                    break;
                }
            }
            bResult = pInstance->OnTest(&m_bIsRunning, &nErrorCode);
            if (nErrorCode == uts.errorcode.E_PassA
                || nErrorCode == uts.errorcode.E_PassB
                || nErrorCode == uts.errorcode.E_PassC)
            {
                nPassCode = nErrorCode;
            }
            // 先判断是否由于Stop结束
            if (!pThis->m_bIsRunning)
            {
                strThroughResultHead.Append(_T("STOP"));  // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
                strThroughResultBody.Append(_T("STOP"));  // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
                uts.info.bHasTestError = TRUE;
                if (bFirstOp)
                {
                    tc.SetStartTime();
                    strThroughResultBody.Append(_T(":0"));
                }
                else
                {
                    strThroughResultBody.AppendFormat(_T(":%.1f"), tc.GetPassTime());
                }

                uts.board.SetItemFail(nIndex);
                uts.board.ShowStatus(eBoardStatus::Status_Stop);
                TRACE("UTSFlow::ThreadProc stop.\n");

                pThis->m_flowStatus = eFlowStatus::Stoping;
                nRet = 1;
                goto end;
            }
            // 再判断结果
			uts.info.bMesResult=uts.errorcode.E_Pass;
			if (uts.info.bMesResult || bResult==FALSE) //EdwardChen add start for MES 2018/04/15
				uts.info.bMesResult=1;
			else
				uts.info.bMesResult=0;

			uts.log.Debug(_T("Result=%d"),bResult); //EdwardChen add end for MES 2018/04/15 
            if (!bResult)
            {
                strThroughResultBody.Append(FAIL_STR);  // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
                uts.info.bHasTestError = TRUE;
                if (bFirstOp)
                {
                    tc.SetStartTime();
                    strThroughResultBody.Append(_T(":0"));
                }
                else
                {
                    strThroughResultBody.AppendFormat(_T(":%.1f"), tc.GetPassTime());
                }
                uts.board.SetItemFail(nIndex);

                //-------------------------------------------------------------------------
                // fix bug #2: Stop on fail模式为0的时候，最终显示结果为最后一项，应该为第一项；Errorcode保留优先级最高的
                if (!bAlreadyShowErrorMsg)
                {
                    // if Fail, show error message
                    CString strErrorMsg = uts.errorcode.GetErrorMsg(strLatestOpFileName, nErrorCode);
					uts.info.nErrorCode=nErrorCode;
                    uts.board.ShowErrorMsg(strErrorMsg);
                    bAlreadyShowErrorMsg = TRUE;
					//!!---Chi-Jen.Liao取消SFC
//                     if (uts.info.nShopFlowEn != 0)
//                     {
//                         GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
//                         gmsfc->sSFC_ErrorCode.Format(_T("%d"), nErrorCode);
//                         gmsfc->sSFC_ErrorDesc = strErrorMsg;
//                     }
                }
                //-------------------------------------------------------------------------

                if (bStopOnFail)
                {
                    break;
                }
            }
            else
            {
                strThroughResultBody.Append(PASS_STR);  // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
                if (bFirstOp)
                {
                    tc.SetStartTime();
                    strThroughResultBody.Append(_T(":0"));
                }
                else
                {
                    strThroughResultBody.AppendFormat(_T(":%.1f"), tc.GetPassTime());
                }
                uts.board.SetItemPass(nIndex);
            }
        }

        // pass fail
        if (!bAlreadyShowErrorMsg)  // fix bug #2: Stop on fail模式为0的时候，最终显示结果为最后一项，应该为第一项；Errorcode保留优先级最高的
        {
            strThroughResultHead.Append(PASS_STR);  // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
            if (nPassCode == uts.errorcode.E_Pass)
            {
                uts.board.ShowStatus(eBoardStatus::Status_Pass);
            }
            else if (nPassCode == uts.errorcode.E_PassA)
            {
                uts.board.ShowStatus(_T("PASS-A"));
            }
            else if (nPassCode == uts.errorcode.E_PassB)
            {
                uts.board.ShowStatus(_T("PASS-B"));
            }
            else if (nPassCode == uts.errorcode.E_PassC)
            {
                uts.board.ShowStatus(_T("PASS-C"));
            }
        }
        else
        {
            strThroughResultHead.Append(FAIL_STR);  // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
            uts.board.ShowStatus(eBoardStatus::Status_Fail);
			//!!---Chi-Jen.Liao取消SFC
//             if (uts.info.nShopFlowEn != 0)
//             {
//                 GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC *)uts.mapUserMemory["SFC"];
//                 gmsfc->bOverAllResult = FALSE;
//             }
            uts.info.bHasTestError = TRUE;
        }

        TRACE("UTSFlow::ThreadProc end.\n");
        pThis->m_flowStatus = eFlowStatus::Stoping;
end:
        //-------------------------------------------------------------------------
        // fix 改善 #4: 白板分开测试导致资料也分开，需要一个总的履历表
        strThroughResult = strThroughResultHead + strThroughResultBody;
        TestFlow::SaveThroughResult(strThroughResult);
        //-------------------------------------------------------------------------
        return nRet;
    }

    void TestFlow::Stop()
    {
        m_flowStatus = eFlowStatus::Stoping;
        m_bIsRunning = FALSE;
    }

    void TestFlow::WaitTestFinish()
    {
        CSingleLock sl(&m_cs, TRUE);
        if (nullptr != m_pThread)
        {
            DWORD dwExitCode = 0;
            do 
            {
                Sleep(50);
                GetExitCodeThread(m_pThread->m_hThread, &dwExitCode);
            } while (dwExitCode == STILL_ACTIVE);
            RELEASE_POINTER(m_pThread);
        }

        // do final
        DoFinalOperatorList();
        
        if (nullptr != m_pDevice)
        {
            m_pDevice->Stop();
        }

        m_flowStatus = eFlowStatus::Ready;
    }

    void TestFlow::Destroy()
    {
        Stop();
        if (nullptr != m_pDevice)
        {
            m_pDevice->Close();
        }
        vector<OPERATOR_INFO>::iterator itorOp;
        vector<HMODULE>::iterator itorModule;
        //------------------------------------------------------------------------------
        // 清理Test operator
        itorOp = m_vecOpList.begin();
        for (; itorOp != m_vecOpList.end(); itorOp++)
        {
            RELEASE_POINTER((*itorOp).pInstance);
        }
        itorModule = m_vecOpDllHandle.begin();
        for (; itorModule != m_vecOpDllHandle.end(); itorModule++)
        {
            ::FreeLibrary(*itorModule);
        }
        //------------------------------------------------------------------------------
        // 清理启动operator
        itorOp = m_vecInitOpList.begin();
        for (; itorOp != m_vecInitOpList.end(); itorOp++)
        {
            RELEASE_POINTER((*itorOp).pInstance);
        }
        itorModule = m_vecInitOpDllHandle.begin();
        for (; itorModule != m_vecInitOpDllHandle.end(); itorModule++)
        {
            ::FreeLibrary(*itorModule);
        }
        //------------------------------------------------------------------------------
        // 清理Final operator
        itorOp = m_vecFinalOpList.begin();
        for (; itorOp != m_vecFinalOpList.end(); itorOp++)
        {
            RELEASE_POINTER((*itorOp).pInstance);
        }
        itorModule = m_vecFinalOpDllHandle.begin();
        for (; itorModule != m_vecFinalOpDllHandle.end(); itorModule++)
        {
            ::FreeLibrary(*itorModule);
        }

        RELEASE_POINTER(m_pDevice);
        if (nullptr != m_hDvDllHandle)
        {
            ::FreeLibrary(m_hDvDllHandle);
        }
    }


}

