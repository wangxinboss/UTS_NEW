#ifndef _OTPCALI_OV13855_MA869_H_
#define _OTPCALI_OV13855_MA869_H_

#include <stdint.h>
#include "../../OVOtp.h"

namespace OtpCali_OV13855_MA869
{

#define MTK_SPC_LEN_OV13855 496
#define MTK_DCC_LEN_OV13855 876
#define	MTK_LSC_LEN_MA869  1868

#pragma pack(push, 1)

	struct MINFO
	{	
		uint8_t flag;
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
		uint8_t sensorfuse[16];	
		uint8_t Reserved[3];
		uint8_t EepromorOTP;
		uint8_t checksum;
	};

	struct SegINFO
	{
		uint8_t flag;
		uint8_t SegCount;
		uint8_t SegAFOffset_H;
		uint8_t SegAFOffset_L;
		uint8_t SegAWBOffset_H;
		uint8_t SegAWBOffset_L;
		uint8_t SegLSCOffset_H;
		uint8_t SegLSCOffset_L;
		uint8_t SegAECOffset_H;
		uint8_t SegAECOffset_L;
		uint8_t SegPDAFOffset_H;
		uint8_t SegPDAFOffset_L;
		uint8_t Reserved[12];
		uint8_t SegChecksumOffset_H;
		uint8_t SegChecksumOffset_L;
		uint8_t checksum;
	};

	struct WB
	{
		uint8_t SegAWBTabID_H;
		uint8_t SegAWBTabID_L;
		uint8_t flag;
		uint8_t TypeNum;
		uint8_t mirror;
		uint8_t filp;
		uint8_t r_H;
		uint8_t r_L;
		uint8_t gr_H;
		uint8_t gr_L;
		uint8_t gb_H;
		uint8_t gb_L;
		uint8_t b_H;
		uint8_t b_L; 
		uint8_t Reserved[8];
		uint8_t checksum;
	};

	struct AF
	{
	 	uint8_t SegAF_TagID_H;
		uint8_t SegAF_TagID_L;
		uint8_t flag;
		uint8_t TypeNum;
		uint8_t inf[2];
		uint8_t mup[2];
		uint8_t Reser[4];
		uint8_t PosDiffA_H; //姿勢差 水平與向下
		uint8_t PosDiffA_L;
		uint8_t PosDiffB_H; //姿勢差 水平與向上
		uint8_t PosDiffB_L;
		uint8_t PosDiffC_H; //姿勢差 向上與向下
		uint8_t PosDiffC_L;
		uint8_t checksum;
	};

	struct LSC
	{
		uint8_t SegLSCTagID_H;
		uint8_t SegLSCTagID_L;
		uint8_t flag;
		uint8_t TypeNum;
		uint8_t VerNum;
		uint8_t mirror;
		uint8_t filp;
		uint8_t lscbuffer[MTK_LSC_LEN_MA869];
		uint8_t checksum;
	};

	struct PDAF
	{
		uint8_t SegPDAFTagID_H;
		uint8_t SegPDAFTagID_L;
		uint8_t flag;
		uint8_t TypeNum;
		uint8_t Reser[6];
		uint8_t spc[MTK_SPC_LEN_OV13855];
		uint8_t Reser2[388];
		uint8_t dcc[MTK_DCC_LEN_OV13855];
		uint8_t checksum;
	};

	struct OTPData
	{
		MINFO minfo;
		SegINFO SI;   //Segment Information
		AF af;
		uint8_t Reserved;
		WB wb;
		uint8_t Reserved2[21];
		LSC lsc;
		uint8_t Reserved3[1692];
		PDAF pdaf;
		uint8_t Reserved4[1688];
		uint8_t SegChecksumTagID_H;
		uint8_t SegChecksumTagID_L;
		uint8_t Totalchecksum;

		
	};



#pragma pack(pop)

	class Database;
	class OtpCali_OV13855_MA869 : public OVOtp
	{
	public:
		OtpCali_OV13855_MA869(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		int get_uniform_otp_data(void *in, void *out, int maxlen);
		int get_otp_group(void);

		int DCC0Cali(void *args) ;
		int PDAFVerify(void *args);
 		int do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len);

		int get_wb_cali_data(WB_GAIN *gain, void *out);
	};
}


#endif
