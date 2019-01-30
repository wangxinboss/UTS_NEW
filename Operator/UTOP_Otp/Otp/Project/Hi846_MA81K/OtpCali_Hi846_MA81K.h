#pragma once

#include <stdint.h>
#include "../../HiOtp.h"


namespace OtpCali_Hi846_MA81K
{
#define	MTK_LSC_LEN_MA81K  858
#pragma pack(push, 1)

	struct MINFO_Group
	{	
		uint8_t moudleID;
		uint8_t afID;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t sensorID;
		uint8_t lensID;
		uint8_t vcmID;
		uint8_t DriverICID;
		uint8_t FnumberID;
		uint8_t resert[6];
		uint8_t mInfoCheckSum;		
	};

	struct MINFO
	{
		MINFO_Group mInfoGroup1;
		MINFO_Group mInfoGroup2;
		MINFO_Group mInfoGroup3;
	};

	struct LSC_Group
	{
		uint8_t lscbuffer[MTK_LSC_LEN_MA81K];
		uint8_t resert[8];
		uint8_t lscchecksum;
	};

	struct LSC
	{
		LSC_Group LSCGroup1;
		LSC_Group LSCGroup2;
		LSC_Group LSCGroup3;
	};

	struct Hi846_WB_Group
	{
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
		uint8_t rg_g[2];
		uint8_t bg_g[2];
		uint8_t gbgr_g[2];
		uint8_t r[2];
		uint8_t b[2];
		uint8_t gr[2];
		uint8_t gb[2];
		uint8_t r_g[2];
		uint8_t b_g[2];
		uint8_t gr_g[2];
		uint8_t gb_g[2];
		uint8_t reserved;
		uint8_t wb_CheckSum;
	};

	struct Hi846_WB
	{
		Hi846_WB_Group wbGroup1;
		Hi846_WB_Group wbGroup2;
		Hi846_WB_Group wbGroup3;

	};

	struct Hi846_AF_Group
	{
		uint8_t Af_Cal;
		uint8_t Af_Inf[2];
		uint8_t Af_Mup[2];
		uint8_t afchecksum;
	};
	struct Hi846_AF
	{
		Hi846_AF_Group afGroup1;
		Hi846_AF_Group afGroup2;
		Hi846_AF_Group afGroup3;
	};

	struct OTPData
	{
		uint8_t mInfoFlag;
		MINFO mInfo;
		uint8_t lscFlag;
		LSC   lsc;
		uint8_t wb_flag;
		Hi846_WB wb;
	};


#pragma pack(pop)

	class Database;
	class OtpCali_Hi846_MA81K : public HiOtp
	{
	public:
		OtpCali_Hi846_MA81K(BaseDevice *dev);

	private:
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int LscCali(void *args);
		int get_minfo_from_db(void *args);

		int do_check_otp(void *args);
	};
}

