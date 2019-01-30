#pragma once

namespace UTS
{
    // DB升级脚本
    const TCHAR *sqlDatabaseUpgrade =
        _T("CREATE TABLE [UTS_OperatorIndexFinal] (   ")
        _T("  [index] INTEGER,                        ")
        _T("  [operator_id] INTEGER);                 ");

    const TCHAR* sqlCreateAllTable = 
        _T("CREATE TABLE [UTS_Operator] (             ")
        _T("  [id] INTEGER PRIMARY KEY AUTOINCREMENT, ")
        _T("  [filename] VARCHAR);                    ")
        _T("CREATE TABLE [UTS_OperatorIndexInit] (    ")
        _T("  [index] INTEGER,                        ")
        _T("  [operator_id] INTEGER);                 ")
        _T("CREATE TABLE [UTS_OperatorIndexTest] (    ")
        _T("  [index] INTEGER,                        ")
        _T("  [operator_id] INTEGER);                 ")
        _T("CREATE TABLE [UTS_OperatorIndexFinal] (   ")
        _T("  [index] INTEGER,                        ")
        _T("  [operator_id] INTEGER);                 ")
        _T("CREATE TABLE [UTS_OperatorSpec] (         ")
        _T("  [id] INTEGER PRIMARY KEY AUTOINCREMENT, ")
        _T("  [operator_id] INTEGER,                  ")
        _T("  [key] VARCHAR,                          ")
        _T("  [value] VARCHAR,                        ")
        _T("  [comment] VARCHAR);                     ")
        _T("CREATE TABLE [UTS_Project] (              ")
        _T("  [id] INTEGER PRIMARY KEY AUTOINCREMENT, ")
        _T("  [key] VARCHAR,                          ")
        _T("  [value] VARCHAR,                        ")
        _T("  [comment] VARCHAR);                     ")
        _T("CREATE TABLE [UTS_ErrorCode] (            ")
        _T("  [id] INTEGER PRIMARY KEY AUTOINCREMENT, ")
        _T("  [operator_id] INTEGER,                  ")
        _T("  [return_value] INTEGER,                 ")
        _T("  [error_code] VARCHAR,                   ")
        _T("  [reserve_int] INTEGER,                  ")
        _T("  [reserve_str] VARCHAR);                 ")
        _T("CREATE TABLE [UTS_Device] (               ")
        _T("  [id] INTEGER PRIMARY KEY AUTOINCREMENT, ")
        _T("  [filename] VARCHAR);                    ")
        _T("CREATE TABLE [UTS_DeviceSetting] (        ")
        _T("  [id] INTEGER PRIMARY KEY AUTOINCREMENT, ")
        _T("  [device_id] INTEGER,                    ")
        _T("  [key] VARCHAR,                          ")
        _T("  [value] VARCHAR,                        ")
        _T("  [comment] VARCHAR);                     ")
        _T("CREATE TABLE [UTS_V5UDeviceRegister] (    ")
        _T("  [id] INTEGER PRIMARY KEY AUTOINCREMENT, ")
        _T("  [name] VARCHAR,                         ")
        _T("  [data] VARCHAR);                        ")
        _T("CREATE TABLE [UTS_Password] (             ")
        _T("  [id] INTEGER PRIMARY KEY AUTOINCREMENT, ")
        _T("  [user_type] INTEGER,                    ")
        _T("  [password] VARCHAR);                    ")
        _T("insert into [UTS_Device](filename) values('UTDV_File.dll');         ")
        _T("insert into [UTS_Device](filename) values('UTDV_V5U.dll');          ")
        _T("insert into [UTS_Device](filename) values('UTDV_UVC.dll');          ")
        _T("insert into [UTS_Password]([user_type], [password]) values(0, '');  ")
        _T("insert into [UTS_Password]([user_type], [password]) values(1, '');  ")
        _T("insert into [UTS_Password]([user_type], [password]) values(2, '');  ")
        ;
    const TCHAR* sqlfindOperator =
        _T("select count(*), [id] from [UTS_Operator] where upper([filename]) = upper('%s')");
    // Operator Index
    const TCHAR* sqlGetInitOperatorList =
        _T("select [UTS_Operator].[filename], [UTS_OperatorIndexInit].[index]")
        _T("  from [UTS_Operator], [UTS_OperatorIndexInit]")
        _T("  where [UTS_Operator].[id] = [UTS_OperatorIndexInit].[operator_id]")
        _T("  order by [UTS_OperatorIndexInit].[index]");
    const TCHAR* sqlGetTestOperatorList =
        _T("select [UTS_Operator].[filename], [UTS_OperatorIndexTest].[index]")
        _T("  from [UTS_Operator], [UTS_OperatorIndexTest]")
        _T("  where [UTS_Operator].[id] = [UTS_OperatorIndexTest].[operator_id]")
        _T("  order by [UTS_OperatorIndexTest].[index]");
    const TCHAR* sqlGetFinalOperatorList =
        _T("select [UTS_Operator].[filename], [UTS_OperatorIndexFinal].[index]")
        _T("  from [UTS_Operator], [UTS_OperatorIndexFinal]")
        _T("  where [UTS_Operator].[id] = [UTS_OperatorIndexFinal].[operator_id]")
        _T("  order by [UTS_OperatorIndexFinal].[index]");
    const TCHAR* sqlDeleteOperatorIndexInit =
        _T("delete from [UTS_OperatorIndexInit]");
    const TCHAR* sqlDeleteOperatorIndexTest =
        _T("delete from [UTS_OperatorIndexTest]");
    const TCHAR* sqlDeleteOperatorIndexFinal =
        _T("delete from [UTS_OperatorIndexFinal]");
    const TCHAR* sqlInsertOperator =
        _T("insert into [UTS_Operator] ([filename]) values('%s')");
    const TCHAR* sqlInsertOperatorIndexInit =
        _T("insert into [UTS_OperatorIndexInit] ([index], [operator_id]) values(%d, %d)");
    const TCHAR* sqlInsertOperatorIndexTest =
        _T("insert into [UTS_OperatorIndexTest] ([index], [operator_id]) values(%d, %d)");
    const TCHAR* sqlInsertOperatorIndexFinal =
        _T("insert into [UTS_OperatorIndexFinal] ([index], [operator_id]) values(%d, %d)");
    // Operator Spec
    const TCHAR* sqlGetOperatorSingleSpec =
        _T("select [UTS_Operator].[filename], [UTS_OperatorSpec].[key], [UTS_OperatorSpec].[value], [UTS_OperatorSpec].[comment], [UTS_Operator].[id]")
        _T("  from [UTS_Operator], [UTS_OperatorSpec]")
        _T("  where [UTS_Operator].[id] = [UTS_OperatorSpec].[operator_id]")
        _T("    and upper([UTS_Operator].[filename]) = upper('%s')")
        _T("    and [UTS_OperatorSpec].[key] = '%s'");
    const TCHAR* sqlInsertOperatorSpec =
        _T("insert into [UTS_OperatorSpec]([operator_id], [key], [value], [comment]) values(%d, '%s', '%s', '%s')");
	const TCHAR* sqlGetOperatorSpecOperatorIdandName =
		_T("select distinct [UTS_OperatorSpec].[operator_id], [UTS_Operator].[filename]")
		_T("  from [UTS_OperatorSpec], [UTS_Operator],[UTS_OperatorIndexTest]")
		_T("  where [UTS_OperatorSpec].[operator_id] = [UTS_Operator].[id]")
		_T("and  [UTS_Operator].[id] = [UTS_OperatorIndexTest].[operator_id]")//fix 删除测试项时会删除测试项的spec//add by wx 2019
		_T("  order by [UTS_Operator].[filename]");  // fix 改善 #5: operator spec修改为显示时默认收起列表 // fix 改善 #5: operator spec修改为显示时默认收起列表，并按字母排序
    const TCHAR* sqlGetOperatorSpec =
        _T("select [UTS_Operator].[filename], [UTS_OperatorSpec].[key], [UTS_OperatorSpec].[value], [UTS_OperatorSpec].[comment]")
        _T("  from [UTS_Operator], [UTS_OperatorSpec]")
        _T("  where [UTS_Operator].[id] = [UTS_OperatorSpec].[operator_id]")
        _T("    and [UTS_Operator].[id] = %d")
        _T("  order by [UTS_Operator].[filename], [UTS_OperatorSpec].[key]");
    const TCHAR* sqlUpdateSingleSpec =
        _T("update [UTS_OperatorSpec] set [value] = '%s' where [operator_id] = %d and [key] = '%s'");
    const TCHAR* sqlUpdateSingleComment =
        _T("update [UTS_OperatorSpec] set [comment] = '%s' where [operator_id] = %d and [key] = '%s'");
    // Project
    const TCHAR* sqlGetProjectSingleSetting =
        _T("select [key], [value], [comment] from [UTS_Project] where [key] = '%s'");
    const TCHAR* sqlInsertProjectSetting =
        _T("insert into [UTS_Project]([key], [value], [comment]) values('%s', '%s', '%s')");
    const TCHAR* sqlGetProjectAllSetting =
        _T("select [key], [value], [comment] from [UTS_Project]");
    const TCHAR* sqlUpdateProjectSingleSetting =
        _T("update [UTS_Project] set [value] = '%s' where [key] = '%s'");
    // Errorcode
    const TCHAR* sqlInsertSingleErrorcodeSetting =
        _T("insert into [UTS_ErrorCode]([operator_id], [return_value], [error_code]) values(%d, %d, '%s')");
    const TCHAR* sqlGetSingleErrorcodeSetting =
        _T("select [error_code] from [UTS_ErrorCode] where [operator_id] = %d and [return_value] = %d");
    const TCHAR* sqlUpdateSingleErrorcodeSetting =
        _T("update [UTS_ErrorCode] set [error_code] = '%s' where [operator_id] = %d and [return_value] = %d");
    // Device Setting
    const TCHAR* sqlfindDevice =
        _T("select count(*), id from [UTS_Device] where upper([filename]) = upper('%s')");
    const TCHAR* sqlGetDeviceFilename = 
        _T("select [filename] from [UTS_Device] where [id] = %d");
    const TCHAR* sqlGetDeviceSettings = 
        _T("select [UTS_Device].[filename], [UTS_DeviceSetting].[key], [UTS_DeviceSetting].[value], [UTS_DeviceSetting].[comment]")
        _T("  from [UTS_Device], [UTS_DeviceSetting]")
        _T("  where [UTS_Device].[id] = %d")
        _T("    and [UTS_Device].[id] = [UTS_DeviceSetting].[device_id]");
    const TCHAR* sqlInsertDevice =
        _T("insert into [UTS_Device] ([filename]) values('%s')");
    const TCHAR* sqlUpdateDeviceSingleSetting =
        _T("update [UTS_DeviceSetting] set [value] = '%s' where [device_id] = %d and [key] = '%s'");
    const TCHAR* sqlGetDeviceSingleSetting =
        _T("select [UTS_Device].[filename], [UTS_DeviceSetting].[key], [UTS_DeviceSetting].[value], [UTS_DeviceSetting].[comment]")
        _T("  from [UTS_Device], [UTS_DeviceSetting]")
        _T("  where [UTS_Device].[id] = [UTS_DeviceSetting].[device_id]")
        _T("    and upper([UTS_Device].[filename]) = upper('%s')")
        _T("    and [UTS_DeviceSetting].[key] = '%s'");
    const TCHAR* sqlInsertDeviceSetting =
        _T("insert into [UTS_DeviceSetting]([device_id], [key], [value], [comment]) values(%d, '%s', '%s', '%s')");
    // UTS_V5UDeviceRegister
    const TCHAR* sqlDeleteV5UDeviceRegister = 
        _T("delete from [UTS_V5UDeviceRegister]");
	const TCHAR* sqlDeleteItemV5UDeviceRegister = 
		_T("delete from [UTS_V5UDeviceRegister] WHERE [name]='%s';");
    const TCHAR* sqlInsertV5UDeviceRegister = 
        _T("insert into [UTS_V5UDeviceRegister]([name], [data]) values('%s', '%s')");
    const TCHAR* sqlGetV5UDeviceRegister = 
        _T("select [name], [data] from [UTS_V5UDeviceRegister]");
    // UTS_Password
    const CHAR* sqlSetPassword = 
        "update [UTS_Password] set [password] = '%s' where [user_type]  = %d";
    const CHAR* sqlGetPassword = 
        "select [password] from [UTS_Password] where [user_type]  = %d";
}
