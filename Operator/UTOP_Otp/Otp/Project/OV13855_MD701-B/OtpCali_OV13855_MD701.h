#ifndef _OTPCALI_OV13855_MD701_H_
#define _OTPCALI_OV13855_MD701_H_

#include <stdint.h>
#include "../../OVOtp.h"

namespace OtpCali_OV13855_MD701
{

#define MTK_SPC_LEN_OV13855 496
#define MTK_DCC_LEN_OV13855 876
#define	MTK_LSC_LEN_MD701  1868

#pragma pack(push, 1)

	struct MINFO
	{	
		uint8_t flag;
		//---------------20170713 Edward Add
		uint8_t StoregeLoc;
		uint8_t sn[10];
		uint8_t IDDef[8];
		uint8_t Ver[4];
		uint8_t sensorfuse[16];
		uint8_t BuildInfo;
		uint8_t ProData[2];
		uint8_t year,month,day,hour,minu;
		uint8_t Reserved[2];
		//-----------20170713 Edward Add
		uint8_t checksum[2];
	};

	struct WB
	{
		uint8_t flag;
		unsigned short lightsource_ev;
		unsigned short lightsource_u;
		unsigned short lightsource_v;
		unsigned short r;
		unsigned short gr;
		unsigned short gb;
		unsigned short b;
		unsigned short rg;
		unsigned short bg;
		unsigned short grgb;

		unsigned short r_g;
		unsigned short gr_g;
		unsigned short gb_g;
		unsigned short b_g;
		unsigned short rg_g;
		unsigned short bg_g;
		unsigned short grgb_g;
		uint8_t checksum[2];
	};

	struct AF
	{
		uint8_t AFflag;
		uint8_t Revers[2];
		uint8_t inf[2];
		uint8_t mup[2];
		uint8_t Reser[4];
		uint8_t AfDirection;
		uint8_t Reserved[119];
		uint8_t checksum[2];
	};

	struct LSC
	{
		uint8_t flag;
		uint8_t lscbuffer[MTK_LSC_LEN_MD701];
		uint8_t Reserved[452];
		uint8_t checksum[2];
	};

	struct PDAF
	{
		uint8_t flag;
		uint8_t spc[MTK_SPC_LEN_OV13855];
		uint8_t dcc[MTK_DCC_LEN_OV13855];
		uint8_t checksum[2];
	};

	struct OTPData
	{
		MINFO minfo;
		AF af;
		WB wb;
		LSC lsc;
		uint8_t Totalchecksum[2];
		uint8_t TotalReserved[781];
		PDAF pdaf;
	};



#pragma pack(pop)

	class Database;
	class OtpCali_OV13855_MD701 : public OVOtp
	{
	public:
		OtpCali_OV13855_MD701(BaseDevice *dev);

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
