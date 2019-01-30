#ifndef _OTPCALI_IMX362_MA702_H_
#define _OTPCALI_IMX362_MA702_H_

#include <stdint.h>
#include "../../SonyOtp.h"

namespace OtpCali_IMX362_MA702
{

#define QULCOMM_SPC_LEN 1030
#define QULCOMM_DCC_LEN 994
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)

	struct MINFO
	{	
		uint8_t minfo_flag;
		uint8_t module;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t supplier;
		uint8_t sensor_type;
		uint8_t lens;
		uint8_t vcm;
		uint8_t vcmdriver;
		uint8_t ir_bg;
		uint8_t mirror_flip;
		uint8_t reserved[2];
		uint8_t sum[2];
	};

	struct AF
	{
		uint8_t af_flag;   
		uint8_t inf[2];
		uint8_t mup[2];
		uint8_t reserved[6];
		uint8_t sum[2];
	};

	struct WB
	{
		uint8_t wb_flag;
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
		uint8_t rg_g[2];
		uint8_t bg_g[2];
		uint8_t gbgr_g[2];
		uint8_t reserved[2];
		uint8_t sum[2];
	};
	
	struct LSC
	{	
		uint8_t lsc_flag;
		//uint8_t lscwidth;
		//uint8_t lscheight;
		uint8_t lsc[QULCOMM_LSC_LEN];
		uint8_t sum[2];
	};

	struct PDAF
	{
		uint8_t pdaf_flag;
		uint8_t cal_version[2];
		uint8_t gm_width[2];     // 17
		uint8_t gm_height[2];    // 13
		uint8_t gm_l[221*2];    // 13x17
		uint8_t gm_r[221*2];    // 13x17
		uint8_t gm_sum[2];
		uint8_t dm_q_format[2];  
		uint8_t dm_width[2];     // 8
		uint8_t dm_height[2];    // 6
		uint8_t dm[48*2];       // 6x8
		uint8_t dm_sum[2];
	};

	struct OTPData
	{
		MINFO minfo; 
		AF af;
		uint8_t reserved[25];
		WB wb;
		LSC lsc;
		PDAF pdaf;
	};



#pragma pack(pop)

	class Database;
	class OtpCali_IMX362_MA702 : public SonyOtp
	{
	public:
		OtpCali_IMX362_MA702(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		
		int DCC0Cali(void *args) ;
		int PDAFVerify(void *args);
 		int do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len);
		int do_DCCCali(uint8_t *SPCTable,uint16_t **imagebuf, 
			int16_t *dcc_stack_lenspos, uint8_t out[]);

		int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, 
			int w, int h, uint8_t out[],uint8_t awbout[]);

		int SPCCali(void *args);

		bool do_qulcomm_DCCVerify_REV(uint8_t *DCCTable,uint16_t *img);
	};
}


#endif
