#ifndef _OTPCALI_GC8024_MF81B_H_
#define _OTPCALI_GC8024_MF81B_H_

#include <stdint.h>
#include "../../GCOtp.h"

namespace OtpCali_GC8024_MF81B
{
#pragma pack(push, 1)
struct WB1
{
	uint8_t r_g;
	uint8_t b_g;
};
struct MINFO1
{
	uint8_t mid;
	uint8_t lens_id;
	uint8_t year;
	uint8_t month;
	uint8_t day;
};

struct MINFO
{
	MINFO1 minfo1;
	MINFO1 minfo2;
};

struct WB
{
	WB1 wb1;
	WB1 wb2;
};


struct OTPData
{
	uint8_t basic_flag1;
	WB wb;
	uint8_t basic_flag2;
	MINFO minfo;
//	uint8_t version_flag;
};

#pragma pack(pop)

class Database;
class OtpCali_GC8024_MF81B : public GCOtp
{
public:
    OtpCali_GC8024_MF81B(BaseDevice *dev);

private:
	int LscCali(void *args);
	int do_prog_otp();
	int do_prog_otp_lsc();
    int get_otp_data_from_db(void *args);
    int get_otp_data_from_sensor(void *args);

    int get_minfo_from_db(void *args);
    int get_uniform_otp_data(void *in, void *out, int maxlen);
    int get_otp_group(void);

};
}


#endif
