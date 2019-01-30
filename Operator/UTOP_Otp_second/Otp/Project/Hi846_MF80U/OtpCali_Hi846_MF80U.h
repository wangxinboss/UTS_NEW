#ifndef _OTPCALI_Hi846_MF80U_H_
#define _OTPCALI_Hi846_MF80U_H_

#include <stdint.h>
#include "../../HynixOtp.h"


namespace OtpCali_Hi846_MF80U
{
#define MTK_LSC_LEN 1868

#pragma pack(push, 1)
struct MINFO
{
    uint8_t flag;
    uint8_t ver[4];
	uint8_t y,m,d;
	uint8_t mid;
    uint8_t lensid;
    uint8_t vcmid;
    uint8_t drvericid;
    uint8_t eepromid;
	uint8_t color_temprature;
    uint8_t bit_enable;  // bit0:AWB  bit1:AF  bit2:AF_inf  bit3:AF_Macro        
	uint8_t reserved[6];
    uint8_t sum;
	uint8_t reserved2[42];
};
struct WB
{
    uint8_t flag;
	uint8_t rg[2];
	uint8_t bg[2];
	uint8_t grgb[2];
	uint8_t rg_g[2];
	uint8_t bg_g[2];
	uint8_t grgb_g[2];
    uint8_t r;
	uint8_t gr;
	uint8_t gb;
    uint8_t b;
	uint8_t r_g;
	uint8_t gr_g;
	uint8_t gb_g;
	uint8_t b_g;
    uint8_t sum;
	uint8_t reserved[42];
};

struct LSC
{
    uint8_t flag;
    uint8_t lsc[MTK_LSC_LEN];
    uint8_t sum;
	uint8_t reserved[18];
};

struct OTPData
{
    MINFO minfo;
    WB wb;
    LSC lsc;
};

#pragma pack(pop)

class Database;
class OtpCali_Hi846_MF80U : public HynixOtp
{
public:
    OtpCali_Hi846_MF80U(BaseDevice *dev);

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
