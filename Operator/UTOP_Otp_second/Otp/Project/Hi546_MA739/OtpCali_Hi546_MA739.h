#ifndef _OTPCALI_Hi546_MA739_H_
#define _OTPCALI_Hi546_MA739_H_

#include <stdint.h>
#include "../../HynixOtp.h"

namespace OtpCali_Hi546_MA739
{
#pragma pack(push, 1)

struct MINFO1
{
	uint8_t mid;
	uint8_t af_flag;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t sensor_id;
	uint8_t lens_id;
	uint8_t vcm_id;
	uint8_t driveric_id;
	uint8_t f_number_id;
	uint8_t reserved[6];
	uint8_t sum;
};
struct WB1
{
	uint8_t rg[2];
	uint8_t bg[2];
	uint8_t gg[2];
	uint8_t g_rg[2];
	uint8_t g_bg[2];
	uint8_t g_gg[2];
	uint8_t reserved[17];
	uint8_t sum;
};
struct AF1
{
	uint8_t direction;
	uint8_t inf[2];
	uint8_t mup[2];
	uint8_t reserved[5];
	uint8_t sum;
};

struct MINFO
{
	uint8_t flag;
	MINFO1 minfo1;
	MINFO1 minfo2;
	MINFO1 minfo3;
};
struct AF
{
	uint8_t flag;
	AF1 af1;
	AF1 af2;
	AF1 af3;
};
struct WB
{
	uint8_t flag;
	WB1 wb1;
	WB1 wb2;
	WB1 wb3;
};
struct OTPData
{
	MINFO minfo;
	WB wb;
	AF af;
};

#pragma pack(pop)

class Database;
class OtpCali_Hi546_MA739 : public HynixOtp
{
public:
    OtpCali_Hi546_MA739(BaseDevice *dev);

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
