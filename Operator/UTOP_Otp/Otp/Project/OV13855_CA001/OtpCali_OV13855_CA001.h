#ifndef _OTPCALI_IMX350_CA225_H_
#define _OTPCALI_IMX350_CA225_H_

#include <stdint.h>
#include "../SonyOtp.h"

namespace OtpCali_IMX350_CA225
{

#define	MTK_LSC_LEN_CA225  1868

#pragma pack(push, 1)

struct MINFO
{	
	uint8_t flag;
	uint8_t caliVer[4];
	uint8_t sn[2];
	uint8_t awbafinfo[2];
};

struct WB
{
	uint8_t r;
	uint8_t gr;
	uint8_t gb;
	uint8_t b;
	uint8_t Golden_r;
	uint8_t Golden_gr;
	uint8_t Golden_gb;
	uint8_t Golden_b;
};

struct AF
{
	uint8_t inf[2];
	uint8_t mup[2];
};

struct LSC
{
	uint8_t lsclength[2];
	uint8_t lscbuffer[MTK_LSC_LEN_CA225];
};

struct PDAF
{
	uint8_t spc[96];
	uint8_t dcc[192];
	uint8_t pdaf_empty[1760];
};

struct Hall
{
	uint8_t hallmin[2];
	uint8_t hallmax[2];
	uint8_t halloffset;
	uint8_t hallgain;
};

struct AF_Distance
{
	uint8_t distance_inf[2];
	uint8_t distance_mup[2];
};

struct AF_Temperature
{
	uint8_t temperature_inf;
	uint8_t temperature_mup;
	uint8_t warm_inf[2];
	uint8_t warm_mup[2];
	uint8_t warm_temperature_inf;
	uint8_t warm_temperature_mup;

	uint8_t middle[2];
	uint8_t temperature_middle;
	uint8_t warm_middle[2];
	uint8_t warm_temperature_middle;
};

struct Reserve
{
	uint8_t reserve_empty[40];
};

struct BackecdInfo
{
	uint8_t mtkver;
	uint8_t mtkflag[3];
	uint8_t year;
	uint8_t month;
	uint8_t day;
};



struct OTPData
{
	MINFO minfo;    
	WB wb;
	AF af;
	LSC lsc;
	PDAF pdaf;
	Hall hall;
	AF_Distance af_dis;
	AF_Temperature af_temperature;
	Reserve reserve;
	BackecdInfo mtkinfo;

	uint8_t checksum[4];
};


#pragma pack(pop)

	class Database;
	class OtpCali_IMX350_CA225 : public SonyOtp
	{
	public:
		OtpCali_IMX350_CA225(BaseDevice *dev);

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
