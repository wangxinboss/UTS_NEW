#include "StdAfx.h"
#include "CommonFunc.h"
#include <string>
#include "UTSDefine.h"
#include "OnSemi_LC898123.h"
#include "AdapterDev.h"
#include "Ois.h"

#include "UsbDrv.H"
#pragma comment(lib,"UsbDrv.lib")

OIS_ON123 *p_Device;

OIS_ON123::OIS_ON123(UTS::BaseDevice *dev) : OISDriver(dev)
{
	i2cMode = UTS::BIT16_BIT16;
	i2cAddr = 0x7C;  //Slave add
	p_Device = this;
}

int OIS_ON123::Write(int addr, const void *data, int len)
{
	if (OISDriver::Write(addr, data, len) < 0)
		return -1;

	return 0;
}

int OIS_ON123::Read(int addr, void *data, int len)
{
	if (OISDriver::Read(addr, data, len) < 0)
		return -1;

	return 0;
}

int OIS_ON123::initialize()
{
	int error = UTS::OIS_ERROR_NO;

	Sleep(100);

	if(CheckI2C() == false) 
		error = UTS::OIS_ERROR_I2C;

	return (SET_ERROR(error));

}

bool OIS_ON123::CheckI2C()
{
	INT32 i2cdata;
	RamWrite32A(0xC000,0x00D00100);
	RamRead32A(0xD000,&i2cdata);

	printk(_T("I2C status : %d\n"), i2cdata);
	if(i2cdata == 0x82 || i2cdata == 0xC2)
		return true;
	else
		return false;
}

bool OIS_ON123::ReadStatus()
{
	UINT8	UcStRd = 1;
	
	for (int i = 0 ;i<10 ;i++)
	{
		UcStRd = RdStatus(1);

		if(UcStRd == 0x00) break;
		else Sleep(5);
	}

	if(UcStRd == 0x00) return true;
	else return false; 
}

int OIS_ON123::ServoControl(int mode)
{
	int error = UTS::OIS_ERROR_NO;

	if(2 == mode)
	{
		RamWrite32A(0xF011,0x00);
		RamWrite32A(0xF01C,0x00);
	}else if(4 == mode)
	{
		RamWrite32A(0xF011,0x00);
		RamWrite32A(0xF01C,0x01);
	}

	if(ReadStatus() == false) 
	{
		error = UTS::OIS_ERROR_SERVO;
	}

	return (SET_ERROR(error));
}


int OIS_ON123::HallCal(UINT32 *HallData)
{
	int error = UTS::OIS_ERROR_NO;

	VCMControl(0x400);
	Sleep(1000);

	UINT32	UlReadVal = 0;
	UINT_8  res_writedata = 1;

	UlReadVal = TneRun();

	if(UlReadVal == 0x02)
	{
		res_writedata = WrHallCalData() ;
		if(res_writedata != 0) error = UTS::OIS_ERROR_SAVEDATA;
	}else
		error = UTS::OIS_ERROR_HALL;

	//Focus Position
	//VCMControl(0);

	//new 
	HallData[0] = StAdjPar.StHalAdj.UsHlxCna ;				// Hall Center Value after Hall Adjust
	HallData[1] = StAdjPar.StHalAdj.UsHlxMax ;				// Hall Max Value
	HallData[2] = StAdjPar.StHalAdj.UsHlxMxa ;				// Hall Max Value after Hall Adjust
	HallData[3] = StAdjPar.StHalAdj.UsHlxMin ;				// Hall Min Value
	HallData[4] = StAdjPar.StHalAdj.UsHlxMna ;				// Hall Min Value after Hall Adjust
	HallData[5] = StAdjPar.StHalAdj.UsHlxGan ;				// Hall Gain Value
	HallData[6] = StAdjPar.StHalAdj.UsHlxOff ;				// Hall Offset Value
	HallData[7] = StAdjPar.StHalAdj.UsAdxOff ;				// Hall A/D Offset Value
	HallData[8] = StAdjPar.StLopGan.UlLxgVal ;              // Loop Gain X
	HallData[9] = StAdjPar.StGvcOff.UsGxoVal ;             // Gyro A/D Offset X

	HallData[10] = StAdjPar.StHalAdj.UsHlyCna ;				// Hall Center Value after Hall Adjust
	HallData[11] = StAdjPar.StHalAdj.UsHlyMax ;				// Hall Max Value
	HallData[12] = StAdjPar.StHalAdj.UsHlyMxa ;				// Hall Max Value after Hall Adjust
	HallData[13] = StAdjPar.StHalAdj.UsHlyMin ;				// Hall Min Value
	HallData[14] = StAdjPar.StHalAdj.UsHlyMna ;				// Hall Min Value after Hall Adjust
	HallData[15] = StAdjPar.StHalAdj.UsHlyGan ;				// Hall Gain Value
	HallData[16] = StAdjPar.StHalAdj.UsHlyOff ;				// Hall Offset Value
	HallData[17] = StAdjPar.StHalAdj.UsAdyOff ;				// Hall A/D Offset Value
	HallData[18] = StAdjPar.StLopGan.UlLygVal ;              // Loop Gain X
	HallData[19] = StAdjPar.StGvcOff.UsGyoVal ;             // Gyro A/D Offset X

	if(error == UTS::OIS_ERROR_HALL)
	{
		return UlReadVal;
	}else
		return (SET_ERROR(error));
}

int OIS_ON123::ACCCal()
{
	int error = UTS::OIS_ERROR_NO;
	UINT_8  res_acccal = 1;
	UINT32	UlReadVal = 0;

	UlReadVal = TneAvc(0x10);

	if(UlReadVal == 0x02)
	{
		UlReadVal = TneAvc(0x80);

		if(UlReadVal == 0x02)
		{
			res_acccal = WrAclOffsetData();
			if(res_acccal != 0 )
			{
				error = UTS::OIS_ERROR_SAVEDATA;
			}
		}else
			error = UTS::OIS_ERROR_ACC;
	}else
		error = UTS::OIS_ERROR_ACC;

	if(error == UTS::OIS_ERROR_ACC)
	{
		return UlReadVal;
	}else
		return (SET_ERROR(error));
}


int OIS_ON123::OISControl(bool mode)
{
	int error = UTS::OIS_ERROR_NO;

	if(true == mode)
	{
		//ServoControl(true);
		//RamWrite32A(0xF012,0x01);
		OisEna();
	}else
	{
		//RamWrite32A(0xF012,0x00);
		OisDis();
	}

	return (SET_ERROR(error));
}

int OIS_ON123::VCMControl(int dac)
{
	if(dac >=0 && dac <2047 )
	{
		dac += 0x10000;

		RamWrite32A(0xF01A,dac);
		ReadStatus();
	}
	return 0;
}

#define	USB_VendorId	0x3412
#define	USB_ProductId	0x375A

#define	CMD_READ_VER	0x41	// Read version command
#define	CMD_UPDATE		0xA0	// Flash update command

int OIS_ON123::LoadFW()
{
	int error = UTS::OIS_ERROR_NO;
	int result =  -1;

#if 1
	printk(_T("OIS DLL : load FW\n"));
	result = FlashUpdateF40();
	printk(_T("FlashUpdate : %d\n"), result);
	Sleep(20);

	if(result != 0x00) 
	{
		result = FlashUpdateF40();
	}
#else

	unsigned char UsbBuff[9];
	unsigned char rUsbBuff[9];

	//Set GPIO Hi
	WPBCtrl(WPB_OFF) ;
	
	if(USB_Find(USB_VendorId, USB_ProductId))
	{
		printk(_T("HID_Init() --> %d\n"), USB_Init(USB_VendorId, USB_ProductId));
		memset(UsbBuff,0,sizeof(char)*9);
		memset(rUsbBuff,0,sizeof(char)*9);
	} 
	else 
	{
		printk(_T("USB Device not found!\n"));
		result = -1;
		goto end;
	}

	UsbBuff[0] = CMD_UPDATE;	// Flash update
	UsbBuff[1] = 0x06;          // Length
	UsbBuff[2] = 0x40;			// CMD_RESERVE0_1
	UsbBuff[3] = 0x00;			// CMD_RESERVE0_2
	UsbBuff[4] = 0x40;			// CMD_RESERVE0_3
	UsbBuff[5] = 0x26;          // Checksum
	
	Sleep(100);
	
	if(USB_Write(UsbBuff, 9))
	{
		Sleep(100);

		USB_Read(rUsbBuff, 9);
		if(UsbBuff[0] == CMD_UPDATE) // Check command
		{
			result = rUsbBuff[4];
			printk(_T("FlashUpdate : %d\n"), result);
			if(result != 0x00)
			{
				USB_Write(UsbBuff, 9);
				Sleep(100);
				USB_Read(rUsbBuff, 9);
				result = rUsbBuff[4];
				printk(_T("FlashUpdate : %d\n"), result);
			}
		}
	} else 
	{
		printk(_T("Write USB failed!\n"));
	}	
	
	USB_Close();

#endif
	

	return result;
}

int OIS_ON123::CheckFW(int _correctver)
{
	int error = UTS::OIS_ERROR_NO;
	int result =  0;

	INT32 fwver;
	RamRead32A(0x8000,&fwver);

	fwver &= 0xFFFF;
	if(fwver != _correctver) error = UTS::OIS_ERROR_FW;

	printk(_T("FW ver:0x%x"),fwver);
	return (SET_ERROR(error));
}

int OIS_ON123::SetGyroGainX(unsigned int Gain)
{           
	RamWrite32A(0x82B8,Gain);
	return (SET_ERROR(UTS::OIS_ERROR_NO));
}
int OIS_ON123::SetGyroGainY(unsigned int Gain)
{
	RamWrite32A(0x8318,Gain);
	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::SaveGyroGainData()
{
	WrGyroGainData( );
	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::SetVrtPos( unsigned int UcPos )
{
	TneVrtPos(UcPos);
	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::SetHrzPos( unsigned int UcPos )
{
	TneHrzPos(UcPos);
	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::GetGyroGainX(UINT32 *fix )
{
	RamRead32A(0x82B8,fix);

	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::GetGyroGainY(UINT32 *fix )
{
	RamRead32A(0x8318,fix);

	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::SaveCrossTalkData(void *MixingValuebuf,void *LinearityValuebuf)
{
	mlMixingValue	*mixVal =  (mlMixingValue*)MixingValuebuf;
	mlLinearityValue *linVal =  (mlLinearityValue*)LinearityValuebuf;

	WrLinMixCalData( 1,mixVal, linVal );

	//ReBoot 
	RamWrite32A(CMD_IO_ADR_ACCESS,SYSDSP_REMAP);
	RamWrite32A(CMD_IO_DAT_ACCESS,0x00001000);

	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::SetShiftGain(unsigned int fix)
{
	RamWrite32A(0x8780,fix);

	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::SetADOffSetX(int CenterX)
{
	RamWrite32A( HALL_RAM_HXOFF,  (UINT_32)((CenterX << 16 ) & 0xFFFF0000 )) ;

	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::SetADOffSetY(int CenterY)
{
	RamWrite32A( HALL_RAM_HYOFF,  (UINT_32)((CenterY << 16 ) & 0xFFFF0000 )) ;

	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::GetADOffSetX(int *CenterX)
{
	UINT_32 Readata;

	RamRead32A(HALL_RAM_HXOFF,&Readata);
	*CenterX  = (int)(Readata>>16);

	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::GetADOffSetY(int *CenterY)
{
	UINT_32 Readata;

	RamRead32A(HALL_RAM_HYOFF,&Readata);
	*CenterY  = (int)(Readata>>16);

	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

int OIS_ON123::SaveADOffset(int CenterX,int CenterY)
{
	WrADOOffsetData();

	return (SET_ERROR(UTS::OIS_ERROR_NO));
}

// int OIS_ON123::DumpFlashData(UINT32 *BufDat)
// {
// 	UINT_32	UiChkSum;
// 
// 	//ReadCalDataF40( BufDat, &UiChkSum);
// 	return (SET_ERROR(UTS::OIS_ERROR_NO));
// }

//---------------------------------------------------------------
void  RamWrite32A(INT_32 address,INT_32 data)
{
	char databuf[4];

	databuf[0] = (char)((data & 0xFF000000) >> 24);
	databuf[1] = (char)((data & 0x00FF0000) >> 16);
	databuf[2] = (char)((data & 0x0000FF00) >> 8);
	databuf[3] = (char)(data & 0x000000FF) ;

	p_Device->Write(address,databuf,4);
	//WitTim(2);
	//Sleep(1);
}

void  RamRead32A(UINT_16 address,void * out)
{
	unsigned char data[4]; 
	UINT_32 *databuf = (UINT_32 *)out;

	p_Device->Read(address,data,4);
	*databuf = ((data[0] << 24 ) + (data[1] << 16) + (data[2] << 8) + (data[3]));
	//WitTim(2);
	//Sleep(1);
}


void WPBCtrl(UINT_8 state)
{
	
}

void CntWrt( void * inbuf, UINT_16 length)
{
	UINT_8 *buf = (UINT_8 *)inbuf;
	UINT_8 databuf[163];

	int address = (buf[0] << 8) + buf[1];

	memcpy(databuf,buf + 2,length - 2);
	p_Device->Write(address,databuf,length - 2);
}

void CntRd3( UINT_32 address, void *inbuf, UINT_16 length)
{
	UINT_8 *buf = (UINT_8 *)inbuf;
	
	p_Device->Read(address,buf,length);
}
