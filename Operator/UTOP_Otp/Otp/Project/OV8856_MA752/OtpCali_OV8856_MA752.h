#ifndef _OTPCALI_OV8856_MA752_H_
#define _OTPCALI_OV8856_MA752_H_

#include <stdint.h>
#include "../../OVOtp.h"
#include "Otp/LSC.h"
#include "Otp/Ref/LSC_Lib/OV/lc43.h"

#pragma comment(lib, "Otp/Ref/LSC_Lib/OV/lc43")

namespace OtpCali_OV8856_MA752
{
#pragma pack(push, 1)

struct WB1
{
	uint8_t r_g_msb;
	uint8_t b_g_msb;
	uint8_t r_g_b_g_lsb;
};
struct MINFO1
{
    uint8_t mid;
    uint8_t lens_id;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	WB1 wb;
};

struct MINFO
{
	uint8_t flag;
	MINFO1 minfo1;
	MINFO1 minfo2;
};

struct LSC1
{
	uint8_t lsc[240];
	uint8_t sum;
};
struct LSC
{
	uint8_t flag;
	LSC1 lsc1;
	LSC1 lsc2;
};

struct AF1
{
	uint8_t inf;
	uint8_t mup;
	uint8_t vcmdir;
};

struct AF
{
	uint8_t flag;
	AF1 af1;
	AF1 af2;
	/*uint8_t Reserve[7];*/
};

struct OTPData
{
	MINFO minfo;
	AF af;
	LSC lsc;
};

#pragma pack(pop)

class Database;
class OtpCali_OV8856_MA752 : public OVOtp
{
public:
    OtpCali_OV8856_MA752(BaseDevice *dev);

private:
	int LscCali(void *args);
	int do_prog_otp();
	int do_prog_otp_lsc();
    int get_otp_data_from_db(void *args);
    int get_otp_data_from_sensor(void *args);

    int get_minfo_from_db(void *args);
    int get_uniform_otp_data(void *in, void *out, int maxlen);
    int get_otp_group(void);

	int do_check_otp(void *args);
};
}


#endif
