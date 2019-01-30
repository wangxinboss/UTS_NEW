#include "StdAfx.h"
#include "OtpCali_OV8856_MA752.h"

#define OFFSETMACRO  58
#define OFFSETINF  62

namespace OtpCali_OV8856_MA752 {
//-------------------------------------------------------------------------------------------------
OtpCali_OV8856_MA752::OtpCali_OV8856_MA752(BaseDevice *dev) : OVOtp(dev)
{
	otp_type = OTP_TYPE_OTP;
    otp_data_len = sizeof(OTPData);
    otp_lsc_len = 240;//MTK_LSC_LEN;
    otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_OV8856_MA752::do_prog_otp()
{
	
	//MINFO
 	if(!sensor->do_prog_otp(0,0x7010,otp_data_in_db,otp_data_len))
 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	
	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_OV8856_MA752::get_otp_data_from_sensor(void *args)
{
    uint8_t *otp = (uint8_t*)args;

	//MINFO
	if(!sensor->do_read_otp(0x0,0x7010,otp,otp_data_len))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_OV8856_MA752::LscCali(void *args)
{
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	OTPData *otp = (OTPData*)args;
	int ret = OTPCALI_ERROR_NO;
	struct LSC_PARAM *lsc = &otp_param.lsc_param;
	BYTE pLenCReg[240]={0};

	dev->GetBufferInfo(m_bufferInfo);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		return SET_ERROR(ret);	
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("Before LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	if (!uts.imgFile.SaveRawFile(_T("Capture_RAW8"),(BYTE*)m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("OV LSC: failed to save raw file!"));
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	ret= LenC_Cal_8858R2A_Raw8(m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,lsc->cali_target,64,pLenCReg,&otp_lsc_len);
	uts.log.Debug(_T("lsc target=%d."),lsc->cali_target);

	if(otp_lsc_len!=240)
	{
		uts.log.Error(_T("lsc size error."));
		ret = OTPCALI_ERROR_LSCCALI;
		return SET_ERROR(ret);
	}
	uts.log.Debug(_T("lsc size ok."));

	int temp;

	temp=sensor->dev->i2c_read(BIT16_BIT8,0x5000);
	sensor->dev->i2c_write(BIT16_BIT8,0x5000 ,0x20|temp);

	for(int i=0;i<240;i++)
	{
		sensor->dev->i2c_write(BIT16_BIT8,0x5900+i,pLenCReg[i]);
	}
	//	write_reg(0x5900,(char*)pLenCReg,240);
	Sleep(1500);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		return SET_ERROR(ret);		
	}

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	if (check_lsc(lsc) < 0) {
		ret = OTPCALI_ERROR_LSCCALI;
		return SET_ERROR(ret);
	}

	/*FILE * fp_lsc;
	fp_lsc=fopen("LSCOTPData.txt","wb");
	if(fp_lsc!=NULL){
	fwrite(pLenCReg,240,1024,fp_lsc);		
	}
	uts.log.Debug(_T("LSC: LSCOTPData file!"));*/

	//upload_lsc:
	if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_LSC,
		(char *)pLenCReg, 240) < 0) 
	{
			uts.log.Error(_T("Failed to update LSC Calibration data to DB!!"));
			return SET_ERROR(OTPCALI_ERROR_DB);
	}

	ret= OTPCALI_ERROR_NO;
	return SET_ERROR(ret);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_OV8856_MA752::get_minfo_from_db(void *args)
{

    MINFO *m = (MINFO *)args;
	
	m->flag = 0x40;	//0x40:Group1 valid, 0xD0:Group2 valid
	m->minfo1.mid = 0x42;	//0x08:Holitech
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
		


    return sizeof(MINFO);
}
int OtpCali_OV8856_MA752::get_otp_data_from_db(void *args)
{
	OTPData *otp = (OTPData*)args;
	WB_PARAM *pwb = &otp_param.wb_param;
	USES_CONVERSION;

	//module info
	int ret = get_minfo_from_db(&otp->minfo);
	//WB
	WB_DATA_UCHAR wb[2];
	ret = get_wb_from_raw_data(&wb[0], &wb[1]);
	if (ret < 0) { return ret;}

	WB_RATIO ratio;
	get_wb_ratio(&wb[0], &ratio, 512);


	/*get_wb_ratio(&wb[0], &ratio, 1.0);
	int awbRG = int(ratio.dr_g * pwb->rg_multi + 0.5);
	int awbBG = int(ratio.db_g * pwb->bg_multi + 0.5);*/


	uts.log.Info(_T("r_g=0x%02x,b_g=0x%02x"),ratio.r_g,ratio.b_g);

	otp->minfo.minfo1.wb.r_g_msb=(BYTE)(ratio.r_g>>2);
	otp->minfo.minfo1.wb.b_g_msb=(BYTE)(ratio.b_g>>2);
	otp->minfo.minfo1.wb.r_g_b_g_lsb=(BYTE)(((ratio.r_g&0x03)<<6)+((ratio.b_g&0x03)<<4));
	//AF
	int inf = 0, marcro = 0;
	ret = get_af_from_raw_data(NULL, &inf, &marcro);
	if (ret < 0) return ret;

	//计算姿势差--------------------
	marcro = marcro - OFFSETMACRO;
	if(marcro < 0) 
	{
		ret = OTPCALI_ERROR_NODATA;
		return SET_ERROR(ret);
	}

	inf = inf - OFFSETINF;
	if(inf < 0)
	{
		ret = OTPCALI_ERROR_NODATA;
		return SET_ERROR(ret);
	}
	//-------------------------------


	otp->af.flag = 0x40;
	otp->af.af1.inf = (BYTE)(inf>>2);
	otp->af.af1.mup = (BYTE)(marcro>>2);
	//01: Upward  10: Horizon  11: Downward 
	otp->af.af1.vcmdir = BYTE(((inf&0x03)<<6)+((marcro&0x03)<<4) + 0x08);
	
	//LSC
	otp->lsc.flag=0x40;
	ret = get_lsc_from_raw_data(otp->lsc.lsc1.lsc, sizeof(otp->lsc.lsc1.lsc));
	if (ret < 0) return ret;
	
	int sum = 0;//CheckSum(otp->lsc.lsc2.lsc, 240);
	for(int i=0;i<240;i++)
	{
		sum+=otp->lsc.lsc1.lsc[i];
	}
	otp->lsc.lsc1.sum=(sum % 0xFF + 1);
	
	uts.log.Info(_T("otp->lsc.lsc1.sum=0x%02x"),otp->lsc.lsc1.sum);
    return 0;
}

int OtpCali_OV8856_MA752::do_check_otp(void *args)
{
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	OTPData *otp = (OTPData*)args;
	struct LSC_PARAM *lsc = &otp_param.lsc_param;
	dev->GetBufferInfo(m_bufferInfo);
	//检查flag
	if (otp->minfo.flag != 0x40 || otp->af.flag != 0x40 || otp->lsc.flag != 0x40)
	{
		uts.log.Info(_T("OTP Flag Data error!!!"));
		return OTPCALI_ERROR_NODATA;
	}
	//检查Sensor ID LensID
	if (otp->minfo.minfo1.mid != 0x42 || otp->minfo.minfo1.lens_id != 0x01)
	{
		uts.log.Info(_T("SensorsID and LensID error!!!"));
		return OTPCALI_ERROR_NODATA;
	}
	//检查CheckSUM
	 uint8_t checkSum = CheckSum(otp->lsc.lsc1.lsc, otp_lsc_len) % 0xff +1;
	 if (otp->lsc.lsc1.sum != checkSum)
	 {
		 uts.log.Info(_T("LSC CheckSum  error!!!"));
		 return OTPCALI_ERROR_NODATA;
	 }
	//LSC应用

	 int temp = sensor->dev->i2c_read(BIT16_BIT8,0x5000);
	 sensor->dev->i2c_write(BIT16_BIT8,0x5000 ,0x20|temp);

	 for(int i=0;i<240;i++)
	 {
		 sensor->dev->i2c_write(BIT16_BIT8,0x5900+i,otp->lsc.lsc1.lsc[i]);
	 }
	 //--------------------------------------------------------------------------

	 Sleep(1500); 
	 
	 if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	 {
		 uts.log.Error(_T("m_pDevice->Recapture error."));
		 return OTPCALI_ERROR_SENSOR;		
	 }

	 dev->DisplayImage(m_bufferObj.pBmpBuffer);

	 RI_RGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, lsc->roi.cx, lsc->roi.cy, lsc->riResult);
	 uts.log.Debug(_T("After LSC Calibration, RI = %.2f, RI Delta = %.2f"),lsc->riResult.dRI,lsc->riResult.dRIDelta);

	 if (check_lsc(lsc) < 0) 
	 {
		 return OTPCALI_ERROR_LSCCALI;
	 }

	return OTPCALI_ERROR_NO;
}


//-------------------------------------------------------------------------------------------------
int OtpCali_OV8856_MA752::get_uniform_otp_data(void *in, void *out, int maxlen)
{
 /*   OTPData *otp = (OTPData*)in;
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
                wb->ratio.dr_g = get_be_val(otp->minfo.minfo1.wb.r_g_msb<<26+otp->minfo.minfo1.wb.r_g_b_g_lsb>>6), sizeof(otp->wb.wb1.r_g))/1023.0;
                wb->ratio.db_g = get_be_val(otp->wb.wb1.b_g, sizeof(otp->wb.wb1.b_g))/1023.0;
              //  wb->ratio_g.dr_g = get_be_val(otp->wb.wb1.r_g_g, sizeof(otp->wb.wb1.r_g_g))/1023.0;
              // wb->ratio_g.db_g = get_be_val(otp->wb.wb1.b_g_g, sizeof(otp->wb.wb1.b_g_g))/1023.0;
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
int OtpCali_OV8856_MA752::get_otp_group(void)
{
    return 0;
}
//-------------------------------------------------------------------------------------------------


}
