#ifndef _OTPCALI_Hi846_MF82K_H_
#define _OTPCALI_Hi846_MF82K_H_

#include <stdint.h>
#include "../../HynixOtp.h"

namespace OtpCali_Hi846_MF82K
{

#define IMX_519_SPC_LEN 140
#define IMX_519_DCC_LEN 96
#define IMX_519_PD_LEN  48
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)

	struct MINFO
	{	
		uint8_t basi_flag;
		uint8_t mid;
		uint8_t af_ff_flag;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t sensor;
		uint8_t lens;
		uint8_t vcm;
		uint8_t vcmdriver;
		uint8_t version;
		uint8_t reserved[6];
		uint8_t sum;
	};

	struct WB
	{
		uint8_t wb_flag;
		uint8_t r_gr[2];
		uint8_t b_gb[2];
		uint8_t gb_gr[2];
		uint8_t r_gr_g[2];
		uint8_t b_gb_g[2];
		uint8_t gb_gr_g[2];
		uint8_t reserved[17];
		uint8_t sum;
	};
	
	struct LSC
	{	
		uint8_t lsc_flag;
		uint8_t lsc[QULCOMM_LSC_LEN];
		uint8_t sum;
	};

	struct OTPData
	{
		MINFO minfo; 
		WB wb;
		LSC lsc;
	};



#pragma pack(pop)

	class Database;
	class OtpCali_Hi846_MF82K : public HynixOtp
	{
	public:
		OtpCali_Hi846_MF82K(BaseDevice *dev);

	private:
		
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);
		int get_minfo_from_db(void *args);
		
		int LscCali(void *args);

		int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[]);


		int do_check_otp(void *args);
		//IMX519_PDAF

	
	};
}


#endif
