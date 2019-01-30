#include <setupapi.h>


// 為函式指標建立相符的Typedef,防止逾越堆疊的記憶體配置和造成存取違規

typedef HDEVINFO (WINAPI *MYPROC1)(LPGUID, PCHAR, HWND, DWORD);
typedef BOOL (WINAPI *MYPROC2)(HDEVINFO, DWORD, PSP_DEVINFO_DATA);
typedef BOOL (WINAPI *MYPROC3)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, PDWORD, PBYTE, DWORD, PDWORD);
typedef BOOL (WINAPI *MYPROC4)(DI_FUNCTION, HDEVINFO, PSP_DEVINFO_DATA);
typedef BOOL (WINAPI *MYPROC5)(HDEVINFO);

//===================================================================

BOOL FindAllDeviceExist(CHAR  *USBDeviceID,char *VID,char *PID,char *REV);
int FindAllDeviceNumber();

BOOL FindDevice(
	int     iOSVer, 
	LPSTR   lpVendorID,
	MYPROC1	Proc_SetupDiGetClassDevs,
	MYPROC2	Proc_SetupDiEnumDeviceInfo,
	MYPROC3	Proc_SetupDiGetDeviceRegistryProperty,
	MYPROC4	Proc_SetupDiCallClassInstaller,
	MYPROC5	Proc_SetupDiDestroyDeviceInfoList,char *VID,char *PID,char *REV);

int FindDeviceNumber(
	int     iOSVer, 
	MYPROC1	Proc_SetupDiGetClassDevs,
	MYPROC2	Proc_SetupDiEnumDeviceInfo,
	MYPROC3	Proc_SetupDiGetDeviceRegistryProperty,
	MYPROC4	Proc_SetupDiCallClassInstaller,
	MYPROC5	Proc_SetupDiDestroyDeviceInfoList);


#define OS_WIN95	0
#define OS_WINNT	1
#define OS_WIN98	2
#define OS_WIN2K	3
#define OS_WINXP	4
#define OS_VISTA	5