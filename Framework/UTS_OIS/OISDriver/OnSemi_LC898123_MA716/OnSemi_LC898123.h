#ifndef _OIS_ON123_H_
#define _OIS_ON123_H_

#include <stdint.h>
#include "../../OISControl.h"


class OIS_ON123 : public UTS::OISDriver
{
public:
	OIS_ON123(UTS::BaseDevice *dev);

	int Write(int addr, const void *data, int len);
	int Read(int addr, void *data, int len);

	int initialize();
	int HallCal(UINT32 *HallData);
	int ACCCal();
	int OISControl(bool mode);
	int VCMControl(int dac) ;
	int ServoControl(int mode);

	int SetGyroGainX(unsigned int Gain);
	int SetGyroGainY(unsigned int Gain);
	int SetShiftGain(unsigned int Gain);
	int SaveGyroGainData();
	int LoadFW();
	int CheckFW(int _correctver);

	int SetVrtPos( unsigned int UcPos );
	int SetHrzPos( unsigned int UcPos );

	int GetGyroGainX(UINT32 *Gain);
	int GetGyroGainY(UINT32 *Gain);

	int SaveCrossTalkData(void *MixingValue,void *LinearityValue);

	//int DumpFlashData(UINT32 *BufDat);
	
	int SetADOffSetX(int CenterX);
	int SetADOffSetY(int CenterY);
	int GetADOffSetX(int *CenterX);
	int GetADOffSetY(int *CenterY);
	int SaveADOffset(int CenterX,int CenterY);

protected:
	bool CheckI2C();
	bool ReadStatus();
};

#endif

extern OIS_ON123 *p_Device;

typedef	unsigned char       UINT_8;
typedef	unsigned long       UINT_32;
typedef	long                INT_32;
typedef	unsigned short      UINT_16;

void  RamWrite32A(INT_32 address,INT_32 data);
void  RamRead32A(UINT_16 address,void * data);

void  WPBCtrl(UINT_8 state);
void  WitTim( UINT_16 );

void CntWrt( void * data, UINT_16 length);
void CntRd3( UINT_32 address, void *data, UINT_16 length) ;
