#include "StdAfx.h"
#include "DBLocalConfig.h"
#include "DBLocalConfigSql.h"
#include "ddm.h"
using namespace DDM;

namespace UTS
{
    #define LOCALDB   "UTS_LOCAL_DB"

    DBLocalConfig::DBLocalConfig(void)
    {
        db = GetDB(LOCALDB, DBTYPE_SQLITE);
        assert(db);
    }

    DBLocalConfig::~DBLocalConfig(void)
    {
        CloseDB(LOCALDB);
        db = NULL;
    }

    // 初始化
    BOOL DBLocalConfig::Initialize()
    {
        // 如果已经打开，先关闭
        if (db != nullptr)
        {
            if (db->IsConnected())
            {
                CloseDB(LOCALDB);
            }
        }

        BOOL bDBExist = FALSE;
        UTS::OSUtil::CreateMultipleDirectory(uts.info.strConfigPath);
        // Get DB file name
        CString strDBFile = uts.info.strConfigPath + _T("UTS.db");
        //CString strDBFile = _T("..\\Config\\UTS.db");
        //AfxMessageBox(strDBFile);
        if (OSUtil::IsFileExist(strDBFile))
        {
            bDBExist = TRUE;
        }
        // connect to DB
        USES_CONVERSION;
        int ret = db->connect(NULL, NULL, NULL, T2A(strDBFile));
        if (ret < 0)
        {
            return FALSE;
        }
        // 创建DB中初始内容
        if (!bDBExist)
        {
            if (db->IsConnected())
            {
                CString strSql;
                strSql.Format(sqlCreateAllTable);
                USES_CONVERSION;
                if (db->execute(T2A(strSql)) < 0)
                {
                    return FALSE;
                }
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            //-------------------------------------------------------------------------
            // 此处执行的SQL，用于程序更改后，DB自动升级
            if (db->IsConnected())
            {
                CString strSql;
                strSql.Format(sqlDatabaseUpgrade);
                USES_CONVERSION;
                db->execute(T2A(strSql));
            }
        }
        return TRUE;
    }

    // Operator是否存在
    BOOL DBLocalConfig::IsOperatorExist(LPCTSTR lpOperatorFilename, BOOL &bExist, int &nOperatorId)
    {
        bExist = FALSE;
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CString strSql;
            strSql.Format(sqlfindOperator, lpOperatorFilename);
            USES_CONVERSION;
            if (db->execute_with_result(T2A(strSql), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), strSql);
                return FALSE;
            }
            assert(records.size() == 1);
            int nCount = (int)(records[0]->cols[0]->data);
            if (nCount > 0)
            {
                bExist = TRUE;
            }
            nOperatorId = (int)(records[0]->cols[1]->data);
            db->free_rows(&records);
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    // 追加Operator
    BOOL DBLocalConfig::AddOperator(LPCTSTR lpOperatorFilename)
    {
        if (db->IsConnected())
        {
            CString strSql;
            strSql.Format(sqlInsertOperator, lpOperatorFilename);
            USES_CONVERSION;
            if (db->execute(T2A(strSql)) < 0)
            {
                uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                return FALSE;
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    // 设置Init Operator
    BOOL DBLocalConfig::SetOperatorIndexInit(const vector<CString> &vecInitOperator)
    {
        if (db->IsConnected())
        {
            CString strSql = sqlDeleteOperatorIndexInit;
            USES_CONVERSION;
            if (db->execute(T2A(strSql)) < 0)
            {
                uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                return FALSE;
            }

            for (size_t i = 0; i < vecInitOperator.size(); i++)
            {
                BOOL bExist = FALSE;
                int nOperatorId = 0;
                if (IsOperatorExist(vecInitOperator[i], bExist, nOperatorId))
                {
                    if (bExist)
                    {
                        strSql.Format(sqlInsertOperatorIndexInit, i, nOperatorId);
                        USES_CONVERSION;
                        if (db->execute(T2A(strSql)) < 0)
                        {
                            uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                            return FALSE;
                        }
                    }
                }
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    // 设置Test Operator
    BOOL DBLocalConfig::SetOperatorIndexTest(const vector<CString> &vecTestOperator)
    {
        if (db->IsConnected())
        {
            CString strSql = sqlDeleteOperatorIndexTest;
            USES_CONVERSION;
            if (db->execute(T2A(strSql)) < 0)
            {
                uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                return FALSE;
            }
            
            for (size_t i = 0; i < vecTestOperator.size(); i++)
            {
                BOOL bExist = FALSE;
                int nOperatorId = 0;
                if (IsOperatorExist(vecTestOperator[i], bExist, nOperatorId))
                {
                    if (bExist)
                    {
                        strSql.Format(sqlInsertOperatorIndexTest, i, nOperatorId);
                        USES_CONVERSION;
                        if (db->execute(T2A(strSql)) < 0)
                        {
                            uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                            return FALSE;
                        }
                    }
                }
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    // 设置Final Operator
    BOOL DBLocalConfig::SetOperatorIndexFinal(const vector<CString> &vecFinalOperator)
    {
        if (db->IsConnected())
        {
            CString strSql = sqlDeleteOperatorIndexFinal;
            USES_CONVERSION;
            if (db->execute(T2A(strSql)) < 0)
            {
                uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                return FALSE;
            }

            for (size_t i = 0; i < vecFinalOperator.size(); i++)
            {
                BOOL bExist = FALSE;
                int nOperatorId = 0;
                if (IsOperatorExist(vecFinalOperator[i], bExist, nOperatorId))
                {
                    if (bExist)
                    {
                        strSql.Format(sqlInsertOperatorIndexFinal, i, nOperatorId);
                        USES_CONVERSION;
                        if (db->execute(T2A(strSql)) < 0)
                        {
                            uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                            return FALSE;
                        }
                    }
                }
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    BOOL DBLocalConfig::GetInitOperatorList(vector<CString> &vecInitOperator)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            USES_CONVERSION;
            if (db->execute_with_result(T2A(sqlGetInitOperatorList), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error."));
                return FALSE;
            }
            for (size_t i = 0; i < records.size(); i++)
            {
                DDM::DBCol *col = records[i]->cols[0];
                CStringA strTestOperator;
                memcpy(strTestOperator.GetBuffer(col->len), col->data, col->len);
                strTestOperator.ReleaseBuffer();
                USES_CONVERSION;
                vecInitOperator.push_back(A2T(strTestOperator));
            }
            db->free_rows(&records);
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    BOOL DBLocalConfig::GetTestOperatorList(vector<CString> &vecTestOperator)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            USES_CONVERSION;
            if (db->execute_with_result(T2A(sqlGetTestOperatorList), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error."));
                return FALSE;
            }
            for (size_t i = 0; i < records.size(); i++)
            {
                DDM::DBCol *col = records[i]->cols[0];
                CStringA strTestOperator;
                memcpy(strTestOperator.GetBuffer(col->len), col->data, col->len);
                strTestOperator.ReleaseBuffer();
                USES_CONVERSION;
                vecTestOperator.push_back(A2T(strTestOperator));
            }
            db->free_rows(&records);
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }
    
    BOOL DBLocalConfig::GetFinalOperatorList(vector<CString> &vecFinalOperator)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            USES_CONVERSION;
            if (db->execute_with_result(T2A(sqlGetFinalOperatorList), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error."));
                return FALSE;
            }
            for (size_t i = 0; i < records.size(); i++)
            {
                DDM::DBCol *col = records[i]->cols[0];
                CStringA strTestOperator;
                memcpy(strTestOperator.GetBuffer(col->len), col->data, col->len);
                strTestOperator.ReleaseBuffer();
                USES_CONVERSION;
                vecFinalOperator.push_back(A2T(strTestOperator));
            }
            db->free_rows(&records);
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    BOOL DBLocalConfig::GetOperatorSingleSpec(
        LPCTSTR lpOperatorFilename,
        LPCTSTR lpKey,
        CString &strValue,
        LPCTSTR lpDefault,
        LPCTSTR lpComment)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CString strSql;
            strSql.Format(sqlGetOperatorSingleSpec, lpOperatorFilename, lpKey);
            USES_CONVERSION;
            if (db->execute_with_result(T2A(strSql), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), strSql);
                return FALSE;
            }
            if (records.size() > 0)
            {
                // 存在，取得Value值
                DDM::DBCol *col2 = records[0]->cols[2];
                CStringA strValueA;
                memcpy(strValueA.GetBuffer(col2->len), col2->data, col2->len);
                strValueA.ReleaseBuffer();
                USES_CONVERSION;
                strValue = A2T(strValueA);

                // 如果Comment有变化，更新之
                DDM::DBCol *col4 = records[0]->cols[4];
                int nOperatorId = (int)(col4->data);
                DDM::DBCol *col3 = records[0]->cols[3];
                CStringA strCommentA;
                memcpy(strCommentA.GetBuffer(col3->len), col3->data, col3->len);
                strCommentA.ReleaseBuffer();
                db->free_rows(&records);
                CString strComment = A2T(strCommentA);
                if (strComment.Compare(lpComment) != 0)
                {
                    uts.log.Warning(GetTimesString(_T("="), 80));
                    uts.log.Warning(_T("Find test item %s->%s comment diff:"), lpOperatorFilename, lpKey);
                    uts.log.Warning(_T("  Old comment:"));
                    uts.log.Warning(_T("    [%s]"), strComment);
                    uts.log.Warning(_T("  New comment:"));
                    uts.log.Warning(_T("    [%s]"), lpComment);

                    strSql.Format(sqlUpdateSingleComment, lpComment, nOperatorId, lpKey);
                    USES_CONVERSION;
                    if (db->execute(T2A(strSql)) < 0)
                    {
                        uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                        return FALSE;
                    }
                    uts.log.Warning(_T("New comment has been updated into DB."));
                    uts.log.Warning(GetTimesString(_T("="), 80));
                }
            }
            else
            {
                // 不存在，取得Default值作为Value
                strValue = lpDefault;
                db->free_rows(&records);
                // 把Default值插入DB
                BOOL bExist = FALSE;
                int nOperatorId = 0;
                if (IsOperatorExist(lpOperatorFilename, bExist, nOperatorId))
                {
                    if (bExist)
                    {
                        strSql.Format(sqlInsertOperatorSpec, nOperatorId, lpKey, lpDefault, lpComment);
                        USES_CONVERSION;
                        if (db->execute(T2A(strSql)) < 0)
                        {
                            uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                            return FALSE;
                        }
                    }
                    else
                    {
                        uts.log.Error(_T("Operator %s is not exist."), lpOperatorFilename);
                        return FALSE;
                    }
                }
                else
                {
                    uts.log.Error(_T("IsOperatorExist error."));
                    return FALSE;
                }
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return TRUE;
    }

    BOOL DBLocalConfig::GetOperatorSingleSpec(
        LPCTSTR lpOperatorFilename,
        LPCTSTR lpKey,
        double &dValue,
        double dDefault,
        LPCTSTR lpComment)
    {
        CString strValue;
        CString strDefault;
        strDefault.Format(_T("%f"), dDefault);
        BOOL bRet = GetOperatorSingleSpec(
            lpOperatorFilename,
            lpKey,
            strValue,
            strDefault,
            lpComment);
        dValue = _ttof(strValue);
        return bRet;
    }

    BOOL DBLocalConfig::GetOperatorSingleSpec(
        LPCTSTR lpOperatorFilename,
        LPCTSTR lpKey,
        int &nValue,
        int nDefault,
        LPCTSTR lpComment)
    {
        CString strValue;
        CString strDefault;
        strDefault.Format(_T("%d"), nDefault);
        BOOL bRet = GetOperatorSingleSpec(
            lpOperatorFilename,
            lpKey,
            strValue,
            strDefault,
            lpComment);
        nValue = _ttoi(strValue);
        return bRet;
    }

    BOOL DBLocalConfig::GetOperatorSpec(int nOperatorId, OPERATOR_SPEC &stOperatorSpec)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CString strSql;
            strSql.Format(sqlGetOperatorSpec, nOperatorId);
            USES_CONVERSION;
            if (db->execute_with_result(T2A(strSql), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), strSql);
                return FALSE;
            }
            for (size_t i = 0; i < records.size(); i++)
            {
                SINGLE_ITEM stSingleSpec = {0};
                CStringA strTempValue[3];
                USES_CONVERSION;

                DDM::DBCol *colKey = records[i]->cols[1];
                memcpy(strTempValue[0].GetBuffer(colKey->len), colKey->data, colKey->len);
                strTempValue[0].ReleaseBuffer();
                stSingleSpec.strKey = A2T(strTempValue[0]);

                DDM::DBCol *colValue = records[i]->cols[2];
                memcpy(strTempValue[1].GetBuffer(colValue->len), colValue->data, colValue->len);
                strTempValue[1].ReleaseBuffer();
                stSingleSpec.strValue = A2T(strTempValue[1]);

                DDM::DBCol *colComment = records[i]->cols[3];
                memcpy(strTempValue[2].GetBuffer(colComment->len), colComment->data, colComment->len);
                strTempValue[2].ReleaseBuffer();
                stSingleSpec.strComment = A2T(strTempValue[2]);

                stOperatorSpec.vecSingleSpec.push_back(stSingleSpec);
            }
            db->free_rows(&records);
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return FALSE;
    }

    BOOL DBLocalConfig::GetAllOperatorSpec(vector<OPERATOR_SPEC> &vecAllSpec)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            //------------------------------------------------------------------------------
            // 取得所有有规格的Operator的id和filename
            USES_CONVERSION;
            if (db->execute_with_result(T2A(sqlGetOperatorSpecOperatorIdandName), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), T2A(sqlGetOperatorSpecOperatorIdandName));
                return FALSE;
            }
            for (size_t i = 0; i < records.size(); i++)
            {
                DDM::DBCol *col0 = records[i]->cols[0];
                OPERATOR_SPEC opSpec = {0};
                opSpec.nOperatorId = (int)(col0->data);
                DDM::DBCol *col1 = records[i]->cols[1];
                CStringA strOperatorFilename;
                memcpy(strOperatorFilename.GetBuffer(col1->len), col1->data, col1->len);
                strOperatorFilename.ReleaseBuffer();
                opSpec.strOperatorFileName = A2T(strOperatorFilename);
                vecAllSpec.push_back(opSpec);
            }
            db->free_rows(&records);
            //------------------------------------------------------------------------------
            // 取得所有有规格的Operator的Spec
            for (size_t i = 0; i < vecAllSpec.size(); i++)
            {
                GetOperatorSpec(vecAllSpec[i].nOperatorId, vecAllSpec[i]);
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return TRUE;
    }

    BOOL DBLocalConfig::SetAllOperatorSpec(const vector<OPERATOR_SPEC> &vecAllSpec)
    {
        BOOL bRet = TRUE;
        CString strSql;

        if (!db->IsConnected())
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        //-------------------------------------------------------------------------
        // fix 改善 #10: 修改配置时时间过长
        if (db->execute("begin transaction;") < 0)
        {
            uts.log.Error(_T("DB begin transaction error."));
            return FALSE;
        }
        //-------------------------------------------------------------------------
        for (size_t i = 0; i < vecAllSpec.size(); i++)
        {
            for (size_t j = 0; j < vecAllSpec[i].vecSingleSpec.size(); j++)
            {
                CString strValue = vecAllSpec[i].vecSingleSpec[j].strValue;
                int nOperatorId = vecAllSpec[i].nOperatorId;
                CString strKey = vecAllSpec[i].vecSingleSpec[j].strKey;

                strSql.Format(sqlUpdateSingleSpec, strValue, nOperatorId, strKey);
                USES_CONVERSION;
                if (db->execute(T2A(strSql)) < 0)
                {
                    uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                    bRet = FALSE;
                    goto end;
                }
            }
        }
end:
        //-------------------------------------------------------------------------
        // fix 改善 #10: 修改配置时时间过长
        if (bRet)
        {
            if (db->execute("commit;") < 0)
            {
                uts.log.Error(_T("[%s]DB commit error."), UTS_FN);
                bRet = FALSE;
            }
        }
        if (!bRet)
        {
            uts.log.Warning(_T("[%s]DB rollback."), UTS_FN);
            if (db->execute("rollback;") < 0)
            {
                uts.log.Error(_T("[%s]DB rollback error."), UTS_FN);
                bRet = FALSE;
            }
        }
        //-------------------------------------------------------------------------
        return bRet;
    }

    BOOL DBLocalConfig::GetProjectSingleSetting(
        LPCTSTR lpKey,
        CString &strValue,
        LPCTSTR lpDefault,
        LPCTSTR lpComment)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CString strSql;
            strSql.Format(sqlGetProjectSingleSetting, lpKey);
            USES_CONVERSION;
            if (db->execute_with_result(T2A(strSql), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), strSql);
                return FALSE;
            }
            if (records.size() > 0)
            {
                // 存在，取得Value值
                DDM::DBCol *col = records[0]->cols[1];
                CStringA strTempValue;
                memcpy(strTempValue.GetBuffer(col->len), col->data, col->len);
                strTempValue.ReleaseBuffer();
                USES_CONVERSION;
                strValue = A2T(strTempValue);
                db->free_rows(&records);
            }
            else
            {
                // 不存在，取得Default值作为Value
                strValue = lpDefault;
                db->free_rows(&records);
                // 把Default值插入DB
                strSql.Format(sqlInsertProjectSetting, lpKey, lpDefault, lpComment);
                USES_CONVERSION;
                if (db->execute(T2A(strSql)) < 0)
                {
                    uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                    return FALSE;
                }
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return TRUE;
    }

    BOOL DBLocalConfig::GetProjectSingleSetting(
        LPCTSTR lpKey,
        double &dValue,
        double dDefault,
        LPCTSTR lpComment)
    {
        CString strValue;
        CString strDefault;
        strDefault.Format(_T("%f"), dDefault);
        BOOL bRet = GetProjectSingleSetting(
            lpKey,
            strValue,
            strDefault,
            lpComment);
        dValue = _ttof(strValue);
        return bRet;
    }

    BOOL DBLocalConfig::GetProjectSingleSetting(
        LPCTSTR lpKey,
        int &nValue,
        int nDefault,
        LPCTSTR lpComment)
    {
        CString strValue;
        CString strDefault;
        strDefault.Format(_T("%d"), nDefault);
        BOOL bRet = GetProjectSingleSetting(
            lpKey,
            strValue,
            strDefault,
            lpComment);
        nValue = _ttoi(strValue);
        return bRet;
    }

    BOOL DBLocalConfig::GetProjectAllSetting(vector<SINGLE_ITEM> &vecAllSetting)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            USES_CONVERSION;
            if (db->execute_with_result(T2A(sqlGetProjectAllSetting), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), T2A(sqlGetProjectAllSetting));
                return FALSE;
            }
            for (size_t i = 0; i < records.size(); i++)
            {
                USES_CONVERSION;
                SINGLE_ITEM si = {0};
                CStringA strOperatorFilename[3];

                DDM::DBCol *col = records[i]->cols[0];
                memcpy(strOperatorFilename[0].GetBuffer(col->len), col->data, col->len);
                strOperatorFilename[0].ReleaseBuffer();

                col = records[i]->cols[1];
                memcpy(strOperatorFilename[1].GetBuffer(col->len), col->data, col->len);
                strOperatorFilename[1].ReleaseBuffer();

                col = records[i]->cols[2];
                memcpy(strOperatorFilename[2].GetBuffer(col->len), col->data, col->len);
                strOperatorFilename[2].ReleaseBuffer();

                si.strKey = A2T(strOperatorFilename[0]);
                si.strValue = A2T(strOperatorFilename[1]);
                si.strComment = A2T(strOperatorFilename[2]);

                vecAllSetting.push_back(si);
            }
            db->free_rows(&records);
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return TRUE;
    }

    BOOL DBLocalConfig::SetProjectAllSetting(const vector<SINGLE_ITEM> &vecAllSetting)
    {
        BOOL bRet = TRUE;
        CString strSql;

        if (!db->IsConnected())
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        //-------------------------------------------------------------------------
        // fix 改善 #10: 修改配置时时间过长
        if (db->execute("begin transaction;") < 0)
        {
            uts.log.Error(_T("DB begin transaction error."));
            return FALSE;
        }
        //-------------------------------------------------------------------------
        for (size_t i = 0; i < vecAllSetting.size(); i++)
        {
            strSql.Format(sqlUpdateProjectSingleSetting, vecAllSetting[i].strValue, vecAllSetting[i].strKey);
            USES_CONVERSION;
            if (db->execute(T2A(strSql)) < 0)
            {
                uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                bRet = FALSE;
                goto end;
            }
        }
end:
        //-------------------------------------------------------------------------
        // fix 改善 #10: 修改配置时时间过长
        if (bRet)
        {
            if (db->execute("commit;") < 0)
            {
                uts.log.Error(_T("[%s]DB commit error."), UTS_FN);
                bRet = FALSE;
            }
        }
        if (!bRet)
        {
            uts.log.Warning(_T("[%s]DB rollback."), UTS_FN);
            if (db->execute("rollback;") < 0)
            {
                uts.log.Error(_T("[%s]DB rollback error."), UTS_FN);
                bRet = FALSE;
            }
        }
        //-------------------------------------------------------------------------
        return bRet;
    }

    // 取得ErrorCode的所有设定
    BOOL DBLocalConfig::GetErrorcodeAllSetting(vector<OPERATOR_ERRORCODE> &vecAllSetting)
    {
        if (db->IsConnected())
        {
            for (size_t i = 0; i < vecAllSetting.size(); i++)
            {
                // 检查Operator是否存在
                BOOL bExist = FALSE;
                int nOperatorId = 0;
                if (IsOperatorExist(vecAllSetting[i].strOperatorFileName, bExist, vecAllSetting[i].nOperatorId))
                {
                    if (bExist)
                    {
                        for (size_t j = 0; j < vecAllSetting[i].vecSingleErrorcodeInfo.size(); j++)
                        {
                            // 检查Errorcode是否存在
                            vector<DDM::DBRow*> records;
                            CString strSql;
                            strSql.Format(
                                sqlGetSingleErrorcodeSetting,
                                vecAllSetting[i].nOperatorId,
                                vecAllSetting[i].vecSingleErrorcodeInfo[j].nReturnValue);
                            USES_CONVERSION;
                            if (db->execute_with_result(T2A(strSql), &records) < 0)
                            {
                                db->free_rows(&records);
                                uts.log.Error(_T("db->execute_with_result error. sql = %s"), T2A(strSql));
                                return FALSE;
                            }
                            if (records.size() > 0)
                            {
                                // 存在则取得
                                DDM::DBCol *col = records[0]->cols[0];
                                CStringA strErrorcode;
                                memcpy(strErrorcode.GetBuffer(col->len), col->data, col->len);
                                strErrorcode.ReleaseBuffer();
                                vecAllSetting[i].vecSingleErrorcodeInfo[j].strErrorCode = A2T(strErrorcode);
                                db->free_rows(&records);
                            }
                            else
                            {
                                db->free_rows(&records);
                                // 不存在则插入
                                CString strSql;
                                strSql.Format(
                                    sqlInsertSingleErrorcodeSetting,
                                    vecAllSetting[i].nOperatorId,
                                    vecAllSetting[i].vecSingleErrorcodeInfo[j].nReturnValue,
                                    vecAllSetting[i].vecSingleErrorcodeInfo[j].strErrorCode);
                                USES_CONVERSION;
                                if (db->execute(T2A(strSql)) < 0)
                                {
                                    uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                                    return FALSE;
                                }
                            }
                        }
                    }
                    else
                    {
                        uts.log.Error(_T("Operator %s is not exist."), vecAllSetting[i].strOperatorFileName);
                        return FALSE;
                    }
                }
                else
                {
                    uts.log.Error(_T("IsOperatorExist error."));
                    return FALSE;
                }
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return TRUE;
    }

    // 设置ErrorCode的所有设定
    BOOL DBLocalConfig::SetErrorcodeAllSetting(const vector<OPERATOR_ERRORCODE> &vecAllSetting)
    {
        BOOL bRet = TRUE;
        if (!db->IsConnected())
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        //-------------------------------------------------------------------------
        // fix 改善 #10: 修改配置时时间过长
        if (db->execute("begin transaction;") < 0)
        {
            uts.log.Error(_T("DB begin transaction error."));
            return FALSE;
        }
        //-------------------------------------------------------------------------
        for (size_t i = 0; i < vecAllSetting.size(); i++)
        {
            for (size_t j = 0; j < vecAllSetting[i].vecSingleErrorcodeInfo.size(); j++)
            {
                CString strSql;
                strSql.Format(
                    sqlUpdateSingleErrorcodeSetting,
                    vecAllSetting[i].vecSingleErrorcodeInfo[j].strErrorCode,
                    vecAllSetting[i].nOperatorId,
                    vecAllSetting[i].vecSingleErrorcodeInfo[j].nReturnValue);
                USES_CONVERSION;
                if (db->execute(T2A(strSql)) < 0)
                {
                    uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                    bRet = FALSE;
                    goto end;
                }
            }
        }
end:
        //-------------------------------------------------------------------------
        // fix 改善 #10: 修改配置时时间过长
        if (bRet)
        {
            if (db->execute("commit;") < 0)
            {
                uts.log.Error(_T("[%s]DB commit error."), UTS_FN);
                bRet = FALSE;
            }
        }
        if (!bRet)
        {
            uts.log.Warning(_T("[%s]DB rollback."), UTS_FN);
            if (db->execute("rollback;") < 0)
            {
                uts.log.Error(_T("[%s]DB rollback error."), UTS_FN);
                bRet = FALSE;
            }
        }
        //-------------------------------------------------------------------------
        return bRet;
    }

    // Device是否存在
    BOOL DBLocalConfig::IsDeviceExist(LPCTSTR lpDeviceFilename, BOOL &bExist, int &nDeviceId)
    {
        bExist = FALSE;
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CString strSql;
            strSql.Format(sqlfindDevice, lpDeviceFilename);
            USES_CONVERSION;
            if (db->execute_with_result(T2A(strSql), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), strSql);
                return FALSE;
            }
            assert(records.size() == 1);
            int nCount = (int)(records[0]->cols[0]->data);
            if (nCount > 0)
            {
                bExist = TRUE;
            }
            nDeviceId = (int)(records[0]->cols[1]->data);
            db->free_rows(&records);
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    // 追加Device
    BOOL DBLocalConfig::AddDevice(LPCTSTR lpDeviceFilename)
    {
        if (db->IsConnected())
        {
            CString strSql;
            strSql.Format(sqlInsertDevice, lpDeviceFilename);
            USES_CONVERSION;
            if (db->execute(T2A(strSql)) < 0)
            {
                uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                return FALSE;
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    BOOL DBLocalConfig::GetDeviceSingleSetting(
        LPCTSTR lpDeviceFilename,
        LPCTSTR lpKey,
        CString &strValue,
        LPCTSTR lpDefault,
        LPCTSTR lpComment)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CString strSql;
            strSql.Format(sqlGetDeviceSingleSetting, lpDeviceFilename, lpKey);
            USES_CONVERSION;
            if (db->execute_with_result(T2A(strSql), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), strSql);
                return FALSE;
            }
            if (records.size() > 0)
            {
                // 存在，取得Value值
                DDM::DBCol *col = records[0]->cols[2];
                CStringA strTempValue;
                memcpy(strTempValue.GetBuffer(col->len), col->data, col->len);
                strTempValue.ReleaseBuffer();
                USES_CONVERSION;
                strValue = A2T(strTempValue);
                db->free_rows(&records);
            }
            else
            {
                // 不存在，取得Default值作为Value
                strValue = lpDefault;
                db->free_rows(&records);
                // 把Default值插入DB
                BOOL bExist = FALSE;
                int nDeviceId = 0;
                if (IsDeviceExist(lpDeviceFilename, bExist, nDeviceId))
                {
                    if (bExist)
                    {
                        strSql.Format(sqlInsertDeviceSetting, nDeviceId, lpKey, lpDefault, lpComment);
                        USES_CONVERSION;
                        if (db->execute(T2A(strSql)) < 0)
                        {
                            uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                            return FALSE;
                        }
                    }
                    else
                    {
                        uts.log.Error(_T("Device %s is not exist."), lpDeviceFilename);
                        return FALSE;
                    }
                }
                else
                {
                    uts.log.Error(_T("IsDeviceExist error."));
                    return FALSE;
                }
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return TRUE;
    }

    BOOL DBLocalConfig::GetDeviceSingleSetting(
        LPCTSTR lpDeviceFilename,
        LPCTSTR lpKey,
        double &dValue,
        double dDefault,
        LPCTSTR lpComment)
    {
        CString strValue;
        CString strDefault;
        strDefault.Format(_T("%f"), dDefault);
        BOOL bRet = GetDeviceSingleSetting(
            lpDeviceFilename,
            lpKey,
            strValue,
            strDefault,
            lpComment);
        dValue = _ttof(strValue);
        return bRet;
    }

    BOOL DBLocalConfig::GetDeviceSingleSetting(
        LPCTSTR lpDeviceFilename,
        LPCTSTR lpKey,
        int &nValue,
        int nDefault,
        LPCTSTR lpComment)
    {
        CString strValue;
        CString strDefault;
        strDefault.Format(_T("%d"), nDefault);
        BOOL bRet = GetDeviceSingleSetting(
            lpDeviceFilename,
            lpKey,
            strValue,
            strDefault,
            lpComment);
        nValue = _ttoi(strValue);
        return bRet;
    }

    // 取得Device的文件名
    BOOL DBLocalConfig::GetDeviceFilename(int nDeviceId, CString &strDeviceFilename)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CString strSql;
            strSql.Format(sqlGetDeviceFilename, nDeviceId);
            USES_CONVERSION;
            if (db->execute_with_result(T2A(strSql), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), strSql);
                return FALSE;
            }
            if (records.size() > 0)
            {
                DDM::DBCol *col = records[0]->cols[0];
                CStringA strTempValue;
                memcpy(strTempValue.GetBuffer(col->len), col->data, col->len);
                strTempValue.ReleaseBuffer();
                USES_CONVERSION;
                strDeviceFilename = A2T(strTempValue);
                db->free_rows(&records);
            }
            else
            {
                db->free_rows(&records);
                uts.log.Error(_T("nDeviceId %d is not exist."), nDeviceId);
                return FALSE;
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return TRUE;
    }

    // 取得指定的Device设定
    BOOL DBLocalConfig::GetDeviceSetting(int nDeviceId, DEVICE_SETTING &stDeviceSetting)
    {
        stDeviceSetting.vecSingleSetting.clear();
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CString strSql;
            strSql.Format(sqlGetDeviceSettings, nDeviceId);
            USES_CONVERSION;
            if (db->execute_with_result(T2A(strSql), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), strSql);
                return FALSE;
            }
            for (size_t j = 0; j < records.size(); j++)
            {
                USES_CONVERSION;
                CStringA strTempValue[4];
                DDM::DBCol *col[4];
                for (int i = 0; i < 4; i++)
                {
                    col[i] = records[j]->cols[i];
                    memcpy(strTempValue[i].GetBuffer(col[i]->len), col[i]->data, col[i]->len);
                    strTempValue[i].ReleaseBuffer();
                }
                stDeviceSetting.nDeviceId = nDeviceId;
                stDeviceSetting.strDeviceFileName = A2T(strTempValue[0]);
                SINGLE_ITEM si = {0};
                si.strKey = A2T(strTempValue[1]);
                si.strValue = A2T(strTempValue[2]);
                si.strComment = A2T(strTempValue[3]);
                stDeviceSetting.vecSingleSetting.push_back(si);
            }
            db->free_rows(&records);
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return TRUE;
    }

    // 设置指定的Device设定
    BOOL DBLocalConfig::SetDeviceSetting(const DEVICE_SETTING &stDeviceSetting)
    {
        BOOL bRet = TRUE;
        CString strSql;

        if (!db->IsConnected())
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        //-------------------------------------------------------------------------
        // fix 改善 #10: 修改配置时时间过长
        if (db->execute("begin transaction;") < 0)
        {
            uts.log.Error(_T("DB begin transaction error."));
            return FALSE;
        }
        //-------------------------------------------------------------------------
        for (size_t i = 0; i < stDeviceSetting.vecSingleSetting.size(); i++)
        {
            CString strSql;
            strSql.Format(
                sqlUpdateDeviceSingleSetting,
                stDeviceSetting.vecSingleSetting[i].strValue,
                stDeviceSetting.nDeviceId,
                stDeviceSetting.vecSingleSetting[i].strKey);
            USES_CONVERSION;
            if (db->execute(T2A(strSql)) < 0)
            {
                uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                bRet = FALSE;
                goto end;
            }
        }
end:
        //-------------------------------------------------------------------------
        // fix 改善 #10: 修改配置时时间过长
        if (bRet)
        {
            if (db->execute("commit;") < 0)
            {
                uts.log.Error(_T("[%s]DB commit error."), UTS_FN);
                bRet = FALSE;
            }
        }
        if (!bRet)
        {
            uts.log.Warning(_T("[%s]DB rollback."), UTS_FN);
            if (db->execute("rollback;") < 0)
            {
                uts.log.Error(_T("[%s]DB rollback error."), UTS_FN);
                bRet = FALSE;
            }
        }
        //-------------------------------------------------------------------------
        return bRet;
    }
    
    // 删除所有V5UDeviceRegister
    BOOL DBLocalConfig::DeleteV5UDeviceRegister()
    {
        if (db->IsConnected())
        {
            CString strSql = sqlDeleteV5UDeviceRegister;
            USES_CONVERSION;
            if (db->execute(T2A(strSql)) < 0)
            {
                uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                return FALSE;
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    // 追加一个V5UDeviceRegister
    BOOL DBLocalConfig::AddV5UDeviceRegister(LPCTSTR lpRegisterName, LPCTSTR lpRegisterData)
    {
        if (db->IsConnected())
        {
            CString strSql;
            strSql.Format(sqlInsertV5UDeviceRegister, lpRegisterName, lpRegisterData);
            USES_CONVERSION;
            if (db->execute(T2A(strSql)) < 0)
            {
                uts.log.Error(_T("db->execute error. sql = %s"), strSql);
                return FALSE;
            }
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }

        return TRUE;
    }

    // 取得所有Register
    BOOL DBLocalConfig::GetAllRegister(vector<REGISTER_ITEM> &vecRegister)
    {
        vecRegister.clear();

        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CString strSql;
            strSql.Format(sqlGetV5UDeviceRegister);
            USES_CONVERSION;
            if (db->execute_with_result(T2A(strSql), &records) < 0)
            {
                db->free_rows(&records);
                uts.log.Error(_T("db->execute_with_result error. sql = %s"), strSql);
                return FALSE;
            }
            for (size_t j = 0; j < records.size(); j++)
            {
                USES_CONVERSION;
                CStringA strTempValue[2];
                DDM::DBCol *col[2];
                for (int i = 0; i < 2; i++)
                {
                    col[i] = records[j]->cols[i];
                    memcpy(strTempValue[i].GetBuffer(col[i]->len), col[i]->data, col[i]->len);
                    strTempValue[i].ReleaseBuffer();
                }

                REGISTER_ITEM ri = {0};
                ri.strName = A2T(strTempValue[0]);
                ri.strData = A2T(strTempValue[1]);
                vecRegister.push_back(ri);
            }
            db->free_rows(&records);
			records.clear();
        }
        else
        {
            uts.log.Error(_T("db->IsConnected == false"));
            return FALSE;
        }
        return TRUE;
    }

    // 取得用户密码
    BOOL DBLocalConfig::GetUserPassword(int nUserType, CStringA &strPassword)
    {
        if (db->IsConnected())
        {
            vector<DDM::DBRow*> records;
            CStringA strSql;
            strSql.Format(sqlGetPassword, nUserType);
            if (db->execute_with_result(strSql, &records) < 0)
            {
                db->free_rows(&records);
                return FALSE;
            }
            if (records.size() > 0)
            {
                DDM::DBCol *col = records[0]->cols[0];
                CStringA strTempValue;
                memcpy(strTempValue.GetBuffer(col->len), col->data, col->len);
                strTempValue.ReleaseBuffer();
                strPassword = strTempValue;
                db->free_rows(&records);
            }
            else
            {
                db->free_rows(&records);
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
        return TRUE;
    }

    // 设置用户密码
    BOOL DBLocalConfig::SetUserPassword(int nUserType, LPCSTR lpPassword)
    {
        if (db->IsConnected())
        {
            CStringA strSql;
            strSql.Format(sqlSetPassword, lpPassword, nUserType);
            if (db->execute(strSql) < 0)
            {
                USES_CONVERSION;
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }

        return TRUE;
    }

	// 删除一个V5UDeviceRegister
	BOOL DBLocalConfig::DeleteV5UDeviceRegister(LPCTSTR lpRegisterName)
	{
		if (db->IsConnected())
		{
			CString strSql;
			strSql.Format(sqlDeleteItemV5UDeviceRegister, lpRegisterName);
			USES_CONVERSION;
			if (db->execute(T2A(strSql)) < 0)
			{
				uts.log.Error(_T("db->execute error. sql = %s"), strSql);
				return FALSE;
			}
		}
		else
		{
			uts.log.Error(_T("db->IsConnected == false"));
			return FALSE;
		}
		return TRUE;
	}

}
