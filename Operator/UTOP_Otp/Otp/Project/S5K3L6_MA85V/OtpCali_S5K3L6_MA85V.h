#pragma once

#include <stdint.h>
#include "../../SamsungOtp.h"


namespace OtpCali_S5K3L6_MA85V
{
#define	MTK_LSC_LEN 1868
#define MTK_SPC_LEN 496
#define MTK_DCC_LEN 908
#pragma pack(push, 1)

	struct MINFO_Group
	{	
		uint8_t moudleID;
		uint8_t LensID;
		uint8_t vcmID;
		uint8_t driverICID;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t mInfoCheckSum;		
	};

	struct LSC_Group
	{
		uint8_t lscbuffer[MTK_LSC_LEN];
		uint8_t lscchecksum;
	};


	struct S5K3L6_WB_Group
	{
		uint8_t r;
		uint8_t gr;
		uint8_t gb;
		uint8_t b;
		uint8_t g_r;
		uint8_t g_gr;
		uint8_t g_gb;
		uint8_t g_b;
		uint8_t wb_CheckSum;
	};


	struct S5K3L6_AF_Group
	{
		uint8_t Af_Inf[2];
		uint8_t Af_Mup[2];
		uint8_t afchecksum;
	};

	struct PDAF
	{
		uint8_t spc[MTK_SPC_LEN];
		uint8_t dcc[MTK_DCC_LEN];
		uint8_t checksum;
	};

	struct OTPData
	{
		uint8_t mInfoFlag;
		MINFO_Group mInfo;
		uint8_t afFlag;
		S5K3L6_AF_Group af;
		uint8_t wb_flag;
		S5K3L6_WB_Group wb;
		uint8_t lscFlag;
		LSC_Group  lsc;
		uint8_t pdafflag;
		PDAF pdaf;		
	};


#pragma pack(pop)

	class Database;
	class OtpCali_S5K3L6_MA85V : public SamsungOtp
	{
	public:
		OtpCali_S5K3L6_MA85V(BaseDevice *dev);

	private:
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int LscCali(void *args);
		int get_minfo_from_db(void *args);

		//int do_check_otp(void *args);
		int DCC0Cali(void *args) ;
		int PDAFVerify(void *args);
 		int do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len);
	};
}

