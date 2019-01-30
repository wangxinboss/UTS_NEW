#pragma once

#include <stdint.h>
#include "../../OVOtp.h"
#include "Otp/Ref/LSC_Lib/OV/lc44.h"


#pragma comment(lib, "Otp/Ref/LSC_Lib/OV/lc44")


namespace OtpCali_OV13855_MA81H
{

#define	LSC_LEN_MA81H  186

#pragma pack(push, 1)

	struct OV13855_WB
	{
		uint8_t r_g_msb;
		uint8_t b_g_msb;
		uint8_t r_g_b_g_lsb;
	};
	struct MINFO_Group
	{
		uint8_t mid;
		uint8_t lens_id;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		OV13855_WB wb;
	};

	struct MINFO
	{
		uint8_t flag;
		MINFO_Group minfo1;
		MINFO_Group minfo2;
	};

	struct LSC_Group
	{
		uint8_t lsc[LSC_LEN_MA81H];
		uint8_t lsc_sum;
		uint8_t lsc_decode_sum;
		uint8_t total_sum;
	};
	struct LSC
	{
		uint8_t flag;
		LSC_Group lsc1;
		LSC_Group lsc2;
	};

	struct AF_Group
	{
		uint8_t inf;
		uint8_t mup;
		uint8_t vcmdir;
	};

	struct AF
	{
		uint8_t flag;
		AF_Group af1;
		AF_Group af2;
	};

	struct OTPData
	{
		MINFO minfo;
		LSC lsc;
		AF af;
	};

#pragma pack(pop)

	class Database;
	class OtpCali_OV13855_MA81H : public OVOtp
	{
	public:
		OtpCali_OV13855_MA81H(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		int do_check_otp();

		int get_wb_cali_data(WB_GAIN *gain, void *out);

	};
}
