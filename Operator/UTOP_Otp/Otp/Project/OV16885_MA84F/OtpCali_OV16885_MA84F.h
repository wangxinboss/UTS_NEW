#pragma once


#include <stdint.h>
#include "../../OVOtp.h"

namespace OtpCali_OV16885_MA84F
{

#define QULCOMM_SPC_LEN 1030
#define QULCOMM_DCC_LEN 994
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)
	
	struct MINFO
	{	
		uint8_t mInfoFlag;
		uint8_t moduleID;
		uint8_t LensID;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t checkSum;
	};
	

	struct AWB
	{
		uint8_t awbFlag;
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
		uint8_t checkSum;
	};

	struct AF
	{
		uint8_t afFlag;
		uint8_t afCaliDir;
		uint8_t inf[2];
		uint8_t mup[2];
		uint8_t checkSum;
	};

	struct LSC
	{	
		uint8_t lscFlag;
		uint8_t lsc[QULCOMM_LSC_LEN];
		uint8_t checkSum;
	};

	struct PDAF_GainMap
	{
		uint8_t gmFlag;
		uint8_t version[2];
		uint8_t gm_width[2];     // 17
		uint8_t gm_height[2];    // 13
		uint8_t gm_l[221*2];    // 13x17
		uint8_t gm_r[221*2];    // 13x17
		uint8_t checkSum;
	};

	struct PDAF_DCC
	{
		uint8_t dccFlag;
		uint8_t dcc_q_format[2];  
		uint8_t dcc_width[2];     // 8
		uint8_t dcc_height[2];    // 6
		uint8_t dcc[48*2];       // 6x8
		uint8_t checkSum;
	};

	

	struct OTPData
	{
		MINFO minfo; 
		AWB awb;
		AF af;
		LSC lsc;
		PDAF_GainMap pdafGM;
		PDAF_DCC   pdafDCC;
	};

#pragma pack(pop)

	class Database;
	class OtpCali_OV16885_MA84F : public OVOtp
	{
	public:
		OtpCali_OV16885_MA84F(BaseDevice *dev);

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

		int do_qulcomm_DCCVerify_REV(uint8_t *DCCTable,uint16_t *img,int& DAC);
		int PDAFVerify(void *args);

		int get_wb_cali_data(WB_GAIN *gain, void *out);
	};
}



