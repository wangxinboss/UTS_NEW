#pragma once

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K3L8_MA83F
{

#define	MTK_LSC_LEN  1868

#pragma pack(push, 1)

	struct MINFO
	{	
		uint8_t flag;
		uint8_t firstPixel;
		uint8_t MirFlipFlag;
		uint8_t width_Low;
		uint8_t width_High;
		uint8_t Height_Low;
		uint8_t Height_High;
		uint8_t LscVersionLow;
		uint8_t LscVersionHigh;
		uint8_t PDAFVersionLow;
		uint8_t PDAFVersionHigh;
		uint8_t ModuleID;
		uint8_t LensID;
		uint8_t Year;
		uint8_t Month;
		uint8_t Day;
	};

	struct WB
	{
		uint8_t version;
		uint8_t enFlag;
		uint8_t r;
		uint8_t gr;
		uint8_t gb;
		uint8_t b;
		uint8_t r_g;
		uint8_t gr_g;
		uint8_t gb_g;
		uint8_t b_g;
	};


	struct LSC
	{
		uint8_t checkSum;
		uint8_t lscbuffer[MTK_LSC_LEN];
	};

	

	struct OTPData
	{
		MINFO minfo;
		WB wb;
		uint8_t reserved[4];
		LSC lsc;

	};

#pragma pack(pop)

	class Database;
	class OtpCali_S5K3L8_MA83F : public SamsungOtp
	{
	public:
		OtpCali_S5K3L8_MA83F(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		int do_check_otp(void *args);

	};
}
