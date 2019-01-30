#ifndef _OTPCALI_S5L3L8_MA706_H_
#define _OTPCALI_S5L3L8_MA706_H_

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K3L8_MA706
{

#define QULCOMM_SPC_LEN 1030
#define QULCOMM_DCC_LEN 994
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)
	
	struct MINFO
	{	
		uint8_t module;
		uint8_t lens;
		uint8_t vcm;
		uint8_t vcmdriver;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t sum;
		uint8_t reserved[3];
	};

	struct AF
	{
		uint8_t mup[2];
		uint8_t inf[2];
		uint8_t start[2];
		uint8_t sum;
		uint8_t reserved[6];
	};

	struct AWB
	{
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
		uint8_t rg_g[2];
		uint8_t bg_g[2];
		uint8_t gbgr_g[2];
		uint8_t sum;
	};

	struct LSC
	{	
		uint8_t lsc[QULCOMM_LSC_LEN];
		uint8_t sum;
	};

	struct PDAF_GM
	{
		uint8_t version[2];
		uint8_t gm_width[2];     // 17
		uint8_t gm_height[2];    // 13
		uint8_t gm_l[221*2];    // 13x17
		uint8_t gm_r[221*2];    // 13x17
		uint8_t sum;
	};
	struct PDAF_DCC
	{
		uint8_t dcc_q_format[2];  
		uint8_t dcc_width[2];     // 8
		uint8_t dcc_height[2];    // 6
		uint8_t dcc[48*2];       // 6x8
		uint8_t sum;
		uint8_t reserved[2556];		//0x0AF6~0x14F1
	};

	struct OTPData
	{
		uint8_t minfo_flag;
		MINFO minfo; 
		uint8_t af_flag;
		AF af;
		uint8_t awb_flag;
		AWB awb;
		uint8_t lsc_flag;
		LSC lsc;
		uint8_t pdaf_gm_flag;
		PDAF_GM q_pdaf_gm;
		uint8_t pdaf_dcc_flag;
		PDAF_DCC q_pdaf_dcc;
	};




#pragma pack(pop)

	class Database;
	class OtpCali_S5K3L8_MA706 : public SamsungOtp
	{
	public:
		OtpCali_S5K3L8_MA706(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		
		int DCC0Cali(void *args) ;
	//	int PDAFVerify(void *args);
 		int do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len);
		int do_DCCCali(uint8_t *SPCTable,uint16_t **imagebuf, 
			int16_t *dcc_stack_lenspos, uint8_t out[]);

		int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, 
			int w, int h, uint8_t out[],uint8_t awbout[]);
		//bool do_qulcomm_DCCVerify_REV(uint8_t *DCCTable,uint16_t *img);
	};
}


#endif
