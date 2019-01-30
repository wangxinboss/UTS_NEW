#include "StdAfx.h"
#include "OtpCali_S5K4H8_MA80W.h"
#include "VCM.h"

#define OFFSETMACRO  85
#define OFFSETINF  114

typedef void (*SetParameter)( int m_nPedestal, int m_nWidth, int m_nHeight, int m_nGr, int m_nR, int m_nB, int m_nGb, double m_dbAlpha, double m_dbSeed, BOOL m_bLens, BOOL m_bScooby );
typedef BOOL (*Run)( BYTE *pRawBuffer, BYTE *pTrgBuffer, BOOL m_bSimulatedResults, CString strWorkDir, BOOL b2ndSet, BOOL *is_SINGULAR, BOOL *bSEED_overflow);
typedef BOOL (*SaveOTPSetfile)( const char* _Filename);
typedef BOOL (*SaveSRAMSetfile)( const char* _Filename);
typedef int  (*GetOTPSetfileSize)();
typedef int  (*GetOTPSetfile)( char *szOTP);
typedef int  (*GetSRAMSetfileSize)();
typedef int  (*GetSRAMSetfile)( char *szSRAM);

using namespace UTS::Algorithm::RI::RI_OpticalCenter;
using namespace UTS::Algorithm::Image;

namespace OtpCali_S5K4H8_MA80W {
//-------------------------------------------------------------------------------------------------
OtpCali_S5K4H8_MA80W::OtpCali_S5K4H8_MA80W(BaseDevice *dev) : SamsungOtp(dev)
{
	otp_type = OTP_TYPE_OTP;
    otp_data_len = sizeof(OTPData);
    otp_lsc_len = 360;
    otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA80W::do_prog_otp()
{

	//lsc
 //	if(!sensor->do_prog_otp(0x0,0x0A3D,otp_data_in_db,1))
 //		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x03,0x0A04,otp_data_in_db+64*0,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x04,0x0A04,otp_data_in_db+64*1,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x05,0x0A04,otp_data_in_db+64*2,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x06,0x0A04,otp_data_in_db+64*3,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x07,0x0A04,otp_data_in_db+64*4,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x08,0x0A04,otp_data_in_db+64*5,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x09,0x0A04,otp_data_in_db+64*6,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x0A,0x0A04,otp_data_in_db+64*7,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x0B,0x0A04,otp_data_in_db+64*8,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x0C,0x0A04,otp_data_in_db+64*9,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x0D,0x0A04,otp_data_in_db+64*10,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x0E,0x0A04,otp_data_in_db+64*11,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);

	//MINFO+AF
	if(!sensor->do_prog_otp(0x0F,0x0A04,otp_data_in_db+64*12,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	
	
	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA80W::get_otp_data_from_sensor(void *args)
{
    OTPData *otp = (OTPData*)args;

	//if(!sensor->do_read_otp(0x0,0x0A3D,&otp->lsc.flag,1))
	//	return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x03,0x0A04,otp->Reserve1+64*0,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x04,0x0A04,otp->Reserve1+64*1,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x05,0x0A04,otp->Reserve1+64*2,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x06,0x0A04,otp->Reserve1+64*3,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x07,0x0A04,otp->Reserve1+64*4,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x08,0x0A04,otp->Reserve1+64*5,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x09,0x0A04,otp->Reserve1+64*6,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x0A,0x0A04,otp->Reserve1+64*7,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x0B,0x0A04,otp->Reserve1+64*8,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x0C,0x0A04,otp->Reserve1+64*9,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x0D,0x0A04,otp->Reserve1+64*10,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x0E,0x0A04,otp->Reserve1+64*11,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	
	//MINFO
	if(!sensor->do_read_otp(0x0F,0x0A04,otp->Reserve1+64*12,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA80W::LscCali(void *args)
{
	OTPData *otp = (OTPData*)args;

	int ret;	

	BOOL pResult;//*pText;
	
	CString tempstr,tempstr1;

	USES_CONVERSION;

	//FILE* fp_bayer_setting = fopen("bayer_setting.xml","rt");
	//FILE* fp_binary_dat    = fopen("binary.dat","rt");

	int m_nWidth = 3264;
	int m_nHeight = 2448;
	int m_nGr=60,m_nR=60,m_nB=60,m_nGb=60;
	double m_dbAlpha = 0.3, m_dbSeed = 0.01;
	BOOL m_bScooby = FALSE;

	CStdioFile file;
	
	int nLength,strValue;
	int cnt_line,cnt_reg;
	BYTE pLenCReg[360];

	CString strRawFile;
	CString strOTPSetFile;
	CString strSRAMSetfile;
	CString strWorkPath;
	char charWorkPath[1024];

	struct LSC_PARAM *lsc = &otp_param.lsc_param;

	HINSTANCE hdll;

	hdll = LoadLibrary(_T("LSCDLL_4H8_v24.dll"));	
	if(hdll==NULL) { 		
		FreeLibrary(hdll);
		uts.log.Error(_T("LSC: LSC_lib load error."));
		return SET_ERROR(SENSORDRIVER_ERROR_LSCCALI);
	}

	SetParameter setParameter             = (SetParameter)GetProcAddress(hdll,"SetParameter");
	Run run                               = (Run)GetProcAddress(hdll,"Run");
	SaveOTPSetfile saveOTPSetfile         = (SaveOTPSetfile)GetProcAddress(hdll,"SaveOTPSetfile");
	SaveSRAMSetfile saveSRAMSetfile       = (SaveSRAMSetfile)GetProcAddress(hdll,"SaveSRAMSetfile");
	GetOTPSetfileSize getOTPSetfileSize   = (GetOTPSetfileSize)GetProcAddress(hdll,"GetOTPSetfileSize");
	GetOTPSetfile getOTPSetfile           = (GetOTPSetfile)GetProcAddress(hdll,"GetOTPSetfile");
	GetSRAMSetfileSize getSRAMSetfileSize = (GetSRAMSetfileSize)GetProcAddress(hdll,"GetSRAMSetfileSize");
	GetSRAMSetfile getSRAMSetfile         = (GetSRAMSetfile)GetProcAddress(hdll,"GetSRAMSetfile");

	if(!setParameter||!run||!saveOTPSetfile||!saveSRAMSetfile||!getOTPSetfileSize||!getOTPSetfile||!getSRAMSetfileSize||!getSRAMSetfile){
		FreeLibrary(hdll);
		uts.log.Error(_T("LSC: LSC_lib getprocess error."));
		return SET_ERROR(SENSORDRIVER_ERROR_LSCCALI);
	}
	//////////////////////////////////////////////////////////////////////////

	dev->GetBufferInfo(m_bufferInfo);

	WORD *RAW10Image_input  = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
	WORD *RAW10Image_output = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame)){
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;	
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);


	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);
	if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_V10_input"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight)){
		uts.log.Error(_T("LSC: failed to save raw_input file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	//////////////////////////////////////////////////////////////////////////
	//get lensRI
	double pLensRI [11];
	FILE* fp_lensRI = fopen("LensRI.txt","rt");
	if(fp_lensRI==NULL)
	{
		uts.log.Error(_T("LensRI.txt Open Error."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}else{
		
		char strData[256];
		for(int i=0;i<11;i++)
		{
			fscanf(fp_lensRI,"%s",strData);
			pLensRI[i] = atof(strData);
			//sscanf(strData,"%f",&pLensRI[i]);
		}

		double crmax = 100*(1-(100-1.4*pLensRI[0])/(100-pLensRI[0]));
		m_nGr = (int) (m_nGr > crmax ? crmax : m_nGr); //m_nGr ªì©l­È?
		m_nR = (int) (m_nR > crmax ? crmax : m_nR);
		m_nB = (int) (m_nB > crmax ? crmax : m_nB);
		m_nGb = (int) (m_nGb > crmax ? crmax : m_nGb);
	}
	fclose(fp_lensRI);
	//////////////////////////////////////////////////////////////////////////

	setParameter(64, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_nGr,m_nR, m_nB, m_nGb, 0.3, 0.01, TRUE, FALSE);
	
	//////////////////////////////////////////////////////////////////////////
	/*AF_INT af;
	GetAFData(&af);
	uts.log.Info(_T("af.inf = %d, af.mup = %d"),af.inf,af.mup);

	if (af.inf < 0 || af.mup < 0){
		uts.log.Error(_T("GetAFData fail"));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	//VCM Move INF 
	DLLSetVCM_Initial(dev,uts.info.nVCMType, NULL);
	Sleep(500);
	DLLSetVCM_Move(dev, uts.info.nVCMType, af.inf);
	Sleep(300);
	*/
	//////////////////////////////////////////////////////////////////////////	

	GetCurrentDirectoryA(1024,charWorkPath);
	strWorkPath = A2W(charWorkPath);

	strWorkPath = _T("D:\\");

	BOOL is_SINGULAR = FALSE;
	BOOL bSEED_overflow = FALSE;

	pResult = -1;
	pResult = run((BYTE *)RAW10Image_input,(BYTE *)RAW10Image_output, TRUE, strWorkPath, FALSE, &is_SINGULAR, &bSEED_overflow);

	//if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_V10_output"),(BYTE*)RAW10Image_output,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight)){
	//	uts.log.Error(_T("LSC: failed to save raw_input file."));
	//	ret = SENSORDRIVER_ERROR_LSCCALI;
	//	goto out;
	//}

	if (pResult == TRUE){
		uts.log.Info(_T("LSC_Lib : run pass."));
	}else{
		uts.log.Error(_T("LSC_Lib : run fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	pResult = -1;
	pResult = saveSRAMSetfile("sram.txt");
	if (pResult == TRUE){
		uts.log.Info(_T("LSC_Lib : SaveSRAMSetfile pass."));
	}else{
		uts.log.Error(_T("LSC_Lib : SaveSRAMSetfile fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	pResult = -1;
	pResult = saveOTPSetfile("otp.txt");
	if (pResult == TRUE){
		uts.log.Info(_T("LSC_Lib : SaveOTPSetfile pass."));
	}else{
		uts.log.Error(_T("LSC_Lib : SaveOTPSetfile fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

/*	if (!file.Open(_T("sram.txt"), CFile::modeRead | CFile::typeText )){
		uts.log.Error(_T("open sram.txt fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	while(file.ReadString(tempstr))
	{
		if(tempstr.Find(_T("s"),0)!=-1)
		{
		    nLength=tempstr.GetLength();
			nLength--;
			tempstr=tempstr.Right(nLength);
			if(nLength==6){
				strValue=strtoul(T2A(tempstr),NULL,16);
				sensor->dev->i2c_write(BIT16_BIT8,(uint16_t)(strValue>>8),(uint16_t)((strValue&0x00ff)));
			}else if(nLength==8){
				strValue=strtoul(T2A(tempstr),NULL,16);
				sensor->dev->i2c_write(BIT16_BIT16,(uint16_t)(strValue>>8),(uint16_t)((strValue&0xffff)));

			}

		}
	}
	file.Close(); 

	sensor->dev->write_sensor(0x6028,0x4000);
	sensor->dev->write_sensor(0x602A,0x0B00);
	sensor->dev->write_sensor(0x6F12,0x0180);
	Sleep(1000);

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_V10_apply"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("LSC: failed to save raw_apply file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);


	if (check_lsc(lsc) < 0) {
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;
	}*/

	if ( !file.Open( _T("otp.txt"), CFile::modeRead | CFile::typeText )){
		uts.log.Error(_T("open otp.txt fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	cnt_line=0,cnt_reg=0;

	memset(pLenCReg,0,360);

	while(file.ReadString(tempstr)&&cnt_reg<otp_lsc_len){
		if(cnt_line<7&&(tempstr.Find(_T("s0A04"),0) != -1)){
		    nLength=tempstr.GetLength();
			nLength-=5;
			tempstr=tempstr.Right(nLength);
			if(nLength==128){
				if(cnt_line == 0){
					tempstr = tempstr.Right(34);
					for(int i= 0 ;i< 17; i++){
						tempstr1 = tempstr.Left(2);
						strValue=strtoul(T2A(tempstr.Left(2)),NULL,16);
						pLenCReg[cnt_reg++]=(BYTE)strValue;
						tempstr.Delete(0,2);
					}
					cnt_line++;
				}else if(cnt_line < 6){

					for(int i= 0 ;i< 64; i++){
						tempstr1 = tempstr.Left(2);
						strValue=strtoul(T2A(tempstr.Left(2)),NULL,16);
						pLenCReg[cnt_reg++]=(BYTE)strValue;
						tempstr.Delete(0,2);
					}
					cnt_line++;
				}else {

					for(int i= 0 ;i< 23; i++){
						tempstr1 = tempstr.Left(2);
						strValue=strtoul(T2A(tempstr.Left(2)),NULL,16);
						pLenCReg[cnt_reg++]=(BYTE)strValue;
						tempstr.Delete(0,2);
					}
					cnt_line++;
				}			
			}
		}
		
	}
	file.Close(); 

	if (cnt_reg != otp_lsc_len) {
		uts.log.Error(_T("lsc otp data count wrong."));
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;	
	}

	uts.log.Info(_T("get otp_LSC pass."));

    //upload_lsc:
	if(uts.info.nOtpDBType>=0){
		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC,(char *)pLenCReg, 360) < 0) {
			uts.log.Error(_T("Failed to update LSC Calibration data to DB!!"));
			ret=OTPCALI_ERROR_DB;
			goto out;
		}
	}

	FILE * fp_lsc;
	fp_lsc=fopen("LSCOTPData.txt","wb");
	if(fp_lsc==NULL){
		uts.log.Error(_T("Failed to save LSC Calibration data to txtt!"));
		ret=OTPCALI_ERROR_LSCCALI;
		goto out;		
	}
	else{
		fwrite(pLenCReg,360,1024,fp_lsc);
		uts.log.Debug(_T("LSC: LSCOTPData file save ok!"));
	}

	uts.log.Debug(_T("otp_lsc:"));
	for(int i=0;i<otp_lsc_len/16;i++)
	{
		PrintDebugArray(pLenCReg+i*16, 16);
		uts.log.Debug(_T(""));
	}
	PrintDebugArray(pLenCReg+(otp_lsc_len/16)*16, otp_lsc_len-(otp_lsc_len/16)*16);

	ret= OTPCALI_ERROR_NO;
	
out:

	RELEASE_ARRAY(RAW10Image_input);
	RELEASE_ARRAY(RAW10Image_output);
	FreeLibrary(hdll);

	return SET_ERROR(ret);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA80W::sram_lsc_witeback(void *args)
{
	int ret;	
	
	CString tempstr,tempstr1;

	USES_CONVERSION;

	CStdioFile file;
	
	int nLength,strValue;

	struct LSC_PARAM *lsc = &otp_param.lsc_param;

	CString strRegName;
	//////////////////////////////////////////////////////////////////////////
/*	AF_INT af;
	GetAFData(&af);
	uts.log.Info(_T("af.inf = %d, af.mup = %d"),af.inf,af.mup);

	if (af.inf < 0 || af.mup < 0){
		uts.log.Error(_T("GetAFData fail"));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	//VCM Move INF 
	DLLSetVCM_Initial(dev,uts.info.nVCMType, NULL);
	Sleep(500);
	DLLSetVCM_Move(dev, uts.info.nVCMType, af.inf);
	Sleep(300);*/
	
	//////////////////////////////////////////////////////////////////////////	

/*	if (!file.Open(_T("sram.txt"), CFile::modeRead | CFile::typeText )){
		uts.log.Error(_T("open sram.txt fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}
	int cnt = 0;

	while(file.ReadString(tempstr)&&cnt<=582)
	{
		cnt++;

		if(tempstr.Find(_T("s"),0)!=-1)
		{
			nLength=tempstr.GetLength();
			nLength--;
			tempstr=tempstr.Right(nLength);
			if(nLength==6){
				strValue=strtoul(T2A(tempstr),NULL,16);
				sensor->dev->i2c_write(BIT16_BIT8,(uint16_t)(strValue>>8),(uint16_t)((strValue&0x00ff)));
			}else if(nLength==8){
				strValue=strtoul(T2A(tempstr),NULL,16);
				sensor->dev->i2c_write(BIT16_BIT16,(uint16_t)(strValue>>8),(uint16_t)((strValue&0xffff)));

			}

		}
	}
	file.Close(); */

//	sensor->dev->write_sensor(0x6028,0x4000);
//	sensor->dev->write_sensor(0x602A,0x0B00);
//	sensor->dev->write_sensor(0x6F12,0x0180);

	sensor->dev->write_sensor(0x0B00,0x0180);
	sensor->dev->write_sensor(0x0100,0x0100);
	Sleep(1000);

	dev->GetBufferInfo(m_bufferInfo);

	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}
	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	WORD *RAW10Image_input  = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
	WORD *RAW10Image_output = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_V10_apply"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("LSC: failed to save raw_apply file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);


	if (check_lsc(lsc) < 0) {
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;
	}

	if ( !file.Open( _T("otp.txt"), CFile::modeRead | CFile::typeText )){
		uts.log.Error(_T("open otp.txt fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	int cnt_line,cnt_reg;
	BYTE pLenCReg[360];

	cnt_line=0,cnt_reg=0;

	memset(pLenCReg,0,360);

/*	while(file.ReadString(tempstr)&&cnt_reg<otp_lsc_len){
		if(cnt_line<7&&(tempstr.Find(_T("s0A04"),0) != -1)){
		    nLength=tempstr.GetLength();
			nLength-=5;
			tempstr=tempstr.Right(nLength);
			if(nLength==128){
				if(cnt_line == 0){
					tempstr = tempstr.Right(34);
					for(int i= 0 ;i< 17; i++){
						tempstr1 = tempstr.Left(2);
						strValue=strtoul(T2A(tempstr.Left(2)),NULL,16);
						pLenCReg[cnt_reg++]=(BYTE)strValue;
						tempstr.Delete(0,2);
					}
					cnt_line++;
				}else if(cnt_line < 6){

					for(int i= 0 ;i< 64; i++){
						tempstr1 = tempstr.Left(2);
						strValue=strtoul(T2A(tempstr.Left(2)),NULL,16);
						pLenCReg[cnt_reg++]=(BYTE)strValue;
						tempstr.Delete(0,2);
					}
					cnt_line++;
				}else {

					for(int i= 0 ;i< 23; i++){
						tempstr1 = tempstr.Left(2);
						strValue=strtoul(T2A(tempstr.Left(2)),NULL,16);
						pLenCReg[cnt_reg++]=(BYTE)strValue;
						tempstr.Delete(0,2);
					}
					cnt_line++;
				}			
			}
		}
		
	}
	file.Close(); 

	if (cnt_reg != otp_lsc_len) {
		uts.log.Error(_T("lsc otp data count wrong."));
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;	
	}

	uts.log.Info(_T("get otp_LSC pass."));

    //upload_lsc:
	if(uts.info.nOtpDBType>=0){
		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC,(char *)pLenCReg, 360) < 0) {
			uts.log.Error(_T("Failed to update LSC Calibration data to DB!!"));
			ret=OTPCALI_ERROR_DB;
			goto out;
		}
	}

	FILE * fp_lsc;
	fp_lsc=fopen("LSCOTPData.txt","wb");
	if(fp_lsc==NULL){
		uts.log.Error(_T("Failed to save LSC Calibration data to txtt!"));
		ret=OTPCALI_ERROR_LSCCALI;
		goto out;		
	}
	else{
		fwrite(pLenCReg,360,1024,fp_lsc);
		uts.log.Debug(_T("LSC: LSCOTPData file save ok!"));
	}

	uts.log.Debug(_T("otp_lsc:"));
	for(int i=0;i<otp_lsc_len/16;i++)
	{
		PrintDebugArray(pLenCReg+i*16, 16);
		uts.log.Debug(_T(""));
	}
	PrintDebugArray(pLenCReg+(otp_lsc_len/16)*16, otp_lsc_len-(otp_lsc_len/16)*16);*/

	ret= OTPCALI_ERROR_NO;
	
out:
	RELEASE_ARRAY(RAW10Image_input);
	RELEASE_ARRAY(RAW10Image_output);
	return SET_ERROR(ret);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA80W::get_minfo_from_db(void *args)
{

	MINFO *m = (MINFO *)args;

	if(otp_param.group_for_use==0)
	{
		m->flag           = 0x40;	//0x40:Group1 valid, 0xD0:Group2 valid
		m->minfo1.mid     = 0x42;	//0x42:Holitech
		m->minfo1.lens_id = 0x2C;
		m->minfo1.vcm_id  = 0x3C;
	}
	if(otp_param.group_for_use==1)
	{
		m->flag           = 0xd0;	//0x40:Group1 valid, 0xD0:Group2 valid
		m->minfo2.mid     = 0x42;	//0x42:Holitech
		m->minfo2.lens_id = 0x2C;
		m->minfo1.vcm_id  = 0x3C;
	}
	if(otp_param.group_for_use>1)
		return SET_ERROR(OTPCALI_ERROR_INVALID_GROUP);

	return sizeof(MINFO);
}
int OtpCali_S5K4H8_MA80W::get_otp_data_from_db(void *args)
{
	OTPData *otp = (OTPData*)args;
	USES_CONVERSION;

	//Reserve1
	memset(otp->Reserve1,0,sizeof(otp->Reserve1));

	if(otp_param.group_for_use==0)
	{
		//LSC
		otp->lsc.lsc1.flag =0x00;   //group1:0x00   
		int ret = get_lsc_from_raw_data(otp->lsc.lsc1.lsc, otp_lsc_len);
		if (ret < 0) {return ret;}

		int sum = 0;
		sum = CheckSum(&otp->lsc.lsc1.lsc, otp_lsc_len);
		otp->lscsum.lscsum1=(sum % 0xFF + 1);

		uts.log.Info(_T("otp->lscsum.lscsum1"),otp->lscsum.lscsum1);

		//module info
		ret = get_minfo_from_db(&otp->minfo);
		if (ret < 0) { return ret;}

		//WB
		WB_DATA_UCHAR wb[2];
		ret = get_wb_from_raw_data(&wb[0], &wb[1]);
		if (ret < 0) { return ret;}

#if 0
		//Golden
		uts.log.Info(_T("wb get pass,start get golden"));
		ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
			3, (char *)& wb[1], 4);
		if (ret < 0) return ret;
		uts.log.Info(_T("wb golden get pass"));
#endif
		

		WB_RATIO ratio, ratio_g;
		get_wb_ratio(&wb[0], &ratio, 512);
		get_wb_ratio(&wb[1], &ratio_g, 512);

		uts.log.Info(_T("r_gr=0x%04x,b_gr=0x%04x,gb_gr=0x%04x"),ratio.r_gr,ratio.b_gr,ratio.gb_gr);

		otp->minfo.minfo1.wb.r_g_msb   =(BYTE)(ratio.r_gr>>2);
		otp->minfo.minfo1.wb.b_g_msb   =(BYTE)(ratio.b_gr>>2);
		otp->minfo.minfo1.wb.gb_gr_msb =(BYTE)(ratio.gb_gr>>2);

		otp->minfo.minfo1.wb.awb_lsb=(BYTE)(((ratio.r_gr&0x03)<<6)+((ratio.b_gr&0x03)<<4)+((ratio.gb_gr&0x03)<<2));

		sum = CheckSum(&otp->minfo.minfo1, sizeof(otp->minfo.minfo1)-1);
		otp->minfo.minfo1.infosum=(sum % 0xFF + 1);

		//AF
		AF_INT af;
		GetAFData(&af);
		uts.log.Info(_T("befor offset:af.inf = %d ,af.mup = %d."),af.inf,af.mup);
		af.inf-=OFFSETINF;
		af.mup-=OFFSETMACRO;
		uts.log.Info(_T("after offset:af.inf = %d ,af.mup = %d."),af.inf,af.mup);

		otp->af.flag = 0x40;
		otp->af.af1.inf_msb =(BYTE)(af.inf>>2);
		otp->af.af1.mup_msb =(BYTE)(af.mup>>2);
		otp->af.af1.af_lsb  =(BYTE)(((af.inf&0x03)<<6)+((af.mup&0x03)<<4));

		sum = 0;
		sum = CheckSum(&otp->af.af1,sizeof(otp->af.af1)-1);
		otp->af.af1.sum=(sum % 0xFF + 1);
	}

	if(otp_param.group_for_use==1)
	{
		//LSC
		otp->lsc.lsc2.flag =0xff;    //group2:0xff
		int ret = get_lsc_from_raw_data(otp->lsc.lsc2.lsc, otp_lsc_len);
		if (ret < 0) {return ret;}

		int sum = 0;
		sum = CheckSum(&otp->lsc.lsc2.lsc, otp_lsc_len);
		otp->lscsum.lscsum2=(sum % 0xFF + 1);

		uts.log.Info(_T("otp->lscsum.lscsum2=0x%02x"),otp->lscsum.lscsum2);

		//module info
		ret = get_minfo_from_db(&otp->minfo);
		if (ret < 0) { return ret;}

		//WB
		WB_DATA_UCHAR wb[2];
		ret = get_wb_from_raw_data(&wb[0], &wb[1]);
		if (ret < 0) { return ret;}
		uts.log.Info(_T("wb get pass"));
		
		//Golden
#if 0
		uts.log.Info(_T("start get golden"));
		ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
		3, (char *)& wb[1], 4);
		if (ret < 0) return ret;
		uts.log.Info(_T("wb golden get pass"));
#endif
		WB_RATIO ratio, ratio_g;
		get_wb_ratio(&wb[0], &ratio, 512);
		get_wb_ratio(&wb[1], &ratio_g, 512);

		uts.log.Info(_T("r_gr=0x%04x,b_gr=0x%04x,gb_gr=0x%04x"),ratio.r_gr,ratio.b_gr,ratio.gb_gr);

		otp->minfo.minfo2.wb.r_g_msb   =(BYTE)(ratio.r_g>>2);
		otp->minfo.minfo2.wb.b_g_msb   =(BYTE)(ratio.b_g>>2);
		otp->minfo.minfo2.wb.gb_gr_msb =(BYTE)(ratio.gb_gr>>2);

		otp->minfo.minfo2.wb.awb_lsb=(BYTE)(((ratio.r_g&0x03)<<6)+((ratio.b_g&0x03)<<4)+((ratio.gb_gr&0x03)<<4));

		sum=0;
		sum = CheckSum(&otp->minfo.minfo2, sizeof(otp->minfo.minfo2)-1);
		otp->minfo.minfo2.infosum=(sum % 0xFF + 1);

		//AF
		AF_INT af;
		GetAFData(&af);
		uts.log.Info(_T("befor offset:af.inf = %d ,af.mup = %d."),af.inf,af.mup);
		af.inf+=OFFSETINF;
		af.mup+=OFFSETMACRO;
		uts.log.Info(_T("after offset:af.inf = %d ,af.mup = %d."),af.inf,af.mup);

		otp->af.flag = 0x40;
		otp->af.af2.inf_msb =(BYTE)(af.inf>>2);
		otp->af.af2.mup_msb =(BYTE)(af.mup>>2);
		otp->af.af2.af_lsb  =(BYTE)(((af.inf&0x03)<<6)+((af.mup&0x03)<<4));

		sum = 0;
		sum = CheckSum(&otp->af.af2, sizeof(otp->af.af2)-1);
		otp->af.af2.sum=(sum % 0xFF + 1);
	}
	if(otp_param.group_for_use>1)
		return SET_ERROR(OTPCALI_ERROR_INVALID_GROUP);

	//Reserve2
	memset(otp->Reserve2,0,sizeof(otp->Reserve2));

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA80W::get_uniform_otp_data(void *in, void *out, int maxlen)
{
  /*  OTPData *otp = (OTPData*)in;
    OtpDataHeader *hdr = (OtpDataHeader*)out;
    hdr->len = 0;

    for (int i = 0; i < OtpDataType_Num; i++) {
        OtpDataItem *item = (OtpDataItem *)&hdr->data[hdr->len];
        item->type = i;
        item->len = 0;

        switch (item->type) {
        case OtpDataType_MInfo:
            {
				OtpDataMInfo *minfo = (OtpDataMInfo*)item->data;
				minfo->tm = minfo->raw + 3*offsetof(MINFO, flag);
				minfo->tm_len = 3;
				minfo->raw_len = sizeof(otp->minfo);
				memcpy(minfo->raw, &otp->minfo, minfo->raw_len);
				item->len = sizeof(OtpDataMInfo) + minfo->raw_len;
            }
            break;
        case OtpDataType_WB:
            {
                OtpDataWB *wb = (OtpDataWB*)item->data;
                wb->has_golden = true;
                wb->ratio.dr_g = get_be_val(otp->wb.wb1.r_g, sizeof(otp->wb.wb1.r_g))/1023.0;
                wb->ratio.db_g = get_be_val(otp->wb.wb1.b_g, sizeof(otp->wb.wb1.b_g))/1023.0;
                wb->ratio_g.dr_g = get_be_val(otp->wb.wb1.r_g_g, sizeof(otp->wb.wb1.r_g_g))/1023.0;
                wb->ratio_g.db_g = get_be_val(otp->wb.wb1.b_g_g, sizeof(otp->wb.wb1.b_g_g))/1023.0;
                wb->raw_len = sizeof(otp->wb);
                memcpy(wb->raw, &otp->wb, wb->raw_len);
                item->len = sizeof(OtpDataWB) + wb->raw_len;
                item->sum_len = sizeof(otp->wb.wb1.sum);
                item->sum = get_be_val(&otp->wb.wb1.sum, sizeof(otp->wb.wb1.sum));
                item->sum_sta = wb->raw + offsetof(WB, wb1.r_g);
                item->sum_calc_len = 12;
            }
            break;
        case OtpDataType_AF:
            {
                OtpDataAF *af = (OtpDataAF*)item->data;
                af->af.start = -1;
                af->af.inf = get_be_val(otp->af.af1.inf, sizeof(otp->af.af1.inf));
                af->af.mup = get_be_val(otp->af.af1.mup, sizeof(otp->af.af1.mup));
                af->raw_len = sizeof(otp->af);
                memcpy(af->raw, &otp->af, af->raw_len);
                item->len = sizeof(OtpDataAF) + af->raw_len;
                item->sum_len = sizeof(otp->af.af1.sum);
                item->sum = get_be_val(&otp->af.af1.sum, sizeof(otp->af.af1.sum));
                item->sum_sta = af->raw + offsetof(AF, af1.vcmdir);
                item->sum_calc_len = 6;
            }
            break;
        case OtpDataType_LSC:
            {
                OtpDataLSC *lsc = (OtpDataLSC*)item->data;
                lsc->lsc = lsc->raw + offsetof(LSC, lsc);
                lsc->raw_len = sizeof(otp->lsc);
                memcpy(lsc->raw, &otp->lsc, lsc->raw_len);
                item->len = sizeof(OtpDataLSC) + lsc->raw_len;
				item->sum_len = 2;
			//	item->sum = get_be_val(otp->lsc.sum, sizeof(otp->lsc.sum));
                item->sum_sta = lsc->lsc;
                item->sum_calc_len = otp_lsc_len;
            }
            break;
        default:
            continue;
        }
        hdr->len += sizeof(OtpDataItem) + item->len;
        if (hdr->len + (int)sizeof(OtpDataHeader) > maxlen) {
            return -1;
        }
    }
    return sizeof(OtpDataHeader) + hdr->len;*/
	return 0;
}

//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA80W::get_otp_group(void)
{
	OTPData *otp = (OTPData*)otp_data_in_sensor;

	int ret=0;

	if(otp->minfo.flag==0x40&&otp->lsc.lsc2.flag==0x00)  ret=0;
	else if(otp->minfo.flag==0xd0&&otp->lsc.lsc2.flag==0xff)  ret=1;
	else ret=-1;

	return ret;
}
//-------------------------------------------------------------------------------------------------


}
