#pragma once
#include <stdint.h>
#include "BaseDevice.h"
#include "AdapterDev.h"

#ifdef OIS_EXPORTS
#define OIS_API __declspec(dllexport)
#else
#define OIS_API __declspec(dllimport)
#endif

namespace UTS
{
	enum OIS_TYPE
	{
		OIS_TYPE_NULL,
		OIS_TYPE_OnSemi_LC898123,
		OIS_TYPE_SIZE,
	};
	enum OIS_ERROR
	{
		OIS_ERROR_NO,
		OIS_ERROR_NOTSUPPORT,
		OIS_ERROR_I2C,
		OIS_ERROR_SERVO,
		OIS_ERROR_HALL,
		OIS_ERROR_ACC,
		OIS_ERROR_OIS,
		OIS_ERROR_FW,
		OIS_ERROR_HallCheck,
	    OIS_ERROR_SAVEDATA,
	};


	class OIS_API OISDriver
	{
	public:
		OISDriver(BaseDevice *dev);
		virtual ~OISDriver(void);

		virtual int initialize(){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int HallCal(UINT32 *HallData){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int HallCheck(float *xMaxAcc,float *yMaxAcc,float *xLimit,float *yLimit,void *xy_raw_data){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int ACCCal(){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int OISControl(bool mode){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int VCMControl(int dac) {return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SetGyroGainX(unsigned int Gain){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SetGyroGainY(unsigned int Gain){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SaveGyroGainData(){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int LoadFW(){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int ReadFWver(INT32 *Ver){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SetSltPos( unsigned int UcPos ){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SetVrtPos( unsigned int UcPos ){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SetHrzPos( unsigned int UcPos ){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int GetGyroGainX(UINT32 *Gain){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int GetGyroGainY(UINT32 *Gain){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int ServoControl(int mode){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int GetGyroX(INT32 *X){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int GetGyroY(INT32 *Y){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}

        virtual int SaveCrossTalkData(void *MixingValue,void *LinearityValue){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SetShiftGain(unsigned int Gain){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		//virtual int DumpFlashData(UINT32 *BufDat){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SetADOffSetX(int CenterX){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SetADOffSetY(int CenterY){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int GetADOffSetX(int *CenterX){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int GetADOffSetY(int *CenterY){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int SaveADOffset(int CenterX,int CenterY){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
		virtual int ErCrossTalkData(){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}

		void SetDevice(BaseDevice *pDevice);
		int oisType;
	
		BaseDevice *dev;

		virtual int Write(int addr, const void *data, int len);
		virtual int Read(int addr, void *data, int len);

	protected:
		AdapterDev *Adev;
		

		int i2cAddr;
		int i2cMode;
		int errorNo;

		int SET_ERROR(int errorNo) { this->errorNo = errorNo; return -errorNo;}

		
	};


	EXTERN_C
	{
		OIS_API OISDriver *GetOISDriverInstance(BaseDevice *dev, int oisType);
	}
}

