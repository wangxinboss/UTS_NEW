#pragma once
#include "UTSDefine.h"
#include "Algorithm.h"
#include "BaseOperator.h"


//UTOP_OTPCheck

#ifdef OPERATOR_EXPORTS
#define OPERATOR_API __declspec(dllexport)
#else
#define OPERATOR_API __declspec(dllimport)
#endif

#define  LSC_DATA_LENGTH 1768

namespace UTS
{
	struct MINFO  //16
	{	
		BYTE FlagOfInfo;
		BYTE CalToolVersion;
		BYTE Year;
		BYTE Month;
		BYTE Day;
		BYTE ModuleID;
		BYTE LensID;
		BYTE VCMID;
		BYTE DriverICID;
		BYTE ColorTemperture;
		BYTE MirrorFlip;
		BYTE QRCode[4];
		BYTE BasicInfoChecksum;
	};


	struct WB	//16
	{	
		BYTE FlagOfWB;
		BYTE WBHex[12];
		BYTE AWBChecksum;
		BYTE Reserved0[2];
	};

	struct LSC  //2016
	{	
		BYTE FlagOfLSC;
		BYTE lscData[LSC_DATA_LENGTH];
		BYTE ChecksumOfLSC;
		BYTE Reserved1[246];
	};

	struct AF  //8
	{	
		BYTE FlagOfAF;
		BYTE AFData[4];
		BYTE AFChecksum;
		BYTE Reserved2[2];
	};


	struct PDAF  //1016
	{	
		BYTE FlagOfGainMap;   //0x0808
		BYTE GainMapData[890];
		BYTE GainMapChecksum;
		BYTE Reserved3[4];
		BYTE FlagDCC;			//0x0b88
		BYTE DCCData[102];
		BYTE DCC_Checksum;  //0x0bef
		BYTE Reserved4[16];
	};


	struct AEC  //16
	{	
		BYTE FlagOfAEC;  //0x0c00
		BYTE AECData[10];
		BYTE AECChecksum;
		BYTE Reserved5[4];
	};

	struct ARCSORT  //2050
	{	
		BYTE FlagOfDual;
		BYTE DualData[2048];
		BYTE DualChecksum;
	};


	struct OTPData
	{
		MINFO minfo;
		WB wb;
		LSC lsc;
		AF af;
		PDAF pdaf;
		AEC aec;
		ARCSORT arcsort;
	};


	typedef struct _operator_param_
	{
		int nEEPRomType;
		int nReBoot;


		MinMax<double> dAWBSpec_RGr;
		MinMax<double> dAWBSpec_BGr;



		MinMax<double> dAFSpec_Inf;
		MinMax<double> dAFSpec_Macro;

		//Module info ID
		int nModuleID;
		int nLensID;
		int nVCMID;
		int nDriverICID;

	} OPERATOR_PARAM;

	typedef struct _operator_result_
	{
		int nAWBdata[6];  //AWB RG,BGr,GrGb,BGr 
		int nAFData[2];  //Inf,Macro
		int nAECData[5];  //BrightNess ratio,Main Gain,Main Exposure,Aux Gain,Aux Exposure
	}OTPCheck_Result;

	class ImplOperator : public BaseOperator
	{
	public:
		ImplOperator(void);
		~ImplOperator(void);

		virtual BOOL OnReadSpec();
		virtual BOOL OnTest(BOOL *pbIsRunning, int *pnErrorCode);
		virtual void OnGetErrorReturnValueList(vector<int> &vecReturnValue);
		virtual void OnGetRegisterList(vector<CString> &vecRegister);
	
		CStringA GetModulePath();


	protected:
		virtual void GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter);

	private:
		//------------------------------------------------------------------------------
		// ²ÎÊý
		OPERATOR_PARAM m_param;
		OTPCheck_Result m_result;
		OTPData m_otpdata;
		//------------------------------------------------------------------------------

		bool CheckOTPData();
		int sum;
	};

	extern "C"
	{
		OPERATOR_API BaseOperator* GetOperator(void);
	}
}

