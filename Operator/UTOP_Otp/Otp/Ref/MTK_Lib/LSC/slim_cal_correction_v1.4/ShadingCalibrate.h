

// The following ifdef block is the standard way of creating macros which make exporting 

// from a DLL simpler. All files within this DLL are compiled with the SHADINGCALIBRATE_EXPORTS

// symbol defined on the command line. this symbol should not be defined on any project

// that uses this DLL. This way any other project whose source files include this file see 

// SHADINGCALIBRATE_API functions as being imported from a DLL, wheras this DLL sees symbols

// defined with this macro as being exported.



#ifdef __cplusplus

extern "C" {

#endif



#ifdef SHADINGCALIBRATE_EXPORTS

    #define SHADINGCALIBRATE_API __declspec(dllexport)

#else

    #define SHADINGCALIBRATE_API __declspec(dllimport)

#endif



#define CCA_TAKE_MODE_SHADING_TABLE_SIZE    (896*4)

#define CCA_PREVIEW_MODE_SHADING_TABLE_SIZE (320*4)

#define MAX_SHADING_TABLE_CAPACITY			(1000*4)

#define MAX_CAP_SHADING_TABLE_SIZE_v1			(144*4)



typedef enum{

    PREVIEW_MODE,					// Capture image in preview mode

    TAKE_MODE						// Capture image in normal mode

}CAMERA_MODE;



typedef enum{

    CALIBRATION_SUCCESS,			// Success to do shading calibration

	CALIBRATION_FAIL,				// Fail to do shading calibration

    STEP_OK,						// STEP OK - internal use

    INVALID_RAW_DATA_NULL,			// Fail to load image raw data

    INCOMPLETE_INITIALIZATION,		// Fail to initilize paramters

    EXCEED_SHADING_TABLE_SIZE,		// Fial because of exceeding shading table size

    FAIL_TO_FILL_SHADING_TABLE,		// Fail to fill shading table

	OVERFLOW_HARDWARE_BITS			// Overflow hardward bits	

}SHADING_CALIBRATION_RESULT;



typedef enum{

    CORRECTION_SUCCESS,				// Success to do shading correction

	CORRECTION_FAIL,				// Fail to do shading correction

	INIT_PARA_FAIL,					// Fail to initlize paramter for shading correction

	FREE_MEM_FAIL,					// Fail to free internal allocated memory

	MEM_INIT_FAIL,					// Fail to allocate internal usage memeory

	IMAGE_LOADING_FAIL,				// Fail to load image data fail

	BUFFER_FOR_LSC_IMAGE_NULL,		// Fail to copy result shading correctio image because there is no buffer for image with shading correction	

	GAIN_TABLE_FAIL				// Fail in gain table check

}SHADING_CORRECT_RESULT;



typedef enum{

	BIT_MODE_8	= 8,				// source raw image is 8 bit mode

	BIT_MODE_10 = 10				// source raw image is 10 bit mode

}BIT_MODE;



typedef struct{    

	// image parameters

    CAMERA_MODE camera_mode;		// camera mode

	int shading_version;

	BIT_MODE source_bit_mode;		// source bit mode

	int src_first_pixel;			// source first pixel    

    unsigned char* raw_image_buf;	// source raw data image

    int raw_image_width;			// source raw data width

    int raw_image_height;			// source raw data height

    // tuning parameters

	bool gGrGbSameTbl;				//Gr/Gb apply the same table

    int offset_x0;

    int offset_y0;

    int offset_x1;

    int offset_y1;

    int m;

    int n;

    int ratio_00;

    int ratio_01;

    int ratio_10;

    int ratio_11;

    int block_avg_width;	

	int corr_level;

	bool is_modify_get_maxVal;

	bool is_modify_set_lsc_cur;

	int pat_center_x;

	int pat_center_y;

	int max_pxl_gain;

	double coefpoly[6];

}LSC_para;



typedef struct{

	unsigned char* shading_table_LO[MAX_CAP_SHADING_TABLE_SIZE_v1];	// source capture raw data image (LO) for v3

	unsigned char* shading_table_MD[MAX_CAP_SHADING_TABLE_SIZE_v1];	// source capture raw data image (MD) for v3

	unsigned char* shading_table_HI[MAX_CAP_SHADING_TABLE_SIZE_v1];	// source capture raw data image (Hi) for v3

}dynamic_shading_para;



typedef struct{   

	// for NVRAM settings

    int           shading_table_size;							// shading coef table size

    unsigned int  shading_table[MAX_SHADING_TABLE_CAPACITY];	// shading coef table

	// for register settings

	int m;

    int n;

	int	block_width;

	int	block_height;

	int	last_block_width;

	int	last_block_height;		

}LSC_coef;



typedef struct{	

    CAMERA_MODE camera_mode;				// camera mode

	int shading_version;

	BIT_MODE source_bit_mode;				// source bit mode

	int src_first_pixel;					// source first pixel    

    int raw_image_width;					// source image width

    int raw_image_height;					// source image height

	float gain_table_thresh;				// threshold of gain table error check

	bool gain_table_check;					// enable/disable gain table

	LSC_coef LSC_coef;						// LSC result coef	

	unsigned char *p_raw_without_LSC_buf;	// input: source raw image pointer to a buffer

	unsigned char *p_raw_with_LSC_buf;		// output: corrected raw image pointer to a buffer

}LSV_para;





//-----------------------------------shading verification---------------------------------------------------------------//

#define checkWinHeight 0.1

#define checkWinWidth 0.1

#define centerWinHeight 0.2

#define centerWinWidth 0.2

#define shadingLumUBound 200

#define shadingLumLBound 50

#define Y_Decay 0.75

#define Y_Decay 0.75

#define R_G_Diff 0.05

#define B_G_Diff 0.05



typedef struct  

{

	int left;

	int right;

	int bottom;

	int top;

}SRect;



typedef struct  

{

	int w;

	int h;

}Ssize;



typedef struct  

{

	int x;

	int y;

}Spoint;



typedef struct 

{

	SRect ROI;

	int m_dAvgB;

	int m_dAvgG;

	int dAvgR;

	int m_dAvgLum;

	bool m_bPass;

	

}ROIinfo;
typedef struct
{
	unsigned char center_R;
	unsigned char center_G;
	unsigned char center_B;
	double R_GAIN;
	double B_GAIN;
}pre_gain;

SHADINGCALIBRATE_API SHADING_CALIBRATION_RESULT ShadingCalibrateEx(LSC_para *p_input, LSC_coef *p_result);

SHADINGCALIBRATE_API SHADING_CORRECT_RESULT		ShadingCorrectEx(LSV_para *p_LSV_para );

SHADINGCALIBRATE_API SHADING_CORRECT_RESULT ShadingCorrectSubEx(LSV_para *p_LSV_para, bool b_subSample);



SHADINGCALIBRATE_API bool ShadingVerification(int imageHeight, int imageWidth, unsigned char *m_pRaw, int m_firstPixel);



#ifdef __cplusplus

}

#endif