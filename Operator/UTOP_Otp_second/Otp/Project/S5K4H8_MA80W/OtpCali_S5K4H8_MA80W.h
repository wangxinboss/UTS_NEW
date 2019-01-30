#ifndef _OTPCALI_S5K4H8_MA80W_H_
#define _OTPCALI_S5K4H8_MA80W_H_

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K4H8_MA80W
{
#pragma pack(push, 1)
struct WB1
{
	uint8_t r_g_msb;
	uint8_t b_g_msb;
	uint8_t gb_gr_msb;
	uint8_t awb_lsb;
};
struct MINFO1
{
	uint8_t mid;
	uint8_t lens_id;
	uint8_t vcm_id;
	WB1 wb;
	uint8_t infosum;
};
struct AF1
{
	uint8_t inf_msb;
	uint8_t mup_msb;
	uint8_t af_lsb;
	uint8_t sum;
};
struct LSC1
{
	uint8_t flag;
	uint8_t lsc[360];
};
struct LSC
{
	LSC1 lsc1;
	LSC1 lsc2;
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
struct LSCSum
{
	uint8_t lscsum1;
	uint8_t lscsum2;
};
struct OTPData
{
	uint8_t Reserve1[46];     //46
	LSC lsc;                  //361*2
	MINFO minfo;              //17
	AF af;                    //9
	LSCSum lscsum;	          //2
	uint8_t Reserve2[36];     //36
};

#pragma pack(pop)

class Database;
class OtpCali_S5K4H8_MA80W : public SamsungOtp
{
public:
    OtpCali_S5K4H8_MA80W(BaseDevice *dev);

private:
	int LscCali(void *args);
	int do_prog_otp();
	int do_prog_otp_lsc();
    int get_otp_data_from_db(void *args);
    int get_otp_data_from_sensor(void *args);

    int get_minfo_from_db(void *args);
    int get_uniform_otp_data(void *in, void *out, int maxlen);
    int get_otp_group(void);

	int sram_lsc_witeback(void *args);
};
}


#endif
