#pragma once
#include "UTSDefine.h"
#include "Export.h"

namespace DDM { class Database; }

namespace UTS
{
    typedef struct _single_errorcode_info_
    {
        int nReturnValue;
        CString strErrorCode;
        int nReserveInt;
        CString strReserveStr;
    } SINGLE_ERRORCODE_INFO;

    typedef struct _operator_errorcode_
    {
        int nOperatorId;
        CString strOperatorFileName;
        vector<SINGLE_ERRORCODE_INFO> vecSingleErrorcodeInfo;
    } OPERATOR_ERRORCODE;

    typedef struct _single_item_
    {
        CString strKey;
        CString strValue;
        CString strComment;
    } SINGLE_ITEM;

    typedef struct _operator_spec_
    {
        int nOperatorId;
        CString strOperatorFileName;
        vector<SINGLE_ITEM> vecSingleSpec;
    } OPERATOR_SPEC;

    typedef struct _device_spec_
    {
        int nDeviceId;
        CString strDeviceFileName;
        vector<SINGLE_ITEM> vecSingleSetting;
    } DEVICE_SETTING;

    typedef struct _register_item_
    {
        CString strName;
        CString strData;
    } REGISTER_ITEM;

    class UTS_FRAMEWORK_API DBLocalConfig
    {
    public:
        DBLocalConfig(void);
        ~DBLocalConfig(void);
        // 初始化
        BOOL Initialize();
        // 判断Operator是否存在，同时取得Operator Id
        BOOL IsOperatorExist(LPCTSTR lpOperatorFilename, BOOL &bExist, int &nOperatorId);
        // 添加一个Operator到主表
        BOOL AddOperator(LPCTSTR lpOperatorFilename);
        // 设置Init Operator的顺序
        BOOL SetOperatorIndexInit(const vector<CString> &vecInitOperator);
        // 设置Test Operator的顺序
        BOOL SetOperatorIndexTest(const vector<CString> &vecTestOperator);
        // 设置Final Operator的顺序
        BOOL SetOperatorIndexFinal(const vector<CString> &vecFinalOperator);
        // 取得Init Operator的顺序
        BOOL GetInitOperatorList(vector<CString> &vecInitOperator);
        // 取得Test Operator的顺序
        BOOL GetTestOperatorList(vector<CString> &vecTestOperator);
        // 取得Final Operator的顺序
        BOOL GetFinalOperatorList(vector<CString> &vecFinalOperator);
        // 取得Operator的单个规格(重载CString, double, int)
        BOOL GetOperatorSingleSpec(LPCTSTR lpOperatorFilename, LPCTSTR lpKey, CString &strValue, LPCTSTR lpDefault, LPCTSTR lpComment);
        BOOL GetOperatorSingleSpec(LPCTSTR lpOperatorFilename, LPCTSTR lpKey, double &dValue, double dDefault, LPCTSTR lpComment);
        BOOL GetOperatorSingleSpec(LPCTSTR lpOperatorFilename, LPCTSTR lpKey, int &nValue, int nDefault, LPCTSTR lpComment);
        // 取得所有Operator的规格
        BOOL GetAllOperatorSpec(vector<OPERATOR_SPEC> &vecAllSpec);
        // 设置所有Operator的规格
        BOOL SetAllOperatorSpec(const vector<OPERATOR_SPEC> &vecAllSpec);
        // 取得单个Operator的规格
        BOOL GetOperatorSpec(int nOperatorId, OPERATOR_SPEC &stOperatorSpec);
        // 取得Project的单个设定(重载CString, double, int)
        BOOL GetProjectSingleSetting(LPCTSTR lpKey, CString &strValue, LPCTSTR lpDefault, LPCTSTR lpComment);
        BOOL GetProjectSingleSetting(LPCTSTR lpKey, double &dValue, double dDefault, LPCTSTR lpComment);
        BOOL GetProjectSingleSetting(LPCTSTR lpKey, int &nValue, int nDefault, LPCTSTR lpComment);
        // 取得Project的所有设定
        BOOL GetProjectAllSetting(vector<SINGLE_ITEM> &vecAllSetting);
        // 设置Project的所有设定
        BOOL SetProjectAllSetting(const vector<SINGLE_ITEM> &vecAllSetting);
        // 取得ErrorCode的所有设定
        BOOL GetErrorcodeAllSetting(vector<OPERATOR_ERRORCODE> &vecAllSetting);
        // 设置ErrorCode的所有设定
        BOOL SetErrorcodeAllSetting(const vector<OPERATOR_ERRORCODE> &vecAllSetting);
        // 判断Device是否存在，同时取得Device Id
        BOOL IsDeviceExist(LPCTSTR lpDeviceFilename, BOOL &bExist, int &nDeviceId);
        // 追加Device
        BOOL AddDevice(LPCTSTR lpDeviceFilename);
        // 取得Device的单个设定(重载CString, double, int)
        BOOL GetDeviceSingleSetting(LPCTSTR lpDeviceFilename, LPCTSTR lpKey, CString &strValue, LPCTSTR lpDefault, LPCTSTR lpComment);
        BOOL GetDeviceSingleSetting(LPCTSTR lpDeviceFilename, LPCTSTR lpKey, double &dValue, double dDefault, LPCTSTR lpComment);
        BOOL GetDeviceSingleSetting(LPCTSTR lpDeviceFilename, LPCTSTR lpKey, int &nValue, int nDefault, LPCTSTR lpComment);
        // 取得Device的文件名
        BOOL GetDeviceFilename(int nDeviceId, CString &strDeviceFilename);
        // 取得指定的Device设定
        BOOL GetDeviceSetting(int nDeviceId, DEVICE_SETTING &m_deviceSetting);
        // 设置指定的Device设定
        BOOL SetDeviceSetting(const DEVICE_SETTING &m_deviceSetting);
        // 删除所有V5UDeviceRegister
        BOOL DeleteV5UDeviceRegister();
		// 删除一个V5UDeviceRegister
		BOOL DeleteV5UDeviceRegister(LPCTSTR lpRegisterName);
        // 追加一个V5UDeviceRegister
        BOOL AddV5UDeviceRegister(LPCTSTR lpRegisterName, LPCTSTR lpRegisterData);
        // 取得所有Register
        BOOL GetAllRegister(vector<REGISTER_ITEM> &vecRegister);
        // 取得用户密码
        BOOL GetUserPassword(int nUserType, CStringA &strPassword);
        // 设置用户密码
        BOOL SetUserPassword(int nUserType, LPCSTR lpPassword);
    private:
        DDM::Database *db;
    };
}
