#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_SFC_CommitTestData.h"
#include "SFCInterface.h"

#pragma comment(lib, "UTTL_SfcIF.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;
    }

    ImplOperator::~ImplOperator(void)
    {
    }
        
    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        if (uts.info.strSN.IsEmpty())
        {
			*pnErrorCode = uts.errorcode.E_Pass;
            goto end;
        }

        if (uts.info.nShopFlowEn != 0)
        {
            GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC*)uts.mapUserMemory["SFC"];
            //-------------------------------------------------------------------------
            // Get Json Result string
            string sJsonResult;
            cJSON *root = cJSON_CreateObject();

            std::vector<TestItemResult>::iterator itor;
            for (itor = gmsfc->vecTestItemResult.begin();
                itor != gmsfc->vecTestItemResult.end();
                itor++)
            {
                if (!itor->sHeader.IsEmpty())
                {
                    // output csv
                    OutputCSV(*itor);

                    // output json
                    if (!itor->sSFC_Filter.IsEmpty())
                    {
                        AddJsonItem(root, *itor);
                    }
                }
            }

            sJsonResult = cJSON_PrintUnformatted(root);
            cJSON_Delete(root);

            //-------------------------------------------------------------------------
            // Commit Test Data
            USES_CONVERSION;
            TEST_DATA test_data = {0};
            test_data.sn = uts.info.strSN;
            test_data.testResult = (gmsfc->bOverAllResult ? 0 : 1);
            test_data.testtime = T2W(gmsfc->sSFC_ServerTime.GetBuffer());
            test_data.errorCode= T2W(gmsfc->sSFC_ErrorCode.GetBuffer());
            test_data.errorDesc= T2W(gmsfc->sSFC_ErrorDesc.GetBuffer());
            test_data.testItems= A2W(sJsonResult.c_str());
            if (!SFC_CommitTestData(&test_data))
            {
                wchar_t error[0x100];
                SFC_GetLastErrorMsg(error, 0x100);
                uts.log.Error(_T("SFC_CommitTestData Error: %s"), error);
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }
            else
            {
                *pnErrorCode = uts.errorcode.E_Pass;
            }
        }
        else
        {
            uts.log.Warning(_T("ShopFlow NOT enable, SFC_CommitTestData omitted."));
            *pnErrorCode = uts.errorcode.E_Pass;
        }

end:
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);
        
        return m_bResult;
    }

    void ImplOperator::OutputCSV(const TestItemResult& resultItem)
    {
        OSUtil::CreateMultipleDirectory(uts.info.strShopFlowFilePath);

        GLOBAL_MEMORY_SFC *gmsfc = (GLOBAL_MEMORY_SFC*)uts.mapUserMemory["SFC"];
        CString strFileName;
        strFileName.Format(_T("%s\\%s-%s-%s.csv"),
            OSUtil::GetNoBSPath(uts.info.strShopFlowFilePath),
            uts.info.strSN,
            gmsfc->sSFC_ServerTime,
            resultItem.sItemName);

        FILE* fp = NULL;
        errno_t etRet = _tfopen_s(&fp, strFileName, _T("a+"));
        if (0 != etRet)
        {
            uts.log.Error(_T("Open file Fail. path = %s"), strFileName);
            return;
        }

        _ftprintf_s(fp, _T("%s"), resultItem.sHeader);
        _ftprintf_s(fp, _T("%s"), resultItem.sData);
        fclose(fp);
    }

    void ImplOperator::AddJsonItem(cJSON *root, const TestItemResult& resultItem)
    {
        USES_CONVERSION;
        vector<CString> hdr, data, filter;
        SplitString(resultItem.sHeader, hdr);
        SplitString(resultItem.sData, data);
        SplitString(resultItem.sSFC_Filter, filter);
        int hdr_num = (int)hdr.size();
        int data_num = (int)data.size();
        int filter_num = (int)filter.size();

        if (hdr_num != data_num)
        {
            uts.log.Warning(_T("CSV File[%s] should be offset, please check!\n"), resultItem.sItemName);
        }
        cJSON *item;
        cJSON_AddItemToObject(root, T2A(resultItem.sItemName), item = cJSON_CreateObject()); 
        for (int j = 0; j < filter_num; j++)
        {
            for (int k = 0; k < hdr_num; k++)
            {
                if (filter[j].Compare(hdr[k]) == 0)
                {
                    cJSON_AddItemToObject(item, T2A(hdr[k]), cJSON_CreateString(T2A(data[k])));
                }
            }
        }
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
    }
    
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
