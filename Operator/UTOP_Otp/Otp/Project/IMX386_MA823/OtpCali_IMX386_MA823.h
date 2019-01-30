#pragma once

#include <stdint.h>
#include "../../SonyOtp.h"

namespace OtpCali_IMX386_MA823
{

#define IMX386_SPC_LEN 96
#define IMX386_DCC_LEN 96
#define IMX386_PD_LEN  48
#define MTK_LSC_LEN    1868

#pragma pack(push, 1)

	struct MINFO
	{	
		uint8_t minfoFlag;
		uint8_t ModuleID;
		uint8_t LensID;
		uint8_t VCMID;
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t checkSum;
	};

	struct WB
	{
		uint8_t wbFlag;
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
		uint8_t checkSum;
	};

	struct AF
	{
		uint8_t afFlag;  
		uint8_t mup[2];
		uint8_t inf[2];
		uint8_t checkSum;
	};

	
	
	struct LSC
	{	
		uint8_t lscFlag;
		uint8_t lsc[MTK_LSC_LEN];
		uint8_t checkSum;
	};

	struct PDAF
	{
		uint8_t pdafFlag;
		uint8_t spc_map[IMX386_SPC_LEN];    // 8*6*2
		uint8_t dcc_map[IMX386_DCC_LEN];    // 6x8
		uint8_t checkSum;
	};
	struct OTPData
	{
		MINFO minfo; 
		WB wb;
		AF af;
		LSC lsc;
		PDAF pdaf;
	};



#pragma pack(pop)

	class Database;
	class OtpCali_IMX386_MA823 : public SonyOtp
	{
	public:
		OtpCali_IMX386_MA823(BaseDevice *dev);

	private:
		
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);
		int get_minfo_from_db(void *args);
		
		int LscCali(void *args);
		int SPCCali(void *args);
		int DCC0Cali(void *args) ;
		int PDAFVerify(void *args);


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
	};
}

