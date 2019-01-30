#pragma once

#include <stdint.h>
#include "../../SamsungOtp.h"

namespace OtpCali_S5K3P9SX_MA841
{
#define QULCOMM_SPC_LEN 1030
#define QULCOMM_DCC_LEN 994
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)
	
	struct MINFO
	{	
		uint8_t flag;
		uint8_t version;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t moduleID;
		uint8_t lensID;
		uint8_t vcmID;
		uint8_t vcmDriverID;
		uint8_t colorTemp;
		uint8_t mirFlipFlag;
		uint8_t qrCode[4];
		uint8_t checkSum;
	};
	
	struct AWB
	{
		uint8_t flag;
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
		uint8_t rg_golden[2];
		uint8_t bg_golden[2];
		uint8_t gbgr_golden[2];
		uint8_t checkSum;
		uint8_t reserved[2]; 
	};

	struct LSC
	{	uint8_t flag;
		uint8_t lsc[QULCOMM_LSC_LEN];
		uint8_t checkSum;
		uint8_t reserved[246];
	};

	struct AF
	{
		uint8_t flag;
		uint8_t inf[2];
		uint8_t mup[2];
		uint8_t checkSum;
		uint8_t reserved[2];
	};

	struct PDAF_GainMap
	{
		uint8_t flag;
		uint8_t version[2];
		uint8_t gm_width[2];     // 17
		uint8_t gm_height[2];    // 13
		uint8_t gm_l[221*2];    // 13x17
		uint8_t gm_r[221*2];    // 13x17
		uint8_t checkSum;
		uint8_t reserved[4];
	};


	struct PDAF_DCC
	{
		uint8_t flag;
		uint8_t dcc_q_format[2];  
		uint8_t dcc_width[2];     // 8
		uint8_t dcc_height[2];    // 6
		uint8_t dcc[48*2];       // 6x8
		uint8_t checkSum;
		uint8_t reserved[16];
	};

	

	struct OTPData
	{
		MINFO minfo; 
		AWB awb;
		LSC lsc;
		AF af;
		PDAF_GainMap pdafGainmap;
		PDAF_DCC pdafDCC;
	};

#pragma pack(pop)

	class Database;
	class OtpCali_S5K3P9SX_MA841 : public SamsungOtp
	{
	public:
		OtpCali_S5K3P9SX_MA841(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		
		int DCC0Cali(void *args) ;
 		int do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len);
		int do_DCCCali(uint8_t *SPCTable,uint16_t **imagebuf, 
			int16_t *dcc_stack_lenspos, uint8_t out[]);

		int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, 
			int w, int h, uint8_t out[],uint8_t awbout[]);

		int do_qulcomm_DCCVerify_REV(uint8_t *DCCTable,uint16_t *img,int& DAC);
		int PDAFVerify(void *args);
	};
}

