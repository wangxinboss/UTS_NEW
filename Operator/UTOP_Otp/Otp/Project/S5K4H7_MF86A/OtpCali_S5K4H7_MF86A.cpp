#include "StdAfx.h"
#include "OtpCali_S5K4H7_MF86A.h"
//#include "..\..\3rdparty\LSC_Lib\Samsung\S5K4H7\LSC.h"

//#pragma  comment(lib,"GLSCv10_LIB_v140.lib")
#define OFFSETMACRO  56
#define OFFSETINF  124

using namespace UTS::Algorithm::Image;

namespace OtpCali_S5K4H7_MF86A {
//-------------------------------------------------------------------------------------------------
OtpCali_S5K4H7_MF86A::OtpCali_S5K4H7_MF86A(BaseDevice *dev) : SamsungOtp(dev)
{
	otp_type = OTP_TYPE_OTP;
    otp_data_len = sizeof(OTPData);
    otp_lsc_len = 360;//MTK_LSC_LEN;
    otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF86A::do_prog_otp()
{
	//Dump OTP-IN-DB Data==========================================
		uint8_t Otp_data_DB_data[473] = {0};
		for(int i = 0 ; i < 473 ; i++)
		{
			Otp_data_DB_data[i] = otp_data_in_db[i];
		}

		FILE *input = fopen("otp-buffer_OTP-data_IN_DB.txt","w");
		if(input != NULL)
		{
			for(int i=0;i<473;i++)
			{
				fprintf(input,"Otp data [0x%04x]:[0x%02x]\n", 0x0a04+i,Otp_data_DB_data[i]);
			}
		
		}
		fclose(input);

	//return 0;	
	//lsc

 	if(sensor->do_prog_otp(0x0,0x0A3D,otp_data_in_db,1))
 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	if(sensor->do_prog_otp(0x01,0x0A04,otp_data_in_db+1+64*0,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	if(sensor->do_prog_otp(0x02,0x0A04,otp_data_in_db+1+64*1,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	if(sensor->do_prog_otp(0x03,0x0A04,otp_data_in_db+1+64*2,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	if(sensor->do_prog_otp(0x04,0x0A04,otp_data_in_db+1+64*3,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	if(sensor->do_prog_otp(0x05,0x0A04,otp_data_in_db+1+64*4,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	if(sensor->do_prog_otp(0x06,0x0A04,otp_data_in_db+1+64*5,40))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	/*uts.log.Info(_T("26"));
	if(!sensor->do_prog_otp(0x07,0x0A04,otp_data_in_db+1+64*6,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	uts.log.Info(_T("31"));
	if(!sensor->do_prog_otp(0x08,0x0A04,otp_data_in_db+1+64*7,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	uts.log.Info(_T("27"));
	if(!sensor->do_prog_otp(0x09,0x0A04,otp_data_in_db+1+64*8,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	uts.log.Info(_T("28"));
	if(!sensor->do_prog_otp(0x0A,0x0A04,otp_data_in_db+1+64*9,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	uts.log.Info(_T("29"));
	if(!sensor->do_prog_otp(0x0B,0x0A04,otp_data_in_db+1+64*10,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	uts.log.Info(_T("30"));*/


	//MINFO
	if(sensor->do_prog_otp(21,0x0A04,otp_data_in_db+361,62))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(sensor->do_prog_otp(22,0x0A04,otp_data_in_db+423,50))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	
	Sleep(100);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF86A::get_otp_data_from_sensor(void *args)
{
    OTPData *otp = (OTPData*)args;
	BOOL b2ndSet = FALSE;
//	char lscbuf[360];

	uts.log.Info(_T("In Get Otp data functon"));
	if(sensor->do_read_otp(0x0,0x0A3D,&otp->lsc.lsc_flag,1))
	{
		uts.log.Info(_T("Read OTP data fail!!"));
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	}
	Sleep(50);
	if(sensor->do_read_otp(0x01,0x0A04,otp->lsc.lsc1.lsc+64*0,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(50);
	if(sensor->do_read_otp(0x02,0x0A04,otp->lsc.lsc1.lsc+64*1,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(50);
	if(sensor->do_read_otp(0x03,0x0A04,otp->lsc.lsc1.lsc+64*2,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(100);
	if(sensor->do_read_otp(0x04,0x0A04,otp->lsc.lsc1.lsc+64*3,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(50);
	if(sensor->do_read_otp(0x05,0x0A04,otp->lsc.lsc1.lsc+64*4,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(50);
	if(sensor->do_read_otp(0x06,0x0A04,otp->lsc.lsc1.lsc+64*5,40))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(50);

	

	//MINFO
	if(sensor->do_read_otp(21,0x0A04,&otp->minfo,62))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	Sleep(50);

	if(sensor->do_read_otp(22,0x0A04,&otp->wb.wb3100.R_avgL,50))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF86A::LscCali(void *args)
{
	uts.log.Debug(_T("Start lsc"));
	
		OTPData *otp = (OTPData*)args;

	int ret;	

	const char *pResult,*pText;
	
	CString tempstr,temp_line;

	USES_CONVERSION;
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	using namespace UTS::Algorithm::Image;

	FILE* fp_bayer_setting = fopen("bayer_setting.xml","rt");
	FILE* fp_binary_dat    = fopen("binary.dat","rt");
	FILE* fp_Run_bat = fopen("S5K4H7_Run.bat","rt");   
		
	CStdioFile file;
	
	int nLength,strValue;
	int cnt_line,cnt_reg;
	BYTE pLenCReg[360];

	struct LSC_PARAM *lsc = &otp_param.lsc_param;

	//////////////////////////////////////////////////////////////////////////

	dev->GetBufferInfo(m_bufferInfo);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;	
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	WORD *RAW10Image_input = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
	//unsigned char *RAW10Image_input = new unsigned char[m_bufferInfo.nWidth* m_bufferInfo.nHeight *2];
	//WORD *RAW10Image_output= new WORD[3264* 2448];
	WORD *RAW10Image_output= new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	//儲存要處理的檔案
	if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_V10_input"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("LSC: failed to save raw_input file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	//確認相關檔案是否都存在
	if(fp_bayer_setting==NULL)
	{
		uts.log.Error(_T("bayer_setting.xml Open Error."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	if(fp_binary_dat==NULL)
	{
		uts.log.Error(_T("binary.dat Open Error."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	if(fp_Run_bat==NULL)
	{
		uts.log.Error(_T("S5K4H7_Run.bat Open Error."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}
	
	fclose(fp_bayer_setting);
	fclose(fp_binary_dat);
	fclose(fp_Run_bat);

	//運行 S5K4H7.bat
	ShellExecute (NULL,L"open",_T("S5K4H7_Run.bat"),NULL,NULL,SW_HIDE);
	Sleep(3000);


	if ( !file.Open( _T("otp.txt"), CFile::modeRead | CFile::typeText ))
	{
		uts.log.Error(_T("open otp.txt fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	cnt_line=0,cnt_reg=0;

	memset(pLenCReg,0,360);

	while(file.ReadString(tempstr)&&cnt_reg<otp_lsc_len)
	{
		if(cnt_line>70&&(tempstr.Find(_T("s"),0) != -1))
		{
		    nLength=tempstr.GetLength();
			nLength--;
			tempstr=tempstr.Right(nLength);
			if(nLength==6)
			{
				strValue=strtoul(T2A(tempstr),NULL,16);
				if((strValue>>8)>0x0A03&&(strValue>>8)<0x0A44)
				    pLenCReg[cnt_reg++]=(BYTE)strValue;
			   //	sensor->dev->write_sensor((WORD)(strValue>>8),strValue%0xff);
			}
		}
		cnt_line++;
	}
	file.Close(); 

	if (cnt_reg != otp_lsc_len) {
		uts.log.Error(_T("lsc otp data count wrong."));
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;	
	}

	uts.log.Info(_T("get otp_LSC pass."));

	 //upload_lsc:
	if(uts.info.nOtpDBType>=0)
	{
		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC,(char *)pLenCReg, 360) < 0) 
		{
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
		fwrite(pLenCReg,360,1,fp_lsc);
		//fprintf(fp_lsc,"%s",pLenCReg);
		fclose(fp_lsc);
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
	
#if 0
	//***************************************************************************************************
	unsigned short *buff = new unsigned short[3280 * 2464];
	FILE *fpRaw = fopen("Input_3280x2464.Raw", "rb");
      if (fpRaw == NULL) {
   		uts.log.Error(_T("Not find LSC_Cali_V10_input.Raw...."));
		return -1;
	}
	fread(buff, 3280*2464, 2, fpRaw);
	fclose(fpRaw);

	if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_ImageBugg"),(BYTE*)buff,3280*2,2464))
	{
		uts.log.Error(_T("LSC: failed to save raw_input file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}
//********************************************************************************************************

	uts.log.Error(_T("LSC_Lib : bayer setting initial!!"));
	pResult = Initialize(NULL, "bayer_setting.xml", 0, "binary.dat");
	if (pResult == 0)
	{
		uts.log.Debug(_T("LSC_Lib :Ready to Run...."));
		pResult = Run(RAW10Image_input);
		
		
		//pResult = Run(buff);
	}
	else
	{
		uts.log.Error(_T("LSC_Lib : initial fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	if (pResult == 0)
	{
		uts.log.Info(_T("LSC_Lib : run pass."));
	}
	else
	{
		uts.log.Error(_T("LSC_Lib : run fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	uts.log.Error(_T("LSC_Lib : Ready to GetOutSetFileFormat~~"));
	pResult = GetOutSetFileFormat(pText, 1);

	if (pResult == 0)
	{
		uts.log.Info(_T("LSC_Lib : GetOutSetFileFormat pass."));
	}
	else
	{
		uts.log.Error(_T("LSC_Lib : GetOutSetFileFormat fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	

//	write_reg(pText,360);

	FILE *fp_sram = fopen("sram.txt", "wt");
	fprintf(fp_sram,"%s",pText);
	fclose(fp_sram);

	pResult = GetOutNVMWrite(pText, 1, 1);

	if (pResult == 0)
	{
		uts.log.Info(_T("LSC_Lib : GetOutNVMWrite pass."));
	}
	else
	{
		uts.log.Error(_T("LSC_Lib : GetOutNVMWrite fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	FILE *fp_otp = fopen("otp.txt", "wt");
	fprintf(fp_otp,"%s",pText);
	fclose(fp_otp);

	pResult = GetResult(RAW10Image_input, RAW10Image_output);

	if (pResult == 0)
	{
		uts.log.Info(_T("LSC_Lib : GetResult pass."));
	}
	else
	{
		uts.log.Error(_T("LSC_Lib : GetResult fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_V10_output"),(BYTE*)RAW10Image_output,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("LSC: failed to save raw_output file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	sensor->dev->write_sensor(0x0100,0x00);
/*	sensor->dev->write_sensor(0x0344,0x00);
	sensor->dev->write_sensor(0x0345,0x08);
	sensor->dev->write_sensor(0x0348,0x0C);
	sensor->dev->write_sensor(0x0349,0xC7);
	sensor->dev->write_sensor(0x0346,0x00);
	sensor->dev->write_sensor(0x0347,0x08);
	sensor->dev->write_sensor(0x034A,0x09);
	sensor->dev->write_sensor(0x034B,0x97);
	sensor->dev->write_sensor(0x034C,0x0C);
	sensor->dev->write_sensor(0x034D,0xC0);
	sensor->dev->write_sensor(0x034E,0x09);
	sensor->dev->write_sensor(0x034F,0x90);*/
	/*s034400	
		s034508 
		s03480C	
		s0349C7
		s034600	
		s034708
		s034A09	
		s034B97
		s034C0C	
		s034DC0
		s034E09	
		s034F90*/

	if (!file.Open(_T("sram.txt"), CFile::modeRead | CFile::typeText ))
	{
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
			if(nLength==6)
			{
				strValue=strtoul(T2A(tempstr),NULL,16);
				sensor->dev->write_sensor((WORD)(strValue>>8),(BYTE)strValue&0xff);
			}

		}
	}
	file.Close(); 

	sensor->dev->write_sensor(0x3400,0x00);
	sensor->dev->write_sensor(0x0B00,0x01);
	Sleep(500);
	sensor->dev->write_sensor(0x0100,0x01);

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
	}

	if ( !file.Open( _T("otp.txt"), CFile::modeRead | CFile::typeText ))
	{
		uts.log.Error(_T("open otp.txt fail."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	cnt_line=0,cnt_reg=0;

	memset(pLenCReg,0,360);

	while(file.ReadString(tempstr)&&cnt_reg<otp_lsc_len)
	{
		if(cnt_line>70&&(tempstr.Find(_T("s"),0) != -1))
		{
		    nLength=tempstr.GetLength();
			nLength--;
			tempstr=tempstr.Right(nLength);
			if(nLength==6)
			{
				strValue=strtoul(T2A(tempstr),NULL,16);
				if((strValue>>8)>0x0A03&&(strValue>>8)<0x0A44)
				    pLenCReg[cnt_reg++]=(BYTE)strValue;
			   //	sensor->dev->write_sensor((WORD)(strValue>>8),strValue%0xff);
			}
		}
		cnt_line++;
	}
	file.Close(); 

	if (cnt_reg != otp_lsc_len) {
		uts.log.Error(_T("lsc otp data count wrong."));
		ret = OTPCALI_ERROR_LSCCALI;
		goto out;	
	}

	uts.log.Info(_T("get otp_LSC pass."));

    //upload_lsc:
	if(uts.info.nOtpDBType>=0)
	{
		if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC,(char *)pLenCReg, 360) < 0) 
		{
			uts.log.Error(_T("Failed to update LSC Calibration data to DB!!"));
			ret=OTPCALI_ERROR_DB;
			goto out;
		}
	}

	//FILE * fp_lsc;
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

#endif
	
out:

	RELEASE_ARRAY(RAW10Image_input);
	RELEASE_ARRAY(RAW10Image_output);

	return SET_ERROR(ret);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF86A::get_minfo_from_db(void *args)
{
	USES_CONVERSION;
    MINFO *m = (MINFO *)args;

	m->minfo1.ModuleID_L = 0x09;
	m->minfo1.ModuleID_H = 0x00;
	
	SYSTEMTIME stime;
	time_t time;
	if (otpDB->GetOtpBurnTime(mid, &time) < 0)
	{
		GetLocalTime(&stime);
		m->minfo1.Day = (uint8_t)stime.wDay;
		m->minfo1.Month =  (uint8_t)stime.wMonth;
		m->minfo1.Year_L = 0x14; 
		m->minfo1.Year_H = stime.wYear % 100;
	}else
	{
		struct tm today;
		_localtime64_s( &today, &time );

		m->minfo1.Day =  (uint8_t)today.tm_mday;
		m->minfo1.Month = (uint8_t)today.tm_mon + 1;
		m->minfo1.Year_L = 0x14; 
		m->minfo1.Year_H = today.tm_year % 100;
	}

	m->minfo1.SensorID_L = 0x61;
	m->minfo1.SensorID_H = 0x00;
	m->minfo1.LensID_L = 0x91;
	m->minfo1.LensID_H = 0x00;
	m->minfo1.VCMID_L = 0x00;
	m->minfo1.VCMID_H = 0x00;
	m->minfo1.flag = 0x01;	

	int sum = CheckSum(&m->minfo1.ModuleID_L, 12);
	uts.log.Info(_T("Module Info sum = %d"),sum);

    	put_be_val(sum % 0xFF , &m->minfo1.sum, sizeof(m->minfo1.sum));

    return sizeof(MINFO);

}
int OtpCali_S5K4H7_MF86A::get_otp_data_from_db(void *args)
{
	OTPData *otp = (OTPData*)args;
	USES_CONVERSION;
	int sum;
	//module info
	int ret = get_minfo_from_db(&otp->minfo);

	//LSC
	otp->lsc.lsc_flag=0x01;
	ret = get_lsc_from_raw_data(otp->lsc.lsc1.lsc, sizeof(otp->lsc.lsc1.lsc));
	if (ret < 0) return ret;

	int GoldenR,GoldenGr,GoldenGb,GoldenB;
	double NormalRG,NormalBG,GoldenRG,GoldenBG;
	double NormalD,GoldenD;
	
	//WB 5100k
	WB_DATA_USHORT wb[2];
	ret = get_wb_from_raw_data(&wb[0], &wb[1],0); //5100k
	if (ret < 0) 
	{ 
		uts.log.Info(_T("get wb 5100k fail"));
		return ret;
	}
	uts.log.Info(_T("get wb 5100k success"));

	uts.log.Info(_T("Get 5100k WB[0].R /Gr / Gb / B = %d %d %d %d") ,  wb[0].R , wb[0].Gr, wb[0].Gb,wb[0].B);

	otp->wb.wb5100.R_avgL = (wb[0].R -64)%256;
	otp->wb.wb5100.R_avgH = (wb[0].R -64)/256;
	otp->wb.wb5100.Gr_avgL = (wb[0].Gr -64)%256;
	otp->wb.wb5100.Gr_avgH = (wb[0].Gr -64)/256;
	otp->wb.wb5100.Gb_avgL = (wb[0].Gb -64)%256;
	otp->wb.wb5100.Gb_avgH = (wb[0].Gb -64)/256;
	otp->wb.wb5100.B_avgL = (wb[0].B -64)%256;
	otp->wb.wb5100.B_avgH = (wb[0].B -64)/256;

	GoldenR = 232;
	GoldenGr= 428;
	GoldenGb = 428;
	GoldenB= 259;
	otp->wb.wb5100.GoldenR_avgL = GoldenR%256;
	otp->wb.wb5100.GoldenR_avgH = GoldenR/256;
	otp->wb.wb5100.GoldenGr_avgL = GoldenGr%256;
	otp->wb.wb5100.GoldenGr_avgH = GoldenGr/256;
	otp->wb.wb5100.GoldenGb_avgL = GoldenGb%256;
	otp->wb.wb5100.GoldenGb_avgH = GoldenGb/256;
	otp->wb.wb5100.GoldenB_avgL = GoldenB%256;
	otp->wb.wb5100.GoldenB_avgH = GoldenB/256;;
	uts.log.Info(_T("Get Golden 5100k WB[0].R /Gr / Gb / B = %d %d %d %d") ,  GoldenR , GoldenGr, GoldenGb,GoldenB);

	NormalRG = (double)(wb[0].R-64) /(double) ((wb[0].Gr+wb[0].Gb-128)/2);
	NormalBG = (double)(wb[0].B-64) /(double) ((wb[0].Gr+wb[0].Gb-128)/2);
	GoldenRG = (double)GoldenR / (double)((GoldenGr+GoldenGb)/2);
	GoldenBG = (double)GoldenB / (double)((GoldenGr+GoldenGb)/2);
	uts.log.Info(_T("Get Golden 5100k NormalRG /NormalBG / GoldenRG / GoldenBG = %.5f %.5f %.5f %.5f") ,  NormalRG , NormalBG, GoldenRG,GoldenBG);

	if((NormalRG > (GoldenRG*1.1))||(NormalRG<(GoldenRG*0.9))||(NormalBG > (GoldenBG*1.1))||(NormalBG<(GoldenBG*0.9)))
	{
		uts.log.Error(_T("AWB 5100k Spec Fail!!"));
		ret = -1;
		return ret;
	}
		
	int iRG_Value, iBG_Value;
	iRG_Value = 1000;
	iBG_Value = 1003;
	otp->wb.wb5100.R_G_L = iRG_Value % 256;
	otp->wb.wb5100.R_G_H = iRG_Value / 256;
	otp->wb.wb5100.B_G_L =  iBG_Value % 256;
	otp->wb.wb5100.B_G_H =  iBG_Value / 256;
    	otp->wb.wb5100.flag_wb = 0x01;	
	sum = CheckSum(&otp->wb.wb5100.R_avgL , 16);
	put_be_val(sum % 0xFF , &otp->wb.wb5100.sum_wb, sizeof(otp->wb.wb5100.sum_wb));
	otp->wb.wb5100.flag_LightSource = 1;	
	sum = CheckSum(&otp->wb.wb5100.R_G_L , 4);
	put_be_val(sum % 0xFF , &otp->wb.wb5100.sum_LightSource, sizeof(otp->wb.wb5100.sum_LightSource));
	
	//WB 4000k
	//WB_DATA_USHORT wb[2];
	ret = get_wb_from_raw_data(&wb[0], &wb[1],1); //4000k
	if (ret < 0) 
	{ 
		uts.log.Info(_T("get wb 4000k fail"));
		return ret;
	}
	uts.log.Info(_T("get wb 4000k success"));

	uts.log.Info(_T("Get 4000k WB[0].R /Gr / Gb / B = %d %d %d %d") ,  wb[0].R , wb[0].Gr, wb[0].Gb,wb[0].B);

	otp->wb.wb4000.R_avgL = (wb[0].R -64)%256;
	otp->wb.wb4000.R_avgH = (wb[0].R -64)/256;
	otp->wb.wb4000.Gr_avgL = (wb[0].Gr -64)%256;
	otp->wb.wb4000.Gr_avgH = (wb[0].Gr -64)/256;
	otp->wb.wb4000.Gb_avgL = (wb[0].Gb -64)%256;
	otp->wb.wb4000.Gb_avgH = (wb[0].Gb -64)/256;
	otp->wb.wb4000.B_avgL = (wb[0].B -64)%256;
	otp->wb.wb4000.B_avgH = (wb[0].B -64)/256;

	GoldenR = 274;
	GoldenGr= 429;
	GoldenGb = 429;
	GoldenB= 233;
	
	otp->wb.wb4000.GoldenR_avgL = GoldenR%256;
	otp->wb.wb4000.GoldenR_avgH = GoldenR/256;
	otp->wb.wb4000.GoldenGr_avgL = GoldenGr%256;
	otp->wb.wb4000.GoldenGr_avgH = GoldenGr/256;
	otp->wb.wb4000.GoldenGb_avgL = GoldenGb%256;
	otp->wb.wb4000.GoldenGb_avgH = GoldenGb/256;
	otp->wb.wb4000.GoldenB_avgL = GoldenB%256;
	otp->wb.wb4000.GoldenB_avgH = GoldenB/256;

	uts.log.Info(_T("Get Golden 4000k WB[0].R /Gr / Gb / B = %d %d %d %d") ,  GoldenR , GoldenGr, GoldenGb,GoldenB);

	NormalRG = (double)(wb[0].R-64) / (double)((wb[0].Gr+wb[0].Gb-128)/2);
	NormalBG = (double)(wb[0].B-64) /(double) ((wb[0].Gr+wb[0].Gb-128)/2);
	GoldenRG = (double)GoldenR / (double)((GoldenGr+GoldenGb)/2);
	GoldenBG = (double)GoldenB / (double)((GoldenGr+GoldenGb)/2);
	uts.log.Info(_T("Get Golden 5100k NormalRG /NormalBG / GoldenRG / GoldenBG = %.5f %.5f %.5f %.5f") ,  NormalRG , NormalBG, GoldenRG,GoldenBG);

	if((NormalRG > (GoldenRG*1.1))||(NormalRG<(GoldenRG*0.9))||(NormalBG > (GoldenBG*1.1))||(NormalBG<(GoldenBG*0.9)))
	{
		uts.log.Error(_T("AWB 4000k Spec Fail!!"));
		ret = -1;
		return ret;
	}
	
	iRG_Value = 999;
	iBG_Value = 995;
	otp->wb.wb4000.R_G_L = iRG_Value % 256;
	otp->wb.wb4000.R_G_H = iRG_Value / 256;
	otp->wb.wb4000.B_G_L =  iBG_Value % 256;
	otp->wb.wb4000.B_G_H =  iBG_Value / 256;
    	otp->wb.wb4000.flag_wb = 0x01;	
	sum = CheckSum(&otp->wb.wb4000.R_avgL , 16);
	put_be_val(sum % 0xFF , &otp->wb.wb4000.sum_wb, sizeof(otp->wb.wb4000.sum_wb));
	otp->wb.wb4000.flag_LightSource = 1;	
	sum = CheckSum(&otp->wb.wb4000.R_G_L , 4);
	put_be_val(sum % 0xFF , &otp->wb.wb4000.sum_LightSource, sizeof(otp->wb.wb4000.sum_LightSource));

	//WB 3100k
	//WB_DATA_USHORT wb[2];
	ret = get_wb_from_raw_data(&wb[0], &wb[1],2); //3100k
	if (ret < 0) 
	{ 
		uts.log.Info(_T("get wb 3100k fail"));
		return ret;
	}
	uts.log.Info(_T("get wb 3100k success"));

	uts.log.Info(_T("Get 3100k WB[0].R /Gr / Gb / B = %d %d %d %d") ,  wb[0].R , wb[0].Gr, wb[0].Gb,wb[0].B);

	otp->wb.wb3100.R_avgL = (wb[0].R -64)%256;
	otp->wb.wb3100.R_avgH = (wb[0].R -64)/256;
	otp->wb.wb3100.Gr_avgL = (wb[0].Gr -64)%256;
	otp->wb.wb3100.Gr_avgH = (wb[0].Gr -64)/256;
	otp->wb.wb3100.Gb_avgL = (wb[0].Gb -64)%256;
	otp->wb.wb3100.Gb_avgH = (wb[0].Gb -64)/256;
	otp->wb.wb3100.B_avgL = (wb[0].B -64)%256;
	otp->wb.wb3100.B_avgH = (wb[0].B -64)/256;

	GoldenR = 328;
	GoldenGr= 430;
	GoldenGb = 429;
	GoldenB= 169;
	
	otp->wb.wb3100.GoldenR_avgL = GoldenR%256;
	otp->wb.wb3100.GoldenR_avgH = GoldenR/256;
	otp->wb.wb3100.GoldenGr_avgL = GoldenGr%256;
	otp->wb.wb3100.GoldenGr_avgH = GoldenGr/256;
	otp->wb.wb3100.GoldenGb_avgL = GoldenGb%256;
	otp->wb.wb3100.GoldenGb_avgH = GoldenGb/256;
	otp->wb.wb3100.GoldenB_avgL = GoldenB%256;
	otp->wb.wb3100.GoldenB_avgH = GoldenB%256;
	uts.log.Info(_T("Get Golden 3100k WB[0].R /Gr / Gb / B = %d %d %d %d") ,  GoldenR , GoldenGr, GoldenGb,GoldenB);

	NormalRG = (double)(wb[0].R-64) / (double)((wb[0].Gr+wb[0].Gb-128)/2);
	NormalBG =(double)(wb[0].B-64) /(double) ((wb[0].Gr+wb[0].Gb-128)/2);
	GoldenRG = (double)GoldenR / (double)((GoldenGr+GoldenGb)/2);
	GoldenBG = (double)GoldenB / (double)((GoldenGr+GoldenGb)/2);
	uts.log.Info(_T("Get Golden 5100k NormalRG /NormalBG / GoldenRG / GoldenBG = %.5f %.5f %.5f %.5f") ,  NormalRG , NormalBG, GoldenRG,GoldenBG);

	if((NormalRG > (GoldenRG*1.1))||(NormalRG<(GoldenRG*0.9))||(NormalBG > (GoldenBG*1.1))||(NormalBG<(GoldenBG*0.9)))
	{
		uts.log.Error(_T("AWB 3100k Spec Fail!!"));
		ret = -1;
		return ret;
	}
	
	iRG_Value = 997;
	iBG_Value = 1009;
	otp->wb.wb3100.R_G_L = iRG_Value % 256;
	otp->wb.wb3100.R_G_H = iRG_Value / 256;
	otp->wb.wb3100.B_G_L =  iBG_Value % 256;
	otp->wb.wb3100.B_G_H =  iBG_Value / 256;
    	otp->wb.wb3100.flag_wb = 0x01;	
	sum = CheckSum(&otp->wb.wb3100.R_avgL , 16);
	put_be_val(sum % 0xFF , &otp->wb.wb3100.sum_wb, sizeof(otp->wb.wb3100.sum_wb));
	otp->wb.wb3100.flag_LightSource = 1;	
	sum = CheckSum(&otp->wb.wb3100.R_G_L , 4);
	put_be_val(sum % 0xFF , &otp->wb.wb3100.sum_LightSource, sizeof(otp->wb.wb3100.sum_LightSource));
	
	//AF
	for(int i = 0 ; i<6 ; i++)
		otp->af.Reserved[i] = 0;
	otp->af.flag = 0;
	otp->af.sum = 0;

	// 2D Barcode
	CString sensorid = uts.info.strSN;//uts.info.strSensorId;
	uts.log.Error(_T("Get SensorID = %s") , sensorid);
	sensorid = sensorid.Left(16);
	memcpy(&otp->sninfo.sn, T2A(sensorid), 16);
	otp->sninfo.flag = 0x01;
	sum = CheckSum(&otp->sninfo.sn[0] , 16);
	put_be_val(sum % 0xFF , &otp->sninfo.sum, sizeof(otp->sninfo.sum));
	
	return ret;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF86A::get_uniform_otp_data(void *in, void *out, int maxlen)
{
   /* OTPData *otp = (OTPData*)in;
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
int OtpCali_S5K4H7_MF86A::get_otp_group(void)
{
    return 0;
}
//-------------------------------------------------------------------------------------------------


}
