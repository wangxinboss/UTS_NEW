#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "SensorDriver.h"
#include "OISControl.h"
#include "EEProm.h"
#include "CommonFunc.h"
#include "otpdb.h"
#include "BaseOperator.h"


using namespace UTS;
using namespace DDM;
class OtpDB;

#define SIZE_K  1024

//--- NVM Error ---//
#define NVM_FW    5
#define NVM_DATA_CONSIST    9
#define NVM_WRITE           10
#define NVM_CHECK           11
#define NVM_HALLCal         25
#define NVM_DB_CONNECT      43
#define NVM_BURNED_BEFORE   44
#define NVM_NOINFO          45
#define NVM_LSC             55
#define NVM_WB              56
#define NVM_BEGOLDEN        57
#define AE_TIMEOUT          58
#define ERR_QULCOMMLSC      59
#define ERR_SPC             60
#define ERR_DCC             61
#define ERR_AUTO_DEV        62
#define NVM_OTP_TEST_OMIT   63
#define NVM_NO_PROG_HISTORY 64
#define ERR_HALLCALI        65
#define ERR_GYROCALI        66
#define ERR_OISCHECK        67

enum OTP_ITEM_ATTR
{
    OTP_ATTR_LT = (1 << 0),
    OTP_ATTR_UP = (1 << 1),
};


class BaseOtp;
typedef int (BaseOtp::*otpfunc)(void *args);
struct OTP_OPTION
{
	int opt;
	const TCHAR *name;
	const TCHAR *section;
	otpfunc action;
	uint32_t attr;
};

struct ROI {
    int x;
    int y;
    int width;
    int height;
};

enum OtpCaliOpt
{
	OtpCaliOpt_ProgOtp,
	OtpCaliOpt_ChkOtp,
	OtpCaliOpt_LscCali,
	OtpCaliOpt_WBCali,
	OtpCaliOpt_TestEEPROM,
	OtpCaliOpt_TestOtpLife,
	OtpCaliOpt_LOADLSC,
	OtpCaliOpt_LOADWB,
	OtpCaliOpt_DUMP,
	OtpCaliOpt_SPC,
	OtpCaliOpt_DCC0,
	OtpCaliOpt_DCC1,
	OtpCaliOpt_ChkHis,
	OtpCaliOpt_ResetEeprom,
	OtpCaliOpt_OffChk,
	OtpCaliOpt_ProgSN,
	OtpCaliOpt_GoldenWB,
	OtpCaliOpt_PDAFVerify,
	OtpCaliOpt_ReProg,
	OtpCaliOpt_LOADFW,
	OtpCaliOpt_CHECKFW,
	OtpCaliOpt_HallCali,
	OtpCaliOpt_GyroCali,
	OtpCaliOpt_OISCheck,
	OtpCaliOpt_SENSORSPC,
	OtpCaliOpt_GyroCaliNonShaker,
	OtpCaliOpt_WBRAW10Cali,
	OtpCaliOpt_SRAMLOADLSC,
	OtpCaliOpt_Num
};

enum OtpCali_ErrorType
{
    OTPCALI_ERROR_NO,
    OTPCALI_ERROR_DB,
    OTPCALI_ERROR_SENSOR,
    OTPCALI_ERROR_EEPROM,
    OTPCALI_ERROR_PROGERR,
    OTPCALI_ERROR_READERR,
    OTPCALI_ERROR_CHKERR,
    OTPCALI_ERROR_LIFETEST,
    OTPCALI_ERROR_LSCCALI,
    OTPCALI_ERROR_WBCALI,
    OTPCALI_ERROR_DATACONSIST,
    OTPCALI_ERROR_OMIT,
    OTPCALI_ERROR_PDAF_SPC,
    OTPCALI_ERROR_PDAF_DCC,
    OTPCALI_ERROR_PDAF_EEPROM_VERIFY,
    OTPCALI_ERROR_OPTNOTSUPPORT,
    OTPCALI_ERROR_NO_GOLDEN,
    OTPCALI_ERROR_SHOULDBE_GOLDEN,
    OTPCALI_ERROR_AE_TIMEOUT,
    OTPCALI_ERROR_NO_PROG_HISTORY,
    OTPCALI_ERROR_NVM_EMPTY,
    OTPCALI_ERROR_NODATA,
    OTPCALI_ERROR_INVALID_GROUP,
    OTPCALI_ERROR_NOTDEFINEED,
	OTPCALI_ERROR_NOIMAGE,
	OTPCALI_ERROR_HALLCALI,
	OTPCALI_ERROR_GYROCALI,
	OTPCALI_ERROR_OISCHECK,
	OTPCALI_ERROR_LOADFW,
};

enum OV_GROUP_FLAG
{
    OV_GROUP_FLAG_EMPTY = 0,
    OV_GROUP_FLAG_VALID = 1,
    OV_GROUP_FLAG_INVALID = 3,
};

#define OVSetGroupFlag(mark, group, group_flag) \
    do { \
    (group) = MIN((group), 3);\
    (mark) |= (group_flag) << (6-2*(group));\
    } while (0)

enum OTP_TYPE
{
    OTP_TYPE_OTP = 1 << 0,
    OTP_TYPE_EEPROM = 1 << 1,
};

enum OTP_OPERATOR_ATTR
{
    OTP_OPERATOR_ATTR_USECACHE = 1 << 0,
};

struct WB_DATA_DOUBLE
{
    double R;
    double Gr;
    double Gb;
    double B;
};
struct WB_DATA_UCHAR
{
    uint8_t R;
    uint8_t Gr;
    uint8_t Gb;
    uint8_t B;
};

struct WB_DATA_USHORT
{
	unsigned short R;
	unsigned short Gr;
	unsigned short Gb;
	unsigned short B;
};

struct WB_DATA_SHORT
{
	unsigned short  RG;
	unsigned short  BG;
	unsigned short  GbGr;

};

struct WB_GAIN
{
    int RGain;
    int BGain;
    int GGain;
};
struct WB_RATIO
{
    int g_a, r_g, b_g;
    int r_gr, b_gr, gb_gr, gr_gb, b_gb;

    double multi;
    double dg_a, dr_g, db_g;
    double dr_gr, db_gr, dgb_gr, dgr_gb, db_gb;
};

struct AF_INT
{
    int start;
    int inf;
    int mup;
};
struct AF_PARAM
{
    int raw_sta, raw_inf, raw_mac;
    int off_sta, off_inf, off_mac;
    int sta, inf, mac;
};

struct WB_PARAM
{
    SIZE roi;
    double rg_target;
    double bg_target;
    double rg_delta_spec;
    double bg_delta_spec;
    double golden_rate_spec;
    double golden_be_spec;
	double golden_rate_spec_gbgr;

    int RGrGbB_min[4];
    int RGrGbB_max[4];

    TCHAR goldenSampleName[30];
	
    WB_DATA_UCHAR off_golden;
    int goldenWarningTime;
    int goldenValidTime;

    double rg_multi, bg_multi;
    WB_DATA_UCHAR raw_after_multi;
	WB_DATA_USHORT raw_after_multi2;

    WB_DATA_UCHAR wb_data_c;
	WB_DATA_USHORT wb_data_c2;
    WB_DATA_UCHAR wb_data_c_aftercali;
	WB_DATA_USHORT wb_data_c_aftercali2;

    WB_RATIO ratio, ratio_g, ratio_m;
    double rg_delta;
    double bg_delta;

	double rg_delta_after_cali;
	double bg_delta_after_cali;
	
	double delta_rgVSG;
	double delta_bgVSG;
	double delta_gbgrVSG;
};

struct LSC_QUALCOMM_SHORT
{
	unsigned short  R[221];
	unsigned short  GR[221];
	unsigned short  Gb[221];
	unsigned short  B[221];
};



struct LSC_PARAM
{
    SIZE roi;

    int cali_target;
    int nShadingValue;
	TCHAR goldenSampleName[30];
    double ri_target_l;
    double ri_target_h;
    double ri_delta_target;

    UTS::Algorithm::RI::RI_RESULT riResult;
	int dAvgCenterG;

	BOOL bLSCTable;
	BOOL bLSCVerify;

	//Qualcomm
	double golden_rate_spec;
	double maxdeltarate;
	LSC_QUALCOMM_SHORT LSCTable;
};

typedef struct TDVib
{
	int StaticCircleWidth ;
	int StaticCircleHeight ;
	int OISOFFCircleWidth  ;
	int OISOFFCircleHeight ;
	int DynamicCircleWidth;
	int DynamicCircleHeight;

	RECT DynamicCircleRECT;
	RECT OISOFFCircleRECT;

	double ratio_x;
	double ratio_y;
	double DB_x;
	double DB_y;
}TDVib;

struct OISCHECK_RESULT
{
	TDVib Vib[5];

	RECT StaticCircleRECT;
};

struct OISCAL_RESULT
{
	double PointCountX[2][512];
	double PointCountY[2][512];
	int countX;
	int countY;

	double uCalX,uCalY;

	int StaticCircleWidth ;
	int StaticCircleHeight ;
	int OISOFFCircleWidth  ;
	int OISOFFCircleHeight ;
	int DynamicCircleWidth ;
	int DynamicCircleHeight ;

	double ratio_x;
	double ratio_y;
	double DB_x;
	double DB_y;
};

struct OISCAL_NONSHAKER_RESULT
{
	double Yaw_Dist;
	double Yaw_MoveDiff; 
	double Yaw_Gain; 
	double Pitch_Dist;  
	double Pitch_MoveDiff; 
	double Pitch_Gain;

	double RealGainX;
	double RealGainY;
};


struct OIS_PAPRAM
{
	int nAutoControlShaker;
	int nROISize;
	RECT OISROI;
	int nThresholdA;
	int nThresholdB;

	double dOIS_MinX_db;
	double dOIS_MinY_db;

	double dGyroTurnStartX,dGyroTurnEndX,dGyroTurnGapX;
	double dGyroTurnStartY,dGyroTurnEndY,dGyroTurnGapY;

	double dfltCodeMin, dfltCodeMax;
	//vector<int> vecOISCheckHz;
	CString OISCheckHz;

	OISCHECK_RESULT oischeckResult;
	OISCAL_RESULT oiscalResult;
	OISCAL_NONSHAKER_RESULT oiscal_NonShakerResult;

	//OIS FW
	CString FWFilePath;
	int nFW_CorrectVer;
};

struct PDAFVERIFY_RESULT
{
	int PDAFVerifyDAC;
	int PD;
	int SobelDAC;
	int PDAFInitDAC;
	double PDAFVerifyError;
	int PDAFVerifySonyError;
	double PDAFMTKLinearity;
};

struct PDAFVERIFY_PAPRAM
{
	int PDToleranceError;
	int cdaf_Step;
	int cdaf_type;
	int cdaf_serchoffset;
	PDAFVERIFY_RESULT pdafResult;
};

struct PDAFCAL_RESULT
{
	int PD1;
	int PD2;
};

struct PDAF_PAPRAM
{
	CString pdafcal_ini_file;
	PDAFCAL_RESULT    pdafcalResult;
	PDAFVERIFY_PAPRAM pdafverify;
};


struct OtpOffCheck
{
	int spec_af_sta_l, spec_af_sta_h;
	int spec_af_inf_l, spec_af_inf_h;
	int spec_af_mac_l, spec_af_mac_h;

	double spec_r_g_l, spec_r_g_h;
	double spec_b_g_l, spec_b_g_h;

	int result;
	int minfo_result;
	int af_result;
	int wb_result;
	int lsc_result;
	int pdaf_result;
};

enum TEST_RESULT_TYPE
{
	RESULT_OMIT,
	RESULT_PASS,
	RESULT_FAIL
};
enum OtpDataType
{
	OtpDataType_MInfo,
	OtpDataType_AF,
	OtpDataType_WB,
	OtpDataType_LSC,
	OtpDataType_PDAF,
	OtpDataType_Num
};
struct OTP_PARAM
{
	int group_for_use;

	int nEEPRomType;

	int SensorBlack_level;

	double dFPNSpec;
	struct AF_PARAM af;
	struct WB_PARAM wb_param;
	struct LSC_PARAM lsc_param;
	struct PDAF_PAPRAM pdaf_param;
	struct OIS_PAPRAM ois_param;
	struct OtpOffCheck otp_off_param;
};

//-------------------------------------------------------------------------------------------------
#pragma pack(push, 1)

struct OtpDataMInfo
{
    uint8_t *tm;
    int tm_len;

    int compare_start;
    int raw_len;
    uint8_t raw[0];
};
struct OtpDataAF
{
    AF_INT af;

    int raw_len;
    uint8_t raw[0];
};
struct OtpDataWB
{
    bool has_golden;
    WB_RATIO ratio, ratio_g;
    WB_GAIN gain;

    int raw_len;
    uint8_t raw[0];
};
struct OtpDataLSC
{
    uint8_t *lsc;

    int raw_len;
    uint8_t raw[0];
};



struct OtpDataItem
{
    int type;

    int sum_len, sum_calc_len;
    void *sum_sta;
    uint32_t sum;

    int len;
    uint8_t data[0];
};
struct OtpDataHeader
{
    int len;
    uint8_t data[0];
};

struct OTP_INIT_PARAM 
{
	int nEEPRomType;
	const TCHAR* sn;
	DDM::OtpDBPara otpDBPara;
	OTP_PARAM stOtpParam;
};

enum SONY_DCC_Status{OFF,HOLD};

#pragma pack(pop)
//-------------------------------------------------------------------------------------------------
class BaseOtp
{
public:
    BaseOtp(BaseDevice *dev);
    virtual ~BaseOtp(void);

    //-------------------------------------------------------------------------
    // WB
    void get_gain_by_wb_ratio(const WB_RATIO *ratio, const WB_RATIO *ratio_target, double base, WB_GAIN *gain);
    int check_wb_with_golden(const WB_RATIO *ratio, const WB_RATIO *ratio_g);
	int check_wb_qualcommm_with_golden(const WB_DATA_SHORT wb,const WB_DATA_SHORT wb_golden);
    virtual int upload_wb_with_golden(const WB_DATA_UCHAR *wb);
	int upload_wb_with_golden(const WB_DATA_SHORT *wb);
	int upload_wb_with_golden(const WB_DATA_USHORT *wb);
    void get_wb_ratio(const WB_DATA_UCHAR *wb, WB_RATIO *ratio, double multi);
	void get_wb_ratio(const WB_DATA_USHORT *wb, WB_RATIO *ratio, double multi);
    void GetWBROI(const WB_PARAM *pwb_param, const SIZE *imagesz, ROI *roi);
    int CheckWBLimit(WB_DATA_UCHAR *wb);
    int wb_do_multi(const WB_DATA_UCHAR *in, WB_DATA_UCHAR *out);
	int wb_do_multi(const WB_DATA_SHORT *in, WB_DATA_SHORT *out);
	int wb_do_multi(const WB_DATA_USHORT *in, WB_DATA_USHORT *out);
    int CheckWBResult(uint8_t* pRaw8, int width, int height, int rawBayerType);
	int CheckWBResult_RAW10(WORD* pRaw10, int width, int height, int rawBayerType);
    virtual int get_wb_gain(WB_RATIO *ratio, WB_RATIO *ratio_target, WB_GAIN *gain) {return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
    virtual int get_wb_cali_data(WB_GAIN *gain, void *out) {return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
    virtual int do_wb_cali_before(uint8_t* pRaw8, int width, int height, int rawBayerType);
    virtual int do_wb_cali();
    virtual int WBCali(void *args);
	virtual int WBCali_RAW10(void *args);
    //-------------------------------------------------------------------------
    // check & prog
    int _get_otp_data_from_db(void *args);
    virtual int get_otp_data_from_db(void *args) {return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
    virtual int get_otp_data_from_sensor(void *args)  {return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
    virtual int prog_otp(void *args);
    virtual int do_prog_otp() {return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
    int check_otp(void *args);
    virtual int do_check_otp(void *args) {return SET_ERROR(OTPCALI_ERROR_NO);}

	int check_otp_off_wb(void *args);
	int check_sram_off_lsc(void *args);
	int otp_off_chk(void *args);
	int check_otp_off_lsc(void *args);
	int check_uniform_otp_data(OtpDataHeader *hdr);
	int wb_check_rawinfo_limit(WB_DATA_UCHAR *wb);
	int wb_check_result_after_calibration(void);
	OtpDataItem *get_otp_from_uniform_by_type(int type);
	int wb_get_rawinfo_from_image(WB_DATA_DOUBLE *dwb, WB_DATA_UCHAR *wb);
    //-------------------------------------------------------------------------
    // Other
    BOOL Initialize_param(const OTP_INIT_PARAM& init_param);
    int is_otp_data_locked(int module_id);
    BOOL is_otp_programed(int module_id);
        
    virtual int check_lsc(struct LSC_PARAM *lsc);
	virtual int sram_lsc_witeback(void *args){return SET_ERROR(OTPCALI_ERROR_NO);}

	virtual int LscCali(void *args);
    virtual int do_lsc_cali();
	virtual int chagetexttohex(char *buf, char *output, int len);
	virtual int write_reg(char *buf, int len);
    virtual int get_group(uint8_t flag, int max_group, int group_flag);
    virtual int get_uniform_otp_data(void *in, void *out, int maxlen) {return SET_ERROR(OTPCALI_ERROR_NO);}
    virtual int get_otp_group(void) {return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
	virtual int get_minfo_from_db(void *args) {return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}

	//LSC
	int do_mtk_lsc_cali();
	int do_mtk_lsc_cali_MTK6797_MONO();
	int do_mtk_lsc_cali_MTK6797_RGB();
	int do_qulcomm_lsc_awb_cali();
	int do_qulcomm_lsc_awb_cali_RAW10();
    int upload_lsc();

    int SET_ERROR(int errorNo);
    bool is_mem_val(const void *mem, int val, int len);
    void PrintDebugArray(const void *array, int len);
    
    int get_otp_from_raw_data(int type, void *out, int len);
    int get_minfo_from_raw_data(void *out, int len);
    int get_wb_from_raw_data(WB_DATA_UCHAR *wb, WB_DATA_UCHAR *wb_g, int type = OTPDB_OTPTYPE_AWB);
	int get_lsc_from_raw_data(void *out, int len);
	int get_af_from_raw_data(int *sta, int *inf, int *mac);
	virtual int upload_lsc_cali_data(void *raw, int len) ;

	int check_lsc_with_golden(uint8_t *modulelsc,int lsclength);
	int check_lsc_qualcommm_with_golden(uint8_t *module,uint8_t *golden,int lsclength);
	virtual int decode_lsc_data(void *in, int len, void *out, int maxlen) {memcpy(out, in, len); return len;}
    inline int get_mid(void) { return mid; }
    OTP_OPTION *get_option(int opt);
    int GetErrorCode();

    struct OTP_PARAM otp_param;
    uint8_t *otp_data_in_db;
    uint8_t *otp_data_in_sensor;
    int otp_data_len;
    int otp_data_eeprom_len; /* indicate eeprom data length in case of using otp and eeprom in the sametime */
    int otp_lsc_len;
	int otp_spc_len;
	int otp_dcc_len;
    uint8_t *otp_lsc_data;

	//for sony pdaf
	int pdaf_pd_len;
	int pdaf_sony_verifydac;
	int pdaf_sony_calmode;

    uint32_t otp_operator_attr;
    static int otpdb_data_cache_len;
    static uint8_t *otpdb_data_cache;
   
	static uint8_t *uniform;

	//device
	static UTS::SensorDriver *sensor;
	static UTS::OISDriver *ois;
	static UTS::EEPROMDriver *eeprom;

	BaseDevice *dev;
	BUFFER_INFO m_bufferInfo;
	TEST_BUFFER m_bufferObj;

	//DUMP
	int DumpOtp(void *args);

	//OIS Load FW
	int LoadFW(void *args);
	virtual int do_LoadFW(CString FWFilePath) {return SET_ERROR(OTPCALI_ERROR_NO);}

	//Check FW
	int CheckFW(void *args);
	virtual int do_CheckFW(int FWVer) {return SET_ERROR(OTPCALI_ERROR_NO);}

	//HallCali
	int HallCali(void *args);
	virtual int do_hallCali() {return SET_ERROR(OTPCALI_ERROR_NO);}
	UINT32 HallCaliData[25];
	UINT32 unHall_errorCode;

	//GyroCali
	int GyroCali(void *args);
	virtual int do_SetgyroX(double gain) {return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int do_SetgyroY(double gain) {return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int do_SaveGyroGainData(double gainX,double gainY) {return SET_ERROR(OTPCALI_ERROR_NO);}

	//OISCheck
	bool VibratorControl(char *CommmandData);
	bool EEAVibratorControl(int HZ);
	int OISCheck(void *args);
	void SaveImage(LPCTSTR lpName);
	void SaveImage2(LPCTSTR lpName);
	void SaveRAWImage(BYTE *RAWBuffer,int width,int height,LPCTSTR lpName);
	virtual int do_OISON() {return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int do_OISOFF() {return SET_ERROR(OTPCALI_ERROR_NO);}

	virtual int SPCCali(void *args);
	int SPCCali_2PD();

	virtual int DCC0Cali(void *args) {return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int DCC1Cali(void *args) {return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int PDAFVerify(void *args) {return SET_ERROR(OTPCALI_ERROR_NO);}

	virtual int do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len){return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}
	virtual int do_DCCCali(uint8_t *SPCTable,uint16_t **imagebuf, 
						   int16_t *dcc_stack_lenspos, uint8_t out[]){return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}


	int do_qulcomm_dcc_cali(int dcctype);
	int get_qulcomm_pd_REVL();
	int get_qulcomm_2pd_REVL();
	int do_sony_dcc_cali();
	int mtk_spc_cali(uint16_t *pRaw10,char *filename,uint8_t out[]);
	int mtk_dcc_cali(char *filename);
	int mtk_dcc_verify(char *filename);

	int get_qulcomm_pd(uint8_t *SPCTable,int type, int inf, int mup, float per);
	int qulcomm_dcc_verify();
	int qulcomm_dcc_verify_REL();
	int sony_dcc_verify();
	int GetAFData(AF_INT *af);

	//Sensor SPC Cali
	virtual int SensorSPCCali(void *args){return SET_ERROR(OTPCALI_ERROR_NO);}
	int SONYSPCCali();
	virtual int do_WriteSPCSetting1(){return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int do_WriteSPCSetting2(){return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int do_WriteDCCSetting3(){return SET_ERROR(OTPCALI_ERROR_NO);}

	virtual int do_SPC_writeback(uint8_t *regs){return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int do_SONY_DCC_stats(int mode){return SET_ERROR(OTPCALI_ERROR_NO);}

	virtual int do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, 
		int w, int h, uint8_t out[],uint8_t awbout[]){return SET_ERROR(OTPCALI_ERROR_NO);}

	virtual int do_qualcomm_DCCCali(uint8_t *SPCTable,uint16_t *raw_negative, 
		uint16_t *raw_positive, int Width, int Height, 
		int dac_negative, int dac_positive){return SET_ERROR(OTPCALI_ERROR_NO);}

	virtual bool do_qulcomm_DCCVerify(uint8_t *SPCTable,uint16_t *img,
				  int w, int h,int PD_ConversionCoeff,
				  int ToleranceError,
				  int VerifyDAC,int solbelfocusdac,
				  int &PDAFDAC,
				  double &VerifyError){return TRUE;}

	virtual bool do_qulcomm_DCCVerify_REV(uint8_t *DCCTable,uint16_t *img){return TRUE;}

	int GyroCali_NonShaker(void *args);
	
	//Ryan@20160506 ON123 nonshaker OIS calibration
	virtual int do_SetSltPos( unsigned int UcPos ){return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int do_SetVrtPos( unsigned int UcPos ){return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int do_SetHrzPos( unsigned int UcPos ){return SET_ERROR(OTPCALI_ERROR_NO);}
	virtual int do_GetGyroGainX(UINT32 *Gain){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
	virtual int do_GetGyroGainY(UINT32 *Gain){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}

	virtual int do_SONY_SPCCali(unsigned short *pRaw10, short *out){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
	virtual int do_SONY_SPCVerify(unsigned short *pRaw10,int *judge){return TRUE;}

	virtual int do_SONY_GetPDTable(int *pdtable){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
	virtual int do_SONY_DCCCali(int lens1, int lens2, int *pd1, int *pd2,unsigned short *dcc_out, int calc_mode){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}
	virtual int do_SONY_DCCVerify(unsigned short*dcc, int *pd, int *dcc_out, int *pd_out ,int calc_mode){return SET_ERROR(OIS_ERROR_NOTSUPPORT);}

	int SamsungLscCali();
	virtual int do_SamsungLSCCali(uint16_t *pRaw10,int width, int height, char *otp, int &otp_len,char *sram, int &sram_len){return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}

	virtual int LoadSensorLSC(char *lscsetting){return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);}

	int doSamsungLSCCali_5E2(uint16_t *pRaw10,int width, int height, char *LSCSetting_OTP,int &LSCLen_OTP, char *LSCSetting_SRAM,int &LSCLen_SRAM);

protected:
    int otp_type;
    int errorNo;
    int mid;
	
    TCHAR m_szSN[128];
    DDM::OtpDB *otpDB;
    time_t otp_data_from_db_valid_time;

    bool is_otp_mem_def();
    int do_dump_otp();
    int do_dump_eeprom();
	void do_save_buff(const char * save_path,void *buffer,int buffer_len,int buffer_type = 0);
	
private:
    //typedef int (OtpCali::*otpfunc)(void *args);
    static OTP_OPTION options[];

};

BaseOtp* GetOtpInstance(
    const TCHAR *pProjectName,
    BaseDevice *pDevice);
