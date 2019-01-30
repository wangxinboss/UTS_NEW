#ifndef _OTPCALI_IMX519_MA722_H_
#define _OTPCALI_IMX519_MA722_H_

#include <stdint.h>
#include "../../SonyOtp.h"

namespace OtpCali_IMX519_MA722
{

#define IMX_519_SPC_LEN 140
#define IMX_519_DCC_LEN 96
#define IMX_519_PD_LEN  48
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)

	struct MINFO
	{	
		uint8_t minfo_flag;
		uint8_t minfo_version;
	//	uint8_t module;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t supplier;
		uint8_t sensor_type;
		uint8_t lens;
		uint8_t vcm;
		uint8_t vcmdriver;
	//	uint8_t ir_bg;
	//	uint8_t mirror_flip;
		uint8_t reserved[21];
		uint8_t sum;
	};

	struct AF
	{
		uint8_t af_flag;   
		uint8_t inf[2];
		uint8_t mup[2];
		uint8_t reserved[10];
		uint8_t sum;
	};

	struct WB
	{
		uint8_t wb_flag;
		uint8_t wb_version;
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
	//	uint8_t rg_g[2];
	//	uint8_t bg_g[2];
	//	uint8_t gbgr_g[2];
		uint8_t reserved[12];
		uint8_t sum;
	};
	
	struct LSC
	{	
		uint8_t lsc_flag;
		uint8_t lsc_version;
		uint8_t lsc[QULCOMM_LSC_LEN];
		uint8_t sum;
	};

/*	struct PDAF   //for Qual
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
	};*/
	struct PDAF
	{
		uint8_t spc_flag;
		uint8_t dcc_flag;
		uint8_t spc_map[10*7*2];    // 10*7
		uint8_t dcc_map[48*2];           // 6x8
		uint8_t spc_sum;
		uint8_t dcc_sum;
	};
	struct OTPData
	{
		MINFO minfo; 
		WB wb;
		LSC lsc;
		AF af;
		PDAF pdaf;
	};



#pragma pack(pop)

	class Database;
	class OtpCali_IMX519_MA722 : public SonyOtp
	{
	public:
		OtpCali_IMX519_MA722(BaseDevice *dev);

	private:
		
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);
		int get_minfo_from_db(void *args);
		
		int LscCali(void *args);
		int SPCCali(void *args);
		int DCC0Cali(void *args) ;
		int PDAFVerify(void *args);

		int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[]);

		int do_SONY_SPCCali(unsigned short *pRaw10, short *out);
		int do_SONY_SPCVerify(unsigned short *pRaw10,int *judge);
		int do_SONY_DCCCali(int lens1, int lens2, int *pd1, int *pd2,unsigned short *dcc_out, int calc_mode);
		int do_SONY_DCCVerify(unsigned short*dcc, int *pd, int *dcc_out, int *pd_out ,int calc_mode);

		int do_WriteSPCSetting1();
		int do_WriteSPCSetting2();
		int do_WriteDCCSetting3();

		int do_SPC_writeback(uint8_t *regs);
		int do_SONY_GetPDTable(int *pdtable);

		int do_SONY_DCC_stats(int mode);

		int do_check_otp(void *args);
		//IMX519_PDAF

	//	virtual int Sony_PDAF_LRC (char* ini_file_path , short *pRaw10, short *pLRC){return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
	//	virtual int Sony_PDAF_LRC_Judge (char* file_ini_path , short *pRaw10, int *judge){return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
	//	virtual int Sony_PDAF_DCC (char *sensor, int lens1, int lens2, int *pd1, int *pd2, int h, int v, unsigned short *dcc_out, int calc_mode){return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
	//	virtual int Sony_PDAF_DCC_verify (char* sensor , int *dcc, int *pd, int h_num ,int v_num, int *dcc_out, int calc_mode){return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
	};
}


#endif
