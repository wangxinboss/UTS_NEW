#include "stdafx.h"
//#include <iostream.h>
#include "FindDevicePidVid.h"


BOOL FindAllDeviceExist(CHAR  *USBDeviceID,char *VID,char *PID,char *REV)
{
// Get a handle to the DLL module.
	HINSTANCE	hinstLib;
	MYPROC1	Proc_SetupDiGetClassDevs;
	MYPROC2	Proc_SetupDiEnumDeviceInfo;
	MYPROC3	Proc_SetupDiGetDeviceRegistryProperty;
	MYPROC4	Proc_SetupDiCallClassInstaller;
	MYPROC5	Proc_SetupDiDestroyDeviceInfoList;


    //add by derek,check vidpid,for xp
	BOOL Check_VidPid=FALSE;
    
	int i=0;    
    hinstLib = LoadLibrary(_T("setupapi"));

	// If the function address is valid, call the function.
	Proc_SetupDiGetClassDevs              = (MYPROC1) GetProcAddress(hinstLib, "SetupDiGetClassDevsW");
	Proc_SetupDiEnumDeviceInfo            = (MYPROC2) GetProcAddress(hinstLib, "SetupDiEnumDeviceInfo");
	Proc_SetupDiGetDeviceRegistryProperty = (MYPROC3) GetProcAddress(hinstLib, "SetupDiGetDeviceRegistryPropertyA");
	Proc_SetupDiCallClassInstaller        = (MYPROC4) GetProcAddress(hinstLib, "SetupDiCallClassInstaller");
	Proc_SetupDiDestroyDeviceInfoList     = (MYPROC5) GetProcAddress(hinstLib, "SetupDiDestroyDeviceInfoList");

	// If the function address is valid, call the function.
	if (!((Proc_SetupDiGetClassDevs != NULL) &&
		(Proc_SetupDiEnumDeviceInfo != NULL) &&
		(Proc_SetupDiGetDeviceRegistryProperty != NULL) &&
		(Proc_SetupDiCallClassInstaller != NULL) &&
		(Proc_SetupDiDestroyDeviceInfoList != NULL)))
		return FALSE;

	
	//for XP

    Check_VidPid=FindDevice(OS_WINXP, USBDeviceID,Proc_SetupDiGetClassDevs,Proc_SetupDiEnumDeviceInfo,Proc_SetupDiGetDeviceRegistryProperty,Proc_SetupDiCallClassInstaller,Proc_SetupDiDestroyDeviceInfoList,VID,PID,REV);
    
	if((Check_VidPid)) 
	{
	    FreeLibrary(hinstLib);
		return TRUE;
	}
	else 
	{
		FreeLibrary(hinstLib);
		return FALSE;
    }
}



BOOL FindDevice(
	int     iOSVer, 
	LPSTR   lpVendorID,
	MYPROC1	Proc_SetupDiGetClassDevs,
	MYPROC2	Proc_SetupDiEnumDeviceInfo,
	MYPROC3	Proc_SetupDiGetDeviceRegistryProperty,
	MYPROC4	Proc_SetupDiCallClassInstaller,
	MYPROC5	Proc_SetupDiDestroyDeviceInfoList,
	char *VID,char *PID,char *REV)
{

	    HDEVINFO	DeviceInfoSet = NULL;
    //	CHAR		szUSBDeviceID[MAX_PATH];
		char		szUSBDeviceID[MAX_PATH];
	    BOOL		fFreeResult = FALSE;
    	DWORD		i;
		int check=-1;

		SP_DEVINFO_DATA	DeviceInfoData;

		LPSTR  cstemp = lpVendorID;
		LPSTR  DEVString;
        int ifound = 0;
	    int usbdevicepos = -1;
		
		USES_CONVERSION;

        sprintf_s(szUSBDeviceID, "USB\\%s", lpVendorID);
        
		DeviceInfoSet = Proc_SetupDiGetClassDevs(NULL, 0, 0, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	    if (DeviceInfoSet == INVALID_HANDLE_VALUE)	return FALSE;
	    
		//
	    //  Enumerate through all Devices.
	    //
        DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for (i = 0; Proc_SetupDiEnumDeviceInfo(DeviceInfoSet, i, &DeviceInfoData); i++)
		{
	        DWORD	DataT;
	        LPSTR	buffer = NULL;

            DWORD	buffersize = 0;

		    while (!Proc_SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData,SPDRP_HARDWAREID, &DataT, (PBYTE) buffer, buffersize, &buffersize))
			{
			   if (GetLastError() == ERROR_INVALID_DATA)
			   {
			     break;
			   }
			   else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			   {
			       if (buffer)	
				   { 
				     LocalFree(buffer); 
				     buffer=NULL;
				   }
				   buffer = (LPSTR) LocalAlloc(LPTR, buffersize);
			   }
			   
			   else
			   {
        	       if (buffer)	{LocalFree(buffer); buffer=0;}
				     //goto cleanup_DeviceInfo1;
			   }
			}

	       	if (GetLastError() == ERROR_INVALID_DATA)
		    continue;
            
			DEVString = (LPSTR)szUSBDeviceID;

		    //clear string "&Rev_xxxx" ,issue ,讓同一種類的camera可以使用,2008/09/22
			
			if((buffer[0]=='U')&&(buffer[1]=='S')&&(buffer[2]=='B'))
			{
				int l=35;
				while(l>=31)
				{
					l--;
					buffer[l]='\0';

				}
				
				if(strlen(buffer)!=0)
				check= _memicmp(DEVString,buffer,11);	

				if( check == 0 ) 
				{
					VID[0] = buffer[ 8];
					VID[1] = buffer[ 9];
					VID[2] = buffer[10];
					VID[3] = buffer[11];

					PID[0] = buffer[17];
					PID[1] = buffer[18];
					PID[2] = buffer[19];
					PID[3] = buffer[20];

					REV[0] = buffer[26];
					REV[1] = buffer[27];
					REV[2] = buffer[28];
					REV[3] = buffer[29];  

					if(REV[3]>= 0x3A && REV[3]<= 0x40 ) //win10 REV Error
					{
						REV[3] += 0x07;
					}

				}


				if(strlen(buffer)!=0)
				check= _memicmp(DEVString,buffer,strlen(buffer));	



				if (buffer)	{ LocalFree(buffer); buffer=0; }
				if( check == 0 ) 
				{
					return TRUE;
				}
				else 
				{
					continue;
				}
		}
	}
	return FALSE;		

}


int  FindAllDeviceNumber()
{
// Get a handle to the DLL module.
	HINSTANCE	hinstLib;
	MYPROC1	Proc_SetupDiGetClassDevs;
	MYPROC2	Proc_SetupDiEnumDeviceInfo;
	MYPROC3	Proc_SetupDiGetDeviceRegistryProperty;
	MYPROC4	Proc_SetupDiCallClassInstaller;
	MYPROC5	Proc_SetupDiDestroyDeviceInfoList;


    //add by derek,check vidpid,for xp
	BOOL Check_VidPid=FALSE;
    
	int i=0;
    /*
	while( i < BS_MaxUSBdeiviceNum)
	{
		m_USBdeiviceCheckList[i].used    = 0;
		m_USBdeiviceCheckList[i].ifexist = 0;
		m_USBdeiviceCheckList[i].USBName = "";
		i++;
	}
	*/
	
    
    hinstLib = LoadLibrary(_T("setupapi"));

	// If the function address is valid, call the function.
	Proc_SetupDiGetClassDevs              = (MYPROC1) GetProcAddress(hinstLib, "SetupDiGetClassDevsW");
	Proc_SetupDiEnumDeviceInfo            = (MYPROC2) GetProcAddress(hinstLib, "SetupDiEnumDeviceInfo");
	Proc_SetupDiGetDeviceRegistryProperty = (MYPROC3) GetProcAddress(hinstLib, "SetupDiGetDeviceRegistryPropertyA");
	Proc_SetupDiCallClassInstaller        = (MYPROC4) GetProcAddress(hinstLib, "SetupDiCallClassInstaller");
	Proc_SetupDiDestroyDeviceInfoList     = (MYPROC5) GetProcAddress(hinstLib, "SetupDiDestroyDeviceInfoList");

	// If the function address is valid, call the function.
	if (!((Proc_SetupDiGetClassDevs != NULL) &&
		(Proc_SetupDiEnumDeviceInfo != NULL) &&
		(Proc_SetupDiGetDeviceRegistryProperty != NULL) &&
		(Proc_SetupDiCallClassInstaller != NULL) &&
		(Proc_SetupDiDestroyDeviceInfoList != NULL)))
		return FALSE;

	int number=FindDeviceNumber(OS_WINXP,Proc_SetupDiGetClassDevs,Proc_SetupDiEnumDeviceInfo,Proc_SetupDiGetDeviceRegistryProperty,Proc_SetupDiCallClassInstaller,Proc_SetupDiDestroyDeviceInfoList);
    FreeLibrary(hinstLib);
	return number;
}

int FindDeviceNumber(
	int     iOSVer, 
	MYPROC1	Proc_SetupDiGetClassDevs,
	MYPROC2	Proc_SetupDiEnumDeviceInfo,
	MYPROC3	Proc_SetupDiGetDeviceRegistryProperty,
	MYPROC4	Proc_SetupDiCallClassInstaller,
	MYPROC5	Proc_SetupDiDestroyDeviceInfoList)
{

	    HDEVINFO	DeviceInfoSet = NULL;
	    BOOL		fFreeResult = FALSE;
    	DWORD		i;


		SP_DEVINFO_DATA	DeviceInfoData;

        int ifound = 0;
	    int usbdevicepos = -1;
    
        
       
		DeviceInfoSet = Proc_SetupDiGetClassDevs(NULL, 0, 0, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	    if (DeviceInfoSet == INVALID_HANDLE_VALUE)	return 0;
	    
		//
	    //  Enumerate through all Devices.
	    //
        DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for (i = 0; Proc_SetupDiEnumDeviceInfo(DeviceInfoSet, i, &DeviceInfoData); i++)
		{
	       
		
		}

	return i;		

}