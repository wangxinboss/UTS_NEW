#ifndef _OTPCALI_OV13855_MA82L_H_
#define _OTPCALI_OV13855_MA82L_H_

#include <stdint.h>
#include "../../OVOtp.h"

namespace OtpCali_OV13855_MA82L
{

#define QULCOMM_SPC_LEN 1030
#define QULCOMM_DCC_LEN 994
#define QULCOMM_LSC_LEN 1768

#pragma pack(push, 1)
	
	struct MINFO
	{	
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t moduleCode;
		uint8_t supplierCode;
		uint8_t moduleVer;
		uint8_t SN[30];
		uint8_t reserved;
		uint8_t checkSum[2];
	};
	struct CCT
	{
		uint8_t Ev[2];
		uint8_t cie_x[2];
		uint8_t cie_y[2];
		uint8_t checkSum[2];
	};


	struct AWB
	{
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
		uint8_t checkSum[2];
	};

	struct stationInfor
	{
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t ver;
		uint8_t number;
		uint8_t checkSum[2];
	};

	struct LSC
	{	
		uint8_t lsc[QULCOMM_LSC_LEN];
		uint8_t checkSum[2];
	};

	struct PDAF_DB
	{
		uint8_t version[2];
		uint8_t gm_width[2];     // 17
		uint8_t gm_height[2];    // 13
		uint8_t gm_l[221*2];    // 13x17
		uint8_t gm_r[221*2];    // 13x17
		uint8_t dcc_q_format[2];  
		uint8_t dcc_width[2];     // 8
		uint8_t dcc_height[2];    // 6
		uint8_t dcc[48*2];       // 6x8
		uint8_t checkSum[2];
	};

	struct AF
	{
		uint8_t reserved1[4];
		uint8_t inf[2];
		uint8_t mup[2];
		uint8_t reserved2[4];
		uint8_t checksum[2];
	};

	struct OTPData
	{
		MINFO minfo; 
		CCT cct;
		AWB awb;
		stationInfor awbststion;
		LSC lsc;
		stationInfor lscstation;
		uint8_t pdaf_flag;
		PDAF_DB pdaf;
		stationInfor pdafstation;
		AF af;
		stationInfor afstation;
		uint8_t reserved[4135];
		uint8_t totalchecksum[2];
	};

#pragma pack(pop)

	class Database;
	class OtpCali_OV13855_MA82L : public OVOtp
	{
	public:
		OtpCali_OV13855_MA82L(BaseDevice *dev);

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
	};
}


#endif
