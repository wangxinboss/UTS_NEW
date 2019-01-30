#include "stdafx.h"
#include "UTSDefine.h"
#include "VCM.h"
#include "SensorDriver.h"
#include "OISControl.h"
#include "VCM_I2C.h"

#pragma comment(lib, "UTSFramework.lib")
#pragma comment(lib, "UTS_Sensor.lib")
#pragma comment(lib, "UTS_OIS.lib")

UTS::BaseDevice* g_pDevice = nullptr;

uint32_t HallMax;
uint32_t HallMin;

uint32_t HallMax_Linear;
uint32_t HallMin_Linear;

uint32_t HallPos;

uint16_t Dac_HallMax;
uint16_t Dac_HallMin;

namespace UTS
{
typedef enum _VCM_Type
	{
		VCM_SENSOR_INTERNAL,
		VCM_LC898214,
		VCM_BU64240,
		VCM_GT9762,
		VCM_ZC533,
		VCM_AK7371,
		VCM_DW9718S,
		VCM_DW9800W,
		VCM_DW9714V,
		VCM_CH9914,
		VCM_CN3927B,
		VCM_ZC524,
		VCM_FP5510E2,
		VCM_ON898123,
		VCM_FP5516WE2,
		VCM_ZC533DEFAULT,
		VCM_LC898214_MTM_ASAFV3B5,
		VCM_LC898217,
		VCM_CN3827B,
		VCM_CN3937,
		VCM_MAX,
	} VCM_Type;


    #define VCM_DISCRIPTION        \
        _T("0: Sensor internal, ") \
		_T("1: LC898214,") \
		_T("2: BU64240,") \
		_T("3: GT9762,") \
		_T("4: ZC533,") \
		_T("5: AK7371,") \
		_T("6: DW9718S,") \
		_T("7: DW9800W,") \
		_T("8: DW9714V,") \
		_T("9: VCM_CH9914,") \
		_T("10: VCM_CN3927B,") \
		_T("11: VCM_ZC524,") \
		_T("12: VCM_FP5510E2,") \
		_T("13: ON898123,") \
		_T("14: VCM_FP5516WE2,") \
		_T("15: VCM_ZC533DEFAULT,") \
		_T("16: VCM_LC898214_MTM_ASAFV3B5,") \
		_T("17: VCM_LC898217,") \
		_T("18: VCM_CN3827B,") \



    LPCTSTR DLLGetVCM_Discription(void)
    {
        return VCM_DISCRIPTION;
    }

	void DLLSetVCM_Initial(BaseDevice* pDevice,int nVCMDriverICType, const void *InitalData)
	{
		g_pDevice = pDevice;

		uts.log.Debug(_T("DLLSetVCM_Initial type:%d"),nVCMDriverICType);
		unsigned char dataBuf[2] = {0};
		unsigned char writeBuffer[256] = {0};
		int nUseSize = 0,idx = 0;
		float volt = 0.0;
		unsigned char data[64]= {0},data2[66]= {0};
		BOOL res;
		int TotalRange;

		switch (nVCMDriverICType)
		{
		case VCM_LC898214:
			Sleep(100); 	
			I2CRead(0xE4,1,0xF0, 1, dataBuf);//Slave address check
			uts.log.Debug(_T("Slave Address :0x[%x] ."), dataBuf[0]);

			HallMin = 0x4500;
			HallMax = 0xBB00;

			dataBuf[0] = 0x01;
			I2CWrite(0xE4,1,0xE0, 1, dataBuf); //Initial Data Down Load
			Sleep(10);
			for (int i= 0  ; i < 100 ; i++)
			{
				I2CRead(0xE4,1,0xE0, 1, dataBuf);
				uts.log.Debug(_T("Down Load Finish :0x[%x] ."), dataBuf[0]);
				if(dataBuf[0] == 0x00) break;
				else Sleep(2); 
			}

			DLLSetVCM_Move(pDevice, nVCMDriverICType, 0);

			break;
		case VCM_LC898217:
			Sleep(100); 	
			I2CRead(0xE4,1,0xF0, 1, dataBuf);//Slave address check
			uts.log.Debug(_T("Slave Address 0xF0=0x[%x] ."), dataBuf[0]);

			Sleep(10);
			HallMin = 0x6400;
			HallMax = 0x8B00;
		
			dataBuf[0] = 0x01;
			I2CWrite(0xE4,1,0xE0, 1, dataBuf); //Initial Data Down Load
			while(1)
			{
				I2CRead(0xE4,1,0xE0, 1, dataBuf);
				uts.log.Debug(_T("0xE0=0x[%x] ."), dataBuf[0]);
				if(dataBuf[0]==0)
					break;
				Sleep(2);
			}
			Sleep(10);
			for (int i= 0  ; i < 100 ; i++)
			{
				I2CRead(0xE4,1,0xB3, 1, dataBuf);
				uts.log.Debug(_T("0xB3=0x[%x] ."), dataBuf[0]);
				if(dataBuf[0]==0)
					break;
				Sleep(2);
			}
// 			I2CRead(0xE4,1,0x167, 2, dataBuf);
// 			uts.log.Debug(_T("0x167=0x[%02X%02X] ."), dataBuf[0],dataBuf[1]);
// 			I2CRead(0xE4,1,0x168, 2, dataBuf);
// 			uts.log.Debug(_T("0x168=0x[%02X%02X] ."), dataBuf[0],dataBuf[1]);
// 			I2CRead(0xE4,1,0x169, 2, dataBuf);
// 			uts.log.Debug(_T("0x169=0x[%02X%02X] ."), dataBuf[0],dataBuf[1]);

			DLLSetVCM_Move(pDevice, nVCMDriverICType, 0);

			break;			
		case VCM_ZC533:
			//Change default slave address
			//1. send command: VID 0x1B 0x01; // select
			Sleep(200);
			dataBuf[0] = 0x01;
			res = I2CWrite(0x18, 1, 0x1B, 1, dataBuf);
			Sleep(10);
			if(res == TRUE)
			{
				//2. send command: EID 0x00 0x00; // read
				dataBuf[0] = 0x00;
				dataBuf[1] = 0x00;
				res = I2CWrite(0xB0, 0, 0x00, 2, dataBuf);
				Sleep(10);
			
				//3. receive data: char[0…63]; // 读出64 个字节的数据，存入数组;
				res = I2CRead(0xB0,0, 0x00, 64, data);
				Sleep(10);

				//------------
				//4. change data: char[60]; // 把char[60](数组倒数第四个)的值改为对应值（0x00/0x01/0x02/0x03
				uts.log.Debug(_T("Slave mode :%d"),data[60]);
				data[60] = 0x03;

				//5. send command: EID 0xC0 0x00; // erase
				dataBuf[0] = 0xC0;
				dataBuf[1] = 0x00;
				res = I2CWrite(0xB0, 0, 0x00, 2, dataBuf);
				Sleep(10);

				//6. send command: EID 0x40 0x00; // write
				//7. send data: char[0…63]; // 写入数组中的64 个字节值。
				memcpy(data2+2,data,64);
				data2[0] = 0x40;
				data2[1] = 0x00;
				res = I2CWrite(0xB0, 0, 0x00, 66, data2);
				
				Sleep(10);
				//---------------------
				//8. restart. 
				//idx = 3; // AFVDD
				idx = 2; // AFVDD

				volt = 0;
				memcpy(writeBuffer + nUseSize, &idx, sizeof(int));
				nUseSize += sizeof(int);
				memcpy(writeBuffer + nUseSize, &volt, sizeof(float));
				nUseSize += sizeof(float);
				pDevice->WriteValue(eDeviceWriteValueType::DWVT_SET_POWER_PIN,writeBuffer, nUseSize);
				uts.log.Debug(_T("Power off AFVDD"));

				Sleep(500);
				nUseSize = 0;
				memset(writeBuffer,0,256*sizeof(unsigned char));
				volt = float(2.8);

				memcpy(writeBuffer + nUseSize, &idx, sizeof(int));
				nUseSize += sizeof(int);
				memcpy(writeBuffer + nUseSize, &volt, sizeof(float));
				nUseSize += sizeof(float);
				pDevice->WriteValue(eDeviceWriteValueType::DWVT_SET_POWER_PIN,writeBuffer, nUseSize);
				uts.log.Debug(_T("Power on AFVDD"));
				Sleep(500);
			}
			dataBuf[0] = 0x00;
			res = I2CWrite(0x1C, 1, 0x02, 1, dataBuf); //Normal Operation
			uts.log.Debug(_T("Rest Slave Address res :%d"),res);
			Sleep(50);
			I2CWrite(0x1C, 1, 0x05, 1, dataBuf); //Direct Mode Select
			Sleep(50);
			dataBuf[1] = 0x00;
			res = I2CWrite(0x1C, 1, 0x03, 2, dataBuf); //DAC : 0
			Sleep(15);

			/*FIH Mode
			I2CRead(0x1C, 1, 0x00, 1, dataBuf);
			Sleep(15);
			I2CRead(0x1C, 1, 0x01, 1, dataBuf);

			dataBuf[0] = 0x06;
			dataBuf[1] = 0x64;
			res = I2CWrite(0x1C, 1, 0x05, 2,dataBuf); 
			*/

			break;

		case VCM_AK7371:
			Sleep(20); 
			dataBuf[0] = 0x00;
			dataBuf[1] = 0x00;
			I2CWrite(0x18, 1, 0x00, 2, dataBuf); //Write Position

			dataBuf[0] = 0x00;
			I2CWrite(0x18, 1, 0x02, 1, dataBuf);   //Set Active mode
			break;

		case VCM_DW9718S:
			Sleep(20); 
			dataBuf[0] = 0x01;
			I2CWrite(0x18, 1, 0x00, 1, dataBuf);	//Power Down mode enable
			dataBuf[0] = 0x00;
			I2CWrite(0x18, 1, 0x00, 1, dataBuf);	//Power Down mode disable

			Sleep(100);	//need 100us
			dataBuf[0] = 0x39;
			I2CWrite(0x18, 1, 0x01, 1, dataBuf);	//UVLO & OCP Disable, Liner Mode SAC mode 3			

			Sleep(85); //need 85us
			dataBuf[0] = 0x65;
			I2CWrite(0x18, 1, 0x05, 1, dataBuf);	//DIV[1:0]=01, SACT[5:0]=100101
													//Res. freq=100Hz, Tvib=10.0ms	
			dataBuf[0] = 0x00;
			dataBuf[1] = 0x00;

			I2CWrite(0x18, 1, 0x02, 2, dataBuf); //Set DAC=0 Position
			break;
		case VCM_DW9800W:
			Sleep(20); 
			dataBuf[0] = 0x01;
			I2CWrite(0x18, 1, 0x02, 1, dataBuf);	//Power Down mode enable
			dataBuf[0] = 0x00;
			I2CWrite(0x18, 1, 0x02, 1, dataBuf);	//Power Down mode disable

			Sleep(100);	//need 100us
			dataBuf[0] = 0x00;
			I2CWrite(0x18, 1, 0x02, 1, dataBuf);	//Ring mode setting: Direct
			dataBuf[0] = 0x40;
			I2CWrite(0x18, 1, 0x06, 1, dataBuf);	//SAC mode 3, DIV2=0			

			dataBuf[0] = 0x65;
			I2CWrite(0x18, 1, 0x07, 1, dataBuf);	//DIV[1:0]=01, SACT[5:0]=100101
			//Res. freq=100Hz, Tvib=10.0ms	
			dataBuf[0] = 0x00;
			dataBuf[1] = 0x00;

			I2CWrite(0x18, 1, 0x03, 2, dataBuf); //Set DAC=0 Position
			break;

		case VCM_DW9714V:
			Sleep(20); 

			dataBuf[0] = 0x80;
			dataBuf[1] = 0x00;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);	//PD mode enable

			dataBuf[0] = 0x00;
			dataBuf[1] = 0x00;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);	//PD mode disable (normal operating)

			Sleep(100); 

			dataBuf[0] = 0xEC;
			dataBuf[1] = 0xA3;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);	//Ringing On

			dataBuf[0] = 0xA1;
			dataBuf[1] = 0x05;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);	//DLC = 0, MCLK = 01

			dataBuf[0] = 0xF2;
			dataBuf[1] = 0xF8;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);	//T_SRC[4:0] = b’11111, 1 Step period = 83us

			dataBuf[0] = 0xDC;
			dataBuf[1] = 0x51;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);	//Ringing Off
			Sleep(20);

			break;

		case VCM_CH9914:
			Sleep(20); 
			dataBuf[0] = 0x00;
			dataBuf[1] = 0x00;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf); //Write Position
			break;
		case VCM_CN3927B:
			Sleep(20); 
			dataBuf[0] = 0x00;
			dataBuf[1] = 0x00;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf); //Write Position
			break;
		case VCM_ZC524:
			Sleep(20); 
			dataBuf[0] = 0x00;
			dataBuf[1] = 0x00;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf); //Write Position
			break;
		case VCM_FP5510E2:
			Sleep(20); 
			dataBuf[0] = 0x00;
			dataBuf[1] = 0x00;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf); //Write Position
			break;
		case VCM_ON898123:
			GetOISDriverInstance(pDevice,uts.info.nOISType)->VCMControl(0);
			break;

		case VCM_FP5516WE2:
			Sleep(200); 

			dataBuf[0] = 0x00;
			I2CWrite(0x18, 1, 0x02, 1, dataBuf);	//power up , step mode

			break;
		case VCM_ZC533DEFAULT:
			dataBuf[0] = 0x00;
			res = I2CWrite(0x1C, 1, 0x02, 1, dataBuf); //Normal Operation
			uts.log.Debug(_T("Rest Slave Address res :%d"),res);
			Sleep(50);
			I2CWrite(0x1C, 1, 0x05, 1, dataBuf); //Direct Mode Select
			Sleep(50);
			dataBuf[1] = 0x00;
			res = I2CWrite(0x1C, 1, 0x03, 2, dataBuf); //DAC : 0
			Sleep(15);
			break;

		case VCM_LC898214_MTM_ASAFV3B5:
			Sleep(500); 	
			I2CRead(0xE4,1,0xF0, 1, dataBuf);//Slave address check
			uts.log.Debug(_T("Slave Address :0x[%x] ."), dataBuf[0]);

			dataBuf[0] = 0x01;
			I2CWrite(0xE4,1,0xE0, 1, dataBuf); //Initial Data Down Load
			Sleep(10);
			for (int i= 0  ; i < 100 ; i++)
			{
				I2CRead(0xE4,1,0xE0, 1, dataBuf);
				uts.log.Debug(_T("Down Load Finish :0x[%x] ."), dataBuf[0]);
				if(dataBuf[0] == 0x00) break;
				else Sleep(2); 
			}
			Sleep(1000);

			//Z1,Z2
			dataBuf[0] = 0x00;
			I2CWrite(0xE4,1,0x87, 1, dataBuf);
			
			//HallMin:Z1
			I2CRead(0xE6,1,0x36, 2, dataBuf);
			HallMin_Linear = (dataBuf[0] *256) + dataBuf[1];
			uts.log.Debug(_T("HallMin :0x%x"), HallMin_Linear);
			HallMin_Linear = ((HallMin_Linear<<4)&0xFFFF);
			uts.log.Debug(_T("Shift HallMin :0x%x"), HallMin_Linear);

			//HallMax:Z2
			I2CRead(0xE6,1,0x38, 2, dataBuf);
			HallMax_Linear = (dataBuf[0] *256) + dataBuf[1];
			uts.log.Debug(_T("HallMax :0x%x"), HallMax_Linear);
			HallMax_Linear = ((HallMax_Linear<<4)&0xFFFF);
			uts.log.Debug(_T("Shift HallMax :0x%x"), HallMax_Linear);
			
			dataBuf[0] = 0x9D;
			I2CWrite(0xE4,1,0x87, 1, dataBuf);

			HallMax = 0x6000;
			HallMin = 0xEA80;

			if(HallMin > HallMin_Linear) HallMin_Linear +=0xFFFF;
			TotalRange = (HallMax - 0x0000) + (0xFFFF - HallMin)+1;
			//int _DAC = HallMin + int(double(TotalRange)/4094.0 * double(nVCM_CurrentDAC));
			Dac_HallMin = UINT32(double(HallMin_Linear - HallMin) * (4094.0/double(TotalRange)));
			Dac_HallMax = UINT32(double((HallMax_Linear + 0xFFFF) - HallMin) * (4094.0/double(TotalRange)));

			DLLSetVCM_Move(pDevice, nVCMDriverICType, Dac_HallMin);

			uts.log.Debug(_T("Z1:%d Z2:%d"), Dac_HallMin,Dac_HallMax);
			break;
		case VCM_CN3827B:
			Sleep(20); 
			dataBuf[0] = 0x00;
			dataBuf[1] = 0x00;
			I2CWrite(0x18, 1, 0x00, 2, dataBuf); //Write Position
			break;

//		case VCM_CH9914:
// 			Sleep(100); 
// 
// 			dataBuf[0] = 0x00;
// 			dataBuf[1] = 0x00;
// 			I2CWrite(0x18, 1, 0x00, 2, dataBuf);
// 
// 			break;

		case VCM_CN3937:
			Sleep(100); 

			dataBuf[0] = 0xEC;
			dataBuf[1] = 0xA3;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);

			dataBuf[0] = 0xA1;
			dataBuf[1] = 0x05;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);

			dataBuf[0] = 0xF2;
			dataBuf[1] = 0x08;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);

			dataBuf[0] = 0xDC;
			dataBuf[1] = 0x51;
			I2CWrite(0x18, 0, 0x00, 2, dataBuf);
			Sleep(20);

			break;			
		default:
			uts.log.Warning(_T("nVCMDriverICType [%d] can not found."), nVCMDriverICType);
			break;
		}
	}
	int DLLSetVCM_GetZ1Z2(BaseDevice* pDevice,int nVCMDriverICType,UINT32 *Z1Z2Data)
	{
		int res = 0;

		switch (nVCMDriverICType)
		{
		case VCM_LC898214_MTM_ASAFV3B5:
			{
				int TotalRange = (HallMax - 0x0000) + (0xFFFF - HallMin)+1;
				//int _DAC = HallMin + int(double(TotalRange)/4094.0 * double(nVCM_CurrentDAC));
				Z1Z2Data[0] = UINT32(double(HallMin_Linear - HallMin) * (4094.0/double(TotalRange)));
				Z1Z2Data[1] = UINT32(double((HallMax_Linear + 0xFFFF) - HallMin) * (4094.0/double(TotalRange)));
				Z1Z2Data[2] = HallMin_Linear;//Z1
				Z1Z2Data[3] = HallMax_Linear;//Z2
				Z1Z2Data[4] = HallMin;       //HallMin
				Z1Z2Data[5] = HallMax;       //HallMax
			}
			break;

		default:
			uts.log.Warning(_T("nVCMDriverICType [%d] can not found."), nVCMDriverICType);
			break;
		}

		return res ;
	}
	int DLLSetVCM_HallCal(BaseDevice* pDevice,int nVCMDriverICType,UINT32 *HallData)
	{
		int res = 0;
		g_pDevice = pDevice;

		switch (nVCMDriverICType)
		{
		case 0:
			break;
	 
			default:
				uts.log.Warning(_T("nVCMDriverICType [%d] can not found."), nVCMDriverICType);
				break;
		}

		return res ;
	}


    void DLLSetVCM_Move(BaseDevice* pDevice, int nVCMDriverICType, int nVCM_CurrentDAC)
    {
        assert(nullptr != pDevice);
        assert(nVCMDriverICType < VCM_MAX);
        assert(nVCM_CurrentDAC >= 0);

        g_pDevice = pDevice;
        unsigned char dataBuf[128] = {0};

        switch (uts.info.nDeviceId)
        {
        case eDeviceType::DeviceType_V5U:
            switch (nVCMDriverICType)
            {
            case VCM_SENSOR_INTERNAL:
                GetSensorInstance(pDevice,uts.info.nSensorType)->SetVcmMove(nVCM_CurrentDAC);
                break;
    
			case VCM_LC898214:
				if(nVCM_CurrentDAC >=0 && nVCM_CurrentDAC <1024 )
				{
					int TotalRange = (HallMin - 0x0000) + (0xFFFF - HallMax);

					int _DAC = HallMin - int(double(TotalRange)/1023.0 * double(nVCM_CurrentDAC));
					if(_DAC < 0x0000 ) _DAC += 0xFFFF; 

					dataBuf[0] = (_DAC & 0xff00) >> 8 ;
					dataBuf[1] = (_DAC & 0x00ff) ;
					I2CWrite(0xE4,1,0xA0, 2, dataBuf); //AF Operation

					Sleep(5); 

					for (int i= 0  ; i < 100 ; i++)
					{
						I2CRead(0xE4,1,0x8F, 1, dataBuf);
						//uts.log.Debug(_T("AF not Finish :0x[%x] ."), dataBuf[0]);
						if(dataBuf[0] == 0x00) break;
						else Sleep(5); 
					}
				}
				break;

			case VCM_LC898217:
				if(nVCM_CurrentDAC >=0 && nVCM_CurrentDAC <1024 )
				{
					//int TotalRange = (HallMin - 0x0000) + (0xFFFF - HallMax);

					//int _DAC = HallMin - int(double(TotalRange)/1023.0 * double(nVCM_CurrentDAC));
					//if(_DAC < 0x0000 ) _DAC += 0xFFFF; 
					int _DAC = 1023-nVCM_CurrentDAC;
					dataBuf[0] = (_DAC & 0xFF00) >> 8 ;
					dataBuf[1] = (_DAC & 0x00FF);
					//uts.log.Debug(_T("_DAC :0x[%x] ."), _DAC);
					uts.log.Debug(_T("nVCM_CurrentDAC :%d[0x%X] ."), nVCM_CurrentDAC,nVCM_CurrentDAC);
					I2CWrite(0xE4,1,0x84, 2, dataBuf); //AF Operation

					Sleep(5); 

// 					for (int i= 0  ; i < 100 ; i++)
// 					{
// 						I2CRead(0xE4,1,0x0A, 2, dataBuf);
// 						uts.log.Debug(_T("0x0A :0x[%x] ."), dataBuf[0]);
// 						uts.log.Debug(_T("0x0B :0x[%x] ."), dataBuf[1]);
// 						if(dataBuf[0] == 0x00) break;
// 						else Sleep(5); 
// 					}
				}
				break;
			case VCM_BU64240: 
				dataBuf[0] = (nVCM_CurrentDAC & 0x0300) >> 8;
				dataBuf[1] = (nVCM_CurrentDAC & 0x00ff);
				dataBuf[0] |= 0xc0;
				I2CWrite(0x18, 0, 0, 2, dataBuf);
				break;

			case VCM_GT9762:
				dataBuf[0] = (nVCM_CurrentDAC & 0x03f0) >> 4;
				dataBuf[1] = (nVCM_CurrentDAC & 0x000f) << 4;	
				dataBuf[1] |= 0x01;
				I2CWrite(0x18, 0, 0, 2, dataBuf);
				break;

			case VCM_ZC533:
				dataBuf[0] = (nVCM_CurrentDAC & 0x0300) >> 8;
				dataBuf[1] = (nVCM_CurrentDAC & 0x00ff);
				I2CWrite(0x1C, 1, 0x03, 2, dataBuf); //DAC 
				Sleep(15);

				break;

			case VCM_AK7371:
				dataBuf[0] = nVCM_CurrentDAC>>2;
				dataBuf[1] = (nVCM_CurrentDAC&0x0003)<<6;

				I2CWrite(0x18, 1, 0x00, 2, dataBuf); //New Position
				break;

			case VCM_DW9718S:
				dataBuf[0] = nVCM_CurrentDAC>>8;
				dataBuf[1] = nVCM_CurrentDAC&0x00FF;

				I2CWrite(0x18, 1, 0x02, 2, dataBuf); //New Position
				break;

			case VCM_DW9800W:
				dataBuf[0] = nVCM_CurrentDAC>>8;
				dataBuf[1] = nVCM_CurrentDAC&0x00FF;

				I2CWrite(0x18, 1, 0x03, 2, dataBuf); //New Position
				break;

			case VCM_DW9714V:
				dataBuf[0] = nVCM_CurrentDAC >> 4;
				dataBuf[1] = ((nVCM_CurrentDAC & 0x00F) <<4 )+0x04;

				I2CWrite(0x18, 0, 0, 2, dataBuf); //New Position
				Sleep(20);
				break;
			case VCM_CH9914:

				dataBuf[0] = (nVCM_CurrentDAC >> 4) & 0x03F;
				dataBuf[1] = ((nVCM_CurrentDAC & 0x00F) <<4) +0x04 ;

				I2CWrite(0x18, 0, 0, 2, dataBuf); //New Position
				Sleep(20);
				break;
			case VCM_CN3927B:

				dataBuf[0] = (nVCM_CurrentDAC >> 4) & 0x03F;
				//!!---2018.07.27 Chi-Jen.Liao
				#if 1
				  dataBuf[1] = ((nVCM_CurrentDAC & 0x0F) <<4) + 0x04;
				#else
				  dataBuf[1] = (nVCM_CurrentDAC & 0x0F) <<4;
				#endif
				//!!---2018.07.27 Chi-Jen.Liao				
				I2CWrite(0x18, 0, 0, 2, dataBuf); //Write Position
				Sleep(20); 
				break;
			case VCM_ZC524:

				dataBuf[0] = (nVCM_CurrentDAC >> 4) & 0x03F;
				dataBuf[1] = (nVCM_CurrentDAC & 0x0F) <<4;
				I2CWrite(0x18, 0, 0, 2, dataBuf); //Write Position
				Sleep(20); 
				break;
			case VCM_FP5510E2:
				Sleep(20); 
				dataBuf[0] = (nVCM_CurrentDAC >> 4) & 0x03F;
				dataBuf[1] = (nVCM_CurrentDAC & 0x0F) <<4;
				I2CWrite(0x18, 0, 0, 2, dataBuf); //Write Position
				break;
			case VCM_ZC533DEFAULT:
				dataBuf[0] = (nVCM_CurrentDAC & 0x0300) >> 8;
				dataBuf[1] = (nVCM_CurrentDAC & 0x00ff);
				I2CWrite(0x18, 1, 0x03, 2, dataBuf); //DAC 
				Sleep(15);
				break;
			case VCM_ON898123:
				GetOISDriverInstance(pDevice,uts.info.nOISType)->VCMControl(nVCM_CurrentDAC);
				break;

			case VCM_FP5516WE2:
				dataBuf[0] = nVCM_CurrentDAC>>8;
				dataBuf[1] = nVCM_CurrentDAC&0x00FF;

				I2CWrite(0x18, 1, 0x03, 2, dataBuf);	//output current 

				break;

			case VCM_LC898214_MTM_ASAFV3B5:
				{
					//if(nVCM_CurrentDAC > Dac_HallMax) nVCM_CurrentDAC = Dac_HallMax;
					//if(nVCM_CurrentDAC < Dac_HallMin) nVCM_CurrentDAC = Dac_HallMin;

					if(nVCM_CurrentDAC >0) nVCM_CurrentDAC -=1;

					int TotalRange = (HallMax - 0x0000) + (0xFFFF - HallMin)+1;

					int _DAC = HallMin + int(double(TotalRange)/4094.0 * double(nVCM_CurrentDAC));

					if(_DAC > 0xFFFF ) _DAC -= 0xFFFF; 
				
					dataBuf[0] = (_DAC & 0xff00) >> 8 ;
					dataBuf[1] = (_DAC & 0x00ff) ;
					I2CWrite(0xE4,1,0xA0, 2, dataBuf); //AF Operation

					Sleep(5); 
					for (int i= 0  ; i < 100 ; i++)
					{
						I2CRead(0xE4,1,0x8F, 1, dataBuf);
						//uts.log.Debug(_T("AF not Finish :0x[%x] ."), dataBuf[0]);
						if(dataBuf[0] == 0x00) break;
						else Sleep(5); 
					}
					//Sleep(100); 

					//Hall pos
					I2CRead(0xE4,1,0x3C, 2, dataBuf);
					HallPos = (dataBuf[0] *256) + dataBuf[1];

					uts.log.Debug(_T("DAC:%d [%x] hall:[%x]"),nVCM_CurrentDAC,_DAC,HallPos);
				}

				break;
			case VCM_CN3827B:
				dataBuf[0] = nVCM_CurrentDAC>>4;
				dataBuf[1] = (nVCM_CurrentDAC&0x000F)<<4;
				uts.log.Info(_T("nVCMDAC[%d] 1:[%d] 0:[%d]"), nVCM_CurrentDAC,dataBuf[1],dataBuf[0]);

				I2CWrite(0x18, 1, 0x00, 2, dataBuf); //New Position
				break;
// 			case VCM_CH9914:
// 				Sleep(80);
// 				dataBuf[0] = nVCM_CurrentDAC>>4; //右移
// 				dataBuf[1] =(nVCM_CurrentDAC & 0x0F)<<4; //左移 
// 				uts.log.Info(_T("nVCMDAC[%d] 1:[%d] 0:[%d]"), nVCM_CurrentDAC,dataBuf[1],dataBuf[0]);
// 
// 				I2CWrite(0x18, 0, 0x00, 2, dataBuf);
// 				break;
			case VCM_CN3937:
				Sleep(80);
				dataBuf[0] = nVCM_CurrentDAC>>4; //右移
				dataBuf[1] =(nVCM_CurrentDAC & 0x0F)<<4; //左移 
				uts.log.Info(_T("nVCMDAC[%d] 1:[%d] 0:[%d]"), nVCM_CurrentDAC,dataBuf[1],dataBuf[0]);

				I2CWrite(0x18, 0, 0x00, 2, dataBuf); //New Position
				break;
            default:
                uts.log.Warning(_T("nVCMDriverICType [%d] can not found."), nVCMDriverICType);
                break;
            }
            break;
        default:
            uts.log.Warning(_T("Device [%d] can not drive VCM."), uts.info.nDeviceId);
            break;
        }
    }
}
