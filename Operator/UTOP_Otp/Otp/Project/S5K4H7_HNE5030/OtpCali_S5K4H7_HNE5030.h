#ifndef _OTPCALI_S5K4H7_HNE5030_H_
#define _OTPCALI_S5K4H7_HNE5030_H_

#include <stdint.h>
#include "../../SamsungOtp.h"

#include "REVL/PDAF_QulComm_RevL.h"
#include "usb_frm11_import.h"
#include "afx.h"

namespace OtpCali_S5K4H7_HNE5030
{

#define	MTK_LSC_LEN_HNE5030  1768

#pragma pack(push, 1)

	struct MINFO
	{	
		uint8_t Flag;
		uint8_t VendorID;
		uint8_t year,month,day,hour,minu,sec;
		uint8_t LnesID;
		uint8_t VCMID;
		uint8_t DriverICID;
		uint8_t OIS_FW_VER;
		uint8_t SensorID;
		uint8_t ProductID;
		uint8_t VersionID;
		uint8_t HistoryID;
		uint8_t FuseID[8];
		uint8_t Reserved[11];
		uint8_t EEPOROTP;
 		uint8_t InfoCheckSum;
	};  //37

	struct SegmentInfo
	{
		uint8_t Flag;
		uint8_t Count;
		uint8_t AF_Offest_H;
		uint8_t AF_Offest_L;
		uint8_t AWB_Offest_H;
		uint8_t AWB_Offest_L;
		uint8_t LSC_Offest_H;
		uint8_t LSC_Offest_L;
		uint8_t AEC_Offest_H;
		uint8_t AEC_Offest_L;
		uint8_t PDAF_Offest_H;
		uint8_t PDAF_Offest_L;
		uint8_t OIS_Offest_H;
		uint8_t OIS_Offest_L;
		uint8_t DualCam_Offest_H;
		uint8_t DualCam_Offest_L;
		uint8_t Reserved[8];
		uint8_t CheckSum_Offest_H;
		uint8_t CheckSum_Offest_L;
		uint8_t SegmentCheckSum;
	};  //27

	struct AF
	{
		uint8_t AF_TagID_H;
		uint8_t AF_TagID_L;
		uint8_t Flag;
		uint8_t TypeNum;
		uint8_t AFInf_H;
		uint8_t AFInf_L;
		uint8_t AFMac_H;
		uint8_t AFMac_L;
		uint8_t Reserved1[4];
		uint8_t PosDifeA_H;
		uint8_t PosDifeA_L;
		uint8_t PosDifeB_H;
		uint8_t PosDifeB_L;
		uint8_t PosDifeC_H;
		uint8_t PosDifeC_L;
		uint8_t AFCheckSum;
		uint8_t Reserved2;
	};  //20

	struct AWB
	{
		uint8_t AWB_TagID_H;
		uint8_t AWB_TagID_L;
		uint8_t Flag;
		uint8_t TypeNum;
		uint8_t Mirror;
		uint8_t Flip;
		uint8_t RGain_H;
		uint8_t RGain_L;
		uint8_t BGain_H;
		uint8_t BGain_L;
		uint8_t GbGain_H;
		uint8_t GbGain_L;
		uint8_t Reserved1[6];
		uint8_t AWBCheckSum;
		uint8_t Reserved2[25];

	};  //44

	struct LSC
	{
		uint8_t LSC_TagID_H;
		uint8_t LSC_TagID_L;
		uint8_t Flag;
		uint8_t TypeNum;
		uint8_t VerNum;
		uint8_t Mirror;
		uint8_t Flip;
		uint8_t lscbuffer[MTK_LSC_LEN_HNE5030];		
		uint8_t LSCCheckSum;
		uint8_t Reserved[1776];
	};  //3552

	struct AEC
	{
		uint8_t Reserved[16];
	};  //16

	struct PDAF
	{
		uint8_t Reserved[1216];
	};  //1216

	struct OIS
	{
		uint8_t Reserved[48];
	};  //48

	struct DualCam
	{
		uint8_t Reserved[2195];
		uint8_t CheckSum_TagID_H;
		uint8_t CheckSum_TagID_L;
		uint8_t TotalCheckSum;
	};  //2198
	
	struct OTPData
	{
		MINFO mInfo;
		SegmentInfo segInfo;
		AF af;
		AWB awb;
		LSC lsc;
		AEC aec;
		PDAF pdaf;
		OIS ois;
		DualCam dualCam;
 
	};



#pragma pack(pop)

	class Database;
	class OtpCali_S5K4H7_HNE5030 : public SamsungOtp
	{
	public:
		OtpCali_S5K4H7_HNE5030(BaseDevice *dev);

		int BOARD_SET_INDEX_NUM;
		CString strHVS_SENSOR_SLAVEADDR;
		int wHVS_SENSOR_SLAVEADDR;

	private:
		int LscCali(void *args);
		int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[]);
		int get_wb_cali_data(WB_GAIN *gain, void *out);

		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		int get_uniform_otp_data(void *in, void *out, int maxlen);
		int get_otp_group(void);

		int do_check_otp();
		int do_check_otp(void *args);	



		BOOL DW9763_Read(USHORT address,unsigned char *Data,int size);
		BOOL DW9763_Write(USHORT address,unsigned char *Data,int size);
		BOOL BL24C64A_Read(USHORT address,unsigned char *Data,int size);
		BOOL BL24C64A_Write(USHORT address,unsigned char *Data,int size);

		CString GetModulePath();
		void OnOTPRead(int startAddr,char* value,int lens);


	};
}


#endif
