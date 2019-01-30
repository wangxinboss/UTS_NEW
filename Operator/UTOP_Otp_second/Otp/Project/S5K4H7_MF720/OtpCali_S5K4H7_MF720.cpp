#include "StdAfx.h"
#include "OtpCali_S5K4H7_MF720.h"
#include "Otp/Ref/LSC_Lib/Samsung/S5K4H7YX/LSC.h"

#pragma comment(lib, "Otp/Ref/LSC_Lib/Samsung/S5K4H7YX/GLSCv10_LIB_v140.lib")


#define OFFSETMACRO  56
#define OFFSETINF  124

namespace OtpCali_S5K4H7_MF720 {
//-------------------------------------------------------------------------------------------------
OtpCali_S5K4H7_MF720::OtpCali_S5K4H7_MF720(BaseDevice *dev) : SamsungOtp(dev)
{
	otp_type = OTP_TYPE_OTP;
    otp_data_len = sizeof(OTPData);
    otp_lsc_len = 360;//MTK_LSC_LEN;
    otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF720::do_prog_otp()
{

	//lsc
 	if(!sensor->do_prog_otp(0x0,0x0A3D,otp_data_in_db,1))
 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x01,0x0A04,otp_data_in_db+1+64*0,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x02,0x0A04,otp_data_in_db+1+64*1,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x03,0x0A04,otp_data_in_db+1+64*2,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x04,0x0A04,otp_data_in_db+1+64*3,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x05,0x0A04,otp_data_in_db+1+64*4,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x06,0x0A04,otp_data_in_db+1+64*5,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x07,0x0A04,otp_data_in_db+1+64*6,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x08,0x0A04,otp_data_in_db+1+64*7,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x09,0x0A04,otp_data_in_db+1+64*8,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x0A,0x0A04,otp_data_in_db+1+64*9,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_prog_otp(0x0B,0x0A04,otp_data_in_db+1+64*10,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);

	//MINFO+AF
	if(!sensor->do_prog_otp(21,0x0A04,otp_data_in_db+721,36))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	
	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF720::get_otp_data_from_sensor(void *args)
{
    OTPData *otp = (OTPData*)args;
	BOOL b2ndSet = FALSE;
//	char lscbuf[360];

	if(!sensor->do_read_otp(0x0,0x0A3D,&otp->lsc.flag,1))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x01,0x0A04,otp->lsc.lsc1.lsc+64*0,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x02,0x0A04,otp->lsc.lsc1.lsc+64*1,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x03,0x0A04,otp->lsc.lsc1.lsc+64*2,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x04,0x0A04,otp->lsc.lsc1.lsc+64*3,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x05,0x0A04,otp->lsc.lsc1.lsc+64*4,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x06,0x0A04,otp->lsc.lsc1.lsc+64*5,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x07,0x0A04,otp->lsc.lsc1.lsc+64*6,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x08,0x0A04,otp->lsc.lsc1.lsc+64*7,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x09,0x0A04,otp->lsc.lsc1.lsc+64*8,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x0A,0x0A04,otp->lsc.lsc1.lsc+64*9,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x0B,0x0A04,otp->lsc.lsc1.lsc+64*10,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(!sensor->do_read_otp(0x0C,0x0A04,otp->lsc.lsc1.lsc+64*11,16))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	
	//MINFO
	if(!sensor->do_read_otp(21,0x0A04,&otp->minfo,22))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	//AF
	if(!sensor->do_read_otp(21,0x0A19,&otp->af,14))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF720::LscCali(void *args)
{
	OTPData *otp = (OTPData*)args;

	int ret;	

	const char *pResult,*pText;
	
	CString tempstr,temp_line;

	USES_CONVERSION;
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	using namespace UTS::Algorithm::Image;

	FILE* fp_bayer_setting = fopen("bayer_setting.xml","rt");
	FILE* fp_binary_dat    = fopen("binary.dat","rt");
		
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
	//WORD *RAW10Image_output= new WORD[3264* 2448];
	WORD *RAW10Image_output= new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	if (!uts.imgFile.SaveRawFile(_T("LSC_Cali_V10_input"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight)){
		uts.log.Error(_T("LSC: failed to save raw_input file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	if(fp_bayer_setting==NULL){
		uts.log.Error(_T("bayer_setting.xml Open Error."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	if(fp_binary_dat==NULL){
		uts.log.Error(_T("binary.dat Open Error."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	fclose(fp_bayer_setting);
	fclose(fp_binary_dat);

	pResult = Initialize(NULL, "bayer_setting.xml", 0, "binary.dat");

	if (pResult == 0)
	{
		pResult = Run(RAW10Image_input);
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

	return SET_ERROR(ret);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF720::get_minfo_from_db(void *args)
{

	MINFO *m = (MINFO *)args;

	m->flag = 0x40;	//0x40:Group1 valid, 0xD0:Group2 valid
	m->minfo1.mid = 0x08;	//0x08:Holitech
	m->minfo1.lens_id=0x01;

	SYSTEMTIME stime;
	time_t time;
	if (otpDB->GetOtpBurnTime(mid, &time) < 0)
	{
		GetLocalTime(&stime);
		m->minfo1.year  = stime.wYear % 100;
		m->minfo1.month = (uint8_t)stime.wMonth;
		m->minfo1.day   = (uint8_t)stime.wDay;
	}else
	{
		struct tm today;
		_localtime64_s( &today, &time );

		m->minfo1.year = today.tm_year % 100;
		m->minfo1.month = (uint8_t)today.tm_mon + 1;
		m->minfo1.day = (uint8_t)today.tm_mday;
	}

/*	return sizeof(MINFO);
		
	USES_CONVERSION;
		
	char sum = CheckSum(&m->minfo1.mid, 9);
    put_be_val(sum % 0xFF + 1, &m->minfo1.sum, sizeof(m->minfo1.sum));*/

    return sizeof(MINFO);
}
int OtpCali_S5K4H7_MF720::get_otp_data_from_db(void *args)
{
	OTPData *otp = (OTPData*)args;
	USES_CONVERSION;

	//LSC
	otp->lsc.flag=0x01;
	int ret = get_lsc_from_raw_data(otp->lsc.lsc1.lsc, sizeof(otp->lsc.lsc1.lsc));
	if (ret < 0) return ret;

	int sum = 0;
	for(int i=0;i<360;i++)
	{
		sum+=otp->lsc.lsc1.lsc[i];
	}
	otp->minfo.minfo1.lscsum=(sum % 0xFF + 1);

	uts.log.Info(_T("otp->lsc.lsc1.sum=0x%02x"),otp->minfo.minfo1.lscsum);

	//module info
	ret = get_minfo_from_db(&otp->minfo);
	//WB
	WB_DATA_UCHAR wb[2];
	ret = get_wb_from_raw_data(&wb[0], &wb[1]);
	if (ret < 0) { return ret;}

	uts.log.Info(_T("wb get pass,start get golden"));
	//Golden
//	ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
//		3, (char *)& wb[1], 4);
//	if (ret < 0) return ret;

	uts.log.Info(_T("wb golden get pass"));

	WB_RATIO ratio, ratio_g;
	get_wb_ratio(&wb[0], &ratio, 512);
	get_wb_ratio(&wb[1], &ratio_g, 512);

	uts.log.Info(_T("r_g=0x%02x,b_g=0x%02x"),ratio.r_g,ratio.b_g);

	otp->minfo.minfo1.wb.r_g_msb=(BYTE)(ratio.r_g>>2);
	otp->minfo.minfo1.wb.b_g_msb=(BYTE)(ratio.b_g>>2);
	otp->minfo.minfo1.wb.r_g_b_g_lsb=(BYTE)(((ratio.r_g&0x03)<<6)+((ratio.b_g&0x03)<<4));

	sum = CheckSum(&otp->minfo.minfo1, 9);
	put_be_val(sum % 0xFF + 1, &otp->minfo.minfo1.infosum, sizeof(otp->minfo.minfo1.infosum));

	//AF
	memset(otp->af.Reserve,0,14);

    return 0;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H7_MF720::get_uniform_otp_data(void *in, void *out, int maxlen)
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
int OtpCali_S5K4H7_MF720::get_otp_group(void)
{
    return 0;
}
//-------------------------------------------------------------------------------------------------


}
