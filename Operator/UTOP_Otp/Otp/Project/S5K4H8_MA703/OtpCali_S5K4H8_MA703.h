#ifndef _OTPCALI_S5K4H8_MA703_H_
#define _OTPCALI_S5K4H8_MA703_H_

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K4H8_MA703
{
#pragma pack(push, 1)
struct MINFO1
{
    uint8_t mid;
	uint8_t year;
	uint8_t month;
	uint8_t day;
    uint8_t lens_id;
    uint8_t vcm_id;
    uint8_t vcmdriver_id;
    uint8_t ir;
    uint8_t etc;
    uint8_t sum;
};
struct AF1
{
	uint8_t vcmdir;
	uint8_t inf[2];
	uint8_t mup[2];
	uint8_t sum;
};
struct WB1
{
    uint8_t r_g[2];
    uint8_t b_g[2];
    uint8_t gr_gb[2];
    uint8_t r_g_g[2];
    uint8_t b_g_g[2];
    uint8_t gr_gb_g[2];
    uint8_t sum;
};
struct LSC
{
	uint8_t lsc[360];
};
struct MINFO
{
	uint8_t flag;
	MINFO1 minfo1;
	MINFO1 minfo2;
};
struct AF
{
	uint8_t flag;
	AF1 af1;
	AF1 af2;
};
struct WB
{
	uint8_t flag;
	WB1 wb1;
	WB1 wb2;
};
struct OTPData
{
	MINFO minfo;
	AF af;
	WB wb;
	LSC lsc;
};

#pragma pack(pop)

class Database;
class OtpCali_S5K4H8_MA703 : public SamsungOtp
{
public:
    OtpCali_S5K4H8_MA703(BaseDevice *dev);

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
