#ifndef _OTPCALI_S5K5E2_MD701_H_
#define _OTPCALI_S5K5E2_MD701_H_

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K5E2_MD701
{

#define	LSC_LEN_MD701  416

#pragma pack(push, 1)

	struct MINFO
	{	
		uint8_t flag;
		uint8_t storagelocation;
		uint8_t sn[10];
		uint8_t venderid;
		uint8_t sensorid;
		uint8_t lensid;
		uint8_t vcmid;
		uint8_t drivericid;
		uint8_t irid;
		uint8_t gyroid;
		uint8_t colortempid;
		uint8_t fpcver;
		uint8_t caliver;
		uint8_t lensver;
		uint8_t oisver;
		uint8_t sensorfuse[16];
		uint8_t camearastage;
		uint8_t factory;
		uint8_t line;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t minute;
		uint8_t reserved[14];
		uint8_t checksum[2];
	};

	struct WB
	{
		uint8_t flag;
		uint8_t oc[16];
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
		uint8_t reserved[11];
		uint8_t checksum[2];
	};

	struct LSC
	{
		uint8_t lscbuffer[LSC_LEN_MD701];
		uint8_t reserved[5];
		uint8_t flag;
		uint8_t checksum[2];
	};

	
	struct OTPData
	{
		MINFO minfo;    
		WB wb;
		LSC lsc;
	};



#pragma pack(pop)

	class Database;
	class OtpCali_S5K5E2_MD701 : public SamsungOtp
	{
	public:
		OtpCali_S5K5E2_MD701(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		int get_uniform_otp_data(void *in, void *out, int maxlen);
		int get_otp_group(void);
		int do_SamsungLSCCali(uint16_t *pRaw10,int width, int height, char *lsc, int &lsc_len,char *sram, int &sram_len);

		
	};
}


#endif
