#pragma once

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K3P9SX_MF83Y
{
#define QULCOMM_SPC_LEN 1030
#define QULCOMM_DCC_LEN 994
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)
	
	struct MINFO
	{	
		uint8_t flag;
		uint8_t version;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t moduleID;
		uint8_t lensID;
		uint8_t vcmID;
		uint8_t vcmDriverID;
		uint8_t colorTemp;
		uint8_t mirFlipFlag;
		uint8_t qrCode[4];
		uint8_t checkSum;
	};
	
	struct AWB
	{
		uint8_t flag;
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
		uint8_t rg_golden[2];
		uint8_t bg_golden[2];
		uint8_t gbgr_golden[2];
		uint8_t checkSum;
		uint8_t reserved[2]; 
	};

	struct LSC
	{	uint8_t flag;
		uint8_t lsc[QULCOMM_LSC_LEN];
		uint8_t checkSum;
	};

	struct OTPData
	{
		MINFO minfo; 
		AWB awb;
		LSC lsc;
	};

#pragma pack(pop)

	class Database;
	class OtpCali_S5K3P9SX_MF83Y : public SamsungOtp
	{
	public:
		OtpCali_S5K3P9SX_MF83Y(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		
		int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, 
			int w, int h, uint8_t out[],uint8_t awbout[]);

		int do_check_otp(void *args);
	};
}

