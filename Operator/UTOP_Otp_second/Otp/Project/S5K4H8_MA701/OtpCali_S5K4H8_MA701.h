#ifndef _OTPCALI_S5K4H8_MA701_H_
#define _OTPCALI_S5K4H8_MA701_H_

#include <stdint.h>
#include "../../SamsungOtp.h"


namespace OtpCali_S5K4H8_MA701
{
#define MTK_LSC_LEN 1868

#pragma pack(push, 1)
struct MINFO
{
    uint8_t flag;
    uint8_t mid;
    uint8_t ver;
    uint8_t y,m,d;
    uint8_t lens;
    uint8_t vcm;
    uint8_t vcmd;
    uint8_t platformid;
    uint8_t sn[4];
    uint8_t sum[2];
};
struct WB
{
    uint8_t flag;
    uint8_t r;
	uint8_t gr;
	uint8_t gb;
    uint8_t b;
	uint8_t r_g;
	uint8_t gr_g;
	uint8_t gb_g;
	uint8_t b_g;
    uint8_t sum[2];
};
struct AF
{
    uint8_t flag;
    uint8_t lens;
    uint8_t vcmdir;
    uint8_t inf[2];
    uint8_t mup[2];
    uint8_t sum[2];
};
struct LSC
{
    uint8_t flag;
    uint8_t lsc[MTK_LSC_LEN];
    uint8_t sum[3];
};

struct OTPData
{
    MINFO minfo;

    WB wb;

    AF af;

    LSC lsc;
};

#pragma pack(pop)

class Database;
class OtpCali_S5K4H8_MA701 : public SamsungOtp
{
public:
    OtpCali_S5K4H8_MA701(BaseDevice *dev);

private:
    int LscCali(void *args);
    int do_prog_otp();
    int get_otp_data_from_db(void *args);
    int get_otp_data_from_sensor(void *args);

    int get_minfo_from_db(void *args);
    int get_uniform_otp_data(void *in, void *out, int maxlen);
    int get_otp_group(void);

};
}


#endif
