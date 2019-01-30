#pragma once

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K3P9SX_HNF1065
{
#define CROSS_TALK_LEN 2048
#define MTK_LSC_LEN 1868
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)
	
	struct MINFO
	{	
		uint8_t moudule_id[2];
		uint8_t day;
		uint8_t month;
		uint8_t year[2];//0x0E14, Mean 2014年,Addr[0x0004]=0x14
		uint8_t sensor_id[2];
		uint8_t lens_id[2];
		uint8_t vcm_id[2];
	};
	
	struct AWB//5100K
	{
		uint8_t r[2];
		uint8_t gr[2];
		uint8_t gb[2];
		uint8_t  b[2]; 
		//golden
		uint8_t r_g[2];
		uint8_t gr_g[2];
		uint8_t gb_g[2];
		uint8_t  b_g[2]; 
	};
	struct AF
	{	uint8_t reversed[20];
		uint8_t flaglsc;//0x04
		uint8_t reversed1;
	};
	
	struct LIGHT
	{	uint8_t lightsource_rg[2];
		uint8_t lightsource_bg[2];
	};

	struct FLAGCHECK
	{
		uint8_t flag;
		uint8_t check;
	};
	struct OTPData
	{
		MINFO minfo; //12byte
		AWB awb5100;//16
		AF af;		//22
		AWB awb3100;
		FLAGCHECK awb_fc5100;
		uint8_t reversed[2];
		FLAGCHECK awb_fc3100;
		LIGHT light5100;//4
		LIGHT light3100;
		FLAGCHECK light_fc5100;
		FLAGCHECK light_fc3100;
		AWB awb4000;
		LIGHT light4000;
		FLAGCHECK awb_fc4000;
		FLAGCHECK  light_fc4000;
		uint8_t reversed1[116];
		uint8_t QrCode[17];
		FLAGCHECK qrcode;
		uint8_t reversed2[269];
		uint8_t lsc_qual[1110];
		FLAGCHECK lscqual;
		uint8_t reversed3[1448];
		uint8_t lsc_mtk[1868];
		FLAGCHECK lscmtk;
		uint8_t reversed4[178];
		uint8_t cross_talk[2048];
		FLAGCHECK crosstalk;
	};

#pragma pack(pop)

	class Database;
	class OtpCali_S5K3P9SX_HNF1065 : public SamsungOtp
	{
	public:
		OtpCali_S5K3P9SX_HNF1065(BaseDevice *dev);

	private:
		int LscCali(void *args);//默认高通
		int LSCMTK_Cail(void *args);
		int WBCali(void *args);
		int CrossTalk(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);
		int upload_lsc_cali_data(void *raw, int len);
		int get_minfo_from_db(void *args);
		int do_dump_otp();
	
		int do_qualcomm_Type_Cali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],int type);
		int do_check_otp(void *args,void *args1);
	};
}

