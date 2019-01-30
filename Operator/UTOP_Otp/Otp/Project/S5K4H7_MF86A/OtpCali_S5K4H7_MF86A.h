#ifndef _OtpCali_S5K4H7_MF86A_H_
#define _OtpCali_S5K4H7_MF86A_H_

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K4H7_MF86A
{
#pragma pack(push, 1)
struct MINFO1
{
	uint8_t ModuleID_L;
	uint8_t ModuleID_H;
	uint8_t Day;
	uint8_t Month;
	uint8_t Year_L;
	uint8_t Year_H;
	uint8_t SensorID_L;
	uint8_t SensorID_H;
	uint8_t LensID_L;
	uint8_t LensID_H;
	uint8_t VCMID_L;
	uint8_t VCMID_H;
	uint8_t flag;	
    	uint8_t sum;
};

struct WB1
{
   	uint8_t R_avgL;
	uint8_t R_avgH;
	uint8_t Gr_avgL;
	uint8_t Gr_avgH;
	uint8_t Gb_avgL;
	uint8_t Gb_avgH;
	uint8_t B_avgL;
	uint8_t B_avgH;
	uint8_t GoldenR_avgL;
	uint8_t GoldenR_avgH;
	uint8_t GoldenGr_avgL;
	uint8_t GoldenGr_avgH;
	uint8_t GoldenGb_avgL;
	uint8_t GoldenGb_avgH;
	uint8_t GoldenB_avgL;
	uint8_t GoldenB_avgH;
	uint8_t R_G_L;
	uint8_t R_G_H;
	uint8_t B_G_L;
	uint8_t B_G_H;
    	uint8_t flag_wb;	
	uint8_t sum_wb;
	uint8_t flag_LightSource;	
	uint8_t sum_LightSource;
};

struct AF
{
	uint8_t Reserved[6];
	uint8_t flag;
	uint8_t sum;
};

struct SNINFO
{
	uint8_t sn[16];
	uint8_t flag;
	uint8_t sum;
};

struct LSC1
{
	uint8_t lsc[360];
};

struct LSC
{
	uint8_t lsc_flag;
	LSC1 lsc1;
	//LSC1 lsc2;
};

struct MINFO
{
	MINFO1 minfo1;
};

struct WB
{
	WB1 wb5100;
	WB1 wb4000;
	WB1 wb3100;
};
struct OTPData
{
	LSC lsc;
	MINFO minfo;
	WB wb;
	AF af;
	SNINFO sninfo;
};

#pragma pack(pop)

class Database;
class OtpCali_S5K4H7_MF86A : public SamsungOtp
{
public:
    OtpCali_S5K4H7_MF86A(BaseDevice *dev);

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
