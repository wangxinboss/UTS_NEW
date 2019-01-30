#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"

//#include "ClientSocket.h"

#include <atlcom.h>


#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif


struct WB_PARAM
{
	SIZE roi;
};

//光源管控规格
struct LightCali
{
	int GoldenR;
	int GoldenGr;
	int GoldenGb;
	int GoldenB;
	double IndexSpec;
	double IndexSpec_RGain;
	double IndexSpec_BGain;
	double IndexSpec_GGain;
	int LightCaliEn;
};

struct OTP_PARAM
{
	struct WB_PARAM wb_param;

	struct LightCali  lightCali;   //光源管控规格

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

struct ROI 
{
	int x;
	int y;
	int width;
	int height;
};

namespace UTS
{
    typedef struct _operator_param_
    {
        CString strRegStrForCheck;
        CString strHintMessage;
		CString IP;
		int Port;
    } OPERATOR_PARAM;

    class ImplOperator : public BaseOperator
    {
    public:
        ImplOperator(void);
        ~ImplOperator(void);

//		CClientSocket m_ClientSocket;

		CString sIniFileName;
		USHORT ReadHour;
		int BlackLel;

		BaseDevice *dev;

		struct OTP_PARAM otp_param;

        virtual BOOL OnReadSpec();
        virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
        virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);



		//光源点检
		int LightCalibra();

    private:
        OPERATOR_PARAM m_param;
    };

    extern "C"
    {
        OPERATOR_API BaseOperator* GetOperator(void);
    }
}
