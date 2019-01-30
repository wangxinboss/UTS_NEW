#ifndef _OTPCALI_S5K3P9SP_MF81P_H_
#define _OTPCALI_S5K3P9SP_MF81P_H_

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K3P9SP_MF81P
{
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)
	
	struct MINFO
	{	
		uint8_t flag;
		uint8_t storage_loc;
		uint8_t smartisan_pn[12];
		uint8_t module;
		uint8_t sensor;
		uint8_t lens;
		uint8_t vcm;
		uint8_t vcmdriver;
		uint8_t ir_bg;
		uint8_t gyro;
		uint8_t color_temp;
		uint8_t oisdriver;
		uint8_t fpc_version;
		uint8_t cal_verison;
		uint8_t ois_fw_version;
		uint8_t serial_code[16];
		uint8_t project_sku;
		uint8_t product_fac;
		uint8_t product_line;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t minute;
		uint8_t reserved[2];
		uint8_t sum[2];
	};

	struct AF
	{
		uint8_t flag;
		uint8_t reserved[13];
	};

	struct AWB
	{
		uint8_t flag;
		uint8_t equip_num;
		uint8_t ratio_5100k_r_gr[2];
		uint8_t ratio_5100k_b_gr[2];
		uint8_t ratio_5100k_gb_gr[2];

		uint8_t _5100kr_gr[2];
		uint8_t _5100kb_gr[2];
		uint8_t _5100kgb_gr[2];

		uint8_t reseved[12];

		uint8_t ratio_3100k_r_gr[2];
		uint8_t ratio_3100k_b_gr[2];
		uint8_t ratio_3100k_gb_gr[2];

		uint8_t _3100kr_gr[2];
		uint8_t _3100kb_gr[2];
		uint8_t _3100kgb_gr[2];

		uint8_t sum[2];
	};

	struct LSC
	{	
		uint8_t flag;
		uint8_t equip_num;
		uint8_t lsc[1105];
		uint8_t r_max[2];
		uint8_t gr_max[2];
		uint8_t gb_max[2];
		uint8_t b_max[2];
		uint8_t oc_x[2];
		uint8_t oc_y[2];

		uint8_t sum[2];
	};

	struct MTF
	{
		uint8_t flag;
		uint8_t equip_num;
		uint8_t center_hor[2];
		uint8_t center_ver[2];     // 

		uint8_t fov_4_lu_hor[2];    // 
		uint8_t fov_4_lu_ver[2];    // 
		uint8_t fov_4_ru_hor[2];    // 
		uint8_t fov_4_ru_ver[2];    // 
		uint8_t fov_4_ld_hor[2];    // 
		uint8_t fov_4_ld_ver[2];    // 
		uint8_t fov_4_rd_hor[2];    // 
		uint8_t fov_4_rd_ver[2];    //

		uint8_t fov_6_lu_hor[2];    // 
		uint8_t fov_6_lu_ver[2];    // 
		uint8_t fov_6_ru_hor[2];    // 
		uint8_t fov_6_ru_ver[2];    // 
		uint8_t fov_6_ld_hor[2];    // 
		uint8_t fov_6_ld_ver[2];    // 
		uint8_t fov_6_rd_hor[2];    // 
		uint8_t fov_6_rd_ver[2];    //

		uint8_t fov_8_lu_hor[2];    // 
		uint8_t fov_8_lu_ver[2];    // 
		uint8_t fov_8_ru_hor[2];    // 
		uint8_t fov_8_ru_ver[2];    // 
		uint8_t fov_8_ld_hor[2];    // 
		uint8_t fov_8_ld_ver[2];    // 
		uint8_t fov_8_rd_hor[2];    // 
		uint8_t fov_8_rd_ver[2];    //

		uint8_t sum[2];

	};
	struct Cross_Talk
	{
		uint8_t flag; 
		uint8_t equip_num;     // 8
		uint8_t cross_talk[2048];    // 6
		uint8_t sum[2];
	};

	struct OTPData
	{
		MINFO minfo; 
		AF af;
		AWB awb;
		LSC lsc;
        uint8_t reserved[165];
		uint8_t total_sum[2];
		Cross_Talk cross_talk;
	};




#pragma pack(pop)

	class Database;
	class OtpCali_S5K3P9SP_MF81P : public SamsungOtp
	{
	public:
		OtpCali_S5K3P9SP_MF81P(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		
		int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, 
			int w, int h, uint8_t out[],uint8_t awbout[]);

	};
}


#endif
