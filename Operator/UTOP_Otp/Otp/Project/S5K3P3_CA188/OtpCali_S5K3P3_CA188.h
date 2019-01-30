#ifndef _OTPCALI_S5K3P3_CA188_H_
#define _OTPCALI_S5K3P3_CA188_H_

#include <stdint.h>
#include "../../SamsungOtp.h"


namespace OtpCali_S5K3P3_CA188
{
#define QULCOMM_LSC_LEN 1768


#pragma pack(push, 1)

	struct MINFO
	{
		uint8_t lsc_ver;
		uint8_t pdaf_ver;
		uint8_t vendor_id;
		uint8_t lens_id;
		uint8_t vcm_id;
		uint8_t vcmdriver_id;
		uint8_t otpmapping_id;
		uint8_t sum_info;
		uint8_t sn[16];
		uint8_t sum_sn;
	};
	struct WB
	{
		uint8_t rg[2];
		uint8_t bg[2];
		uint8_t gbgr[2];
		uint8_t revert[2];
	};
	struct AF
	{
		uint8_t start[2];
		uint8_t mup[2];
		uint8_t inf[2];
	};
	struct LSC
	{	
		uint8_t lscwidth;
		uint8_t lscheight;
		uint8_t lsc[QULCOMM_LSC_LEN];
	};

	
	struct PDAF
	{
		uint8_t offsetX[2];
		uint8_t offsetY[2];
		uint8_t ratioX[2];
		uint8_t ratioY[2];
		uint8_t mapWidth[2];
		uint8_t mapHeight[2];

		uint8_t leftgain[2048];
		uint8_t rightgain[2048];	
		uint8_t pd[2];
	};

	struct OTPData
	{
		MINFO minfo;

		AF af;

		WB wb;

		LSC lsc;

		PDAF pdaf;

		uint8_t checksum[2];
	};

#pragma pack(pop)

	class Database;
	class OtpCali_S5K3P3_CA188 : public SamsungOtp
	{
	public:
		OtpCali_S5K3P3_CA188(BaseDevice *dev);

	private:
		int LscCali(void *args);
		int do_prog_otp();
		int get_otp_data_from_db(void *args);
		int get_otp_data_from_sensor(void *args);

		int get_minfo_from_db(void *args);
		int get_uniform_otp_data(void *in, void *out, int maxlen);
		int get_otp_group(void);

		int do_hallCali();
		int do_SetgyroX(double gain);
		int do_SetgyroY(double gain);
		int do_OISON();
		int do_OISOFF();

		int do_SaveGyroGainData(double gainX,double gainY);
		int do_LoadFW(CString FWFilePath);
		int do_CheckFW(int FWVer);
		int do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len);
		int DCC0Cali(void *args) ;
		int DCC1Cali(void *args) ;
		int PDAFVerify(void *args);
		int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, 
								   int w, int h, uint8_t out[],uint8_t awbout[]);
		int do_qualcomm_DCCCali(uint8_t *SPCTable,uint16_t *raw_negative, 
								uint16_t *raw_positive, int Width, int Height, 
								int dac_negative, int dac_positive);
		bool do_qulcomm_DCCVerify(uint8_t *SPCTable,uint16_t *img,
			int w, int h,int PD_ConversionCoeff,
			int ToleranceError,
			int VerifyDAC,int solbelfocusdac,
			int &PDAFDAC,
			double &VerifyError);

		//Ryan@20160506
		int do_SetSltPos( unsigned int UcPos );
		int do_SetVrtPos( unsigned int UcPos );
		int do_SetHrzPos( unsigned int UcPos );
		int do_GetGyroGainX(UINT32 *Gain);
		int do_GetGyroGainY(UINT32 *Gain);

	};

	
}


#endif
