#include "StdAfx.h"
#include "OtpCali_GC8024_MF81B.h"


#define OFFSETMACRO  56
#define OFFSETINF  124

namespace OtpCali_GC8024_MF81B {
//-------------------------------------------------------------------------------------------------
OtpCali_GC8024_MF81B::OtpCali_GC8024_MF81B(BaseDevice *dev) : GCOtp(dev)
{
	otp_type = OTP_TYPE_OTP;
    otp_data_len =sizeof(OTPData);
    otp_lsc_len = 0;//MTK_LSC_LEN;
    otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_GC8024_MF81B::do_prog_otp()
{

	//basic flag1&awb
 	if(!sensor->do_prog_otp(0x00,0x00,otp_data_in_db,5))
 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	//basic flag2
	if(!sensor->do_prog_otp(0x01,0x36,otp_data_in_db+0x05,1))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	//module info
	if(!sensor->do_prog_otp(0x01,0x75,otp_data_in_db+0x06,10))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_GC8024_MF81B::get_otp_data_from_sensor(void *args)
{
	OTPData *otp = (OTPData*)args;

	//basic flag1&awb
	if(!sensor->do_read_otp(0x00,0x00,&otp->basic_flag1,5))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	//basic flag2
	if(!sensor->do_read_otp(0x01,0x36,&otp->basic_flag1+0x05,1))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	//module info
	if(!sensor->do_read_otp(0x01,0x75,&otp->basic_flag1+0x06,10))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);

	return SET_ERROR(OTPCALI_ERROR_NO);
}

//-------------------------------------------------------------------------------------------------
int OtpCali_GC8024_MF81B::LscCali(void *args)
{
	OTPData *otp = (OTPData*)args;

	int ret;	

	ret= OTPCALI_ERROR_NO;
	

	return SET_ERROR(ret);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_GC8024_MF81B::get_minfo_from_db(void *args)
{

	MINFO *m = (MINFO *)args;

	m->minfo1.mid = 0x42;	//
	m->minfo1.lens_id=0x43;

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
int OtpCali_GC8024_MF81B::get_otp_data_from_db(void *args)
{
	OTPData *otp = (OTPData*)args;
	USES_CONVERSION;

	int ret = 0;

	memset(otp, 0, sizeof(OTPData));

	//basic flag1
	otp->basic_flag1 = 0x41;

	//WB
	WB_DATA_UCHAR wb[2];
	ret = get_wb_from_raw_data(&wb[0], &wb[1]);
	if (ret < 0) { return ret;}

	uts.log.Info(_T("wb get pass,start get golden"));

	WB_RATIO ratio, ratio_g;
	get_wb_ratio(&wb[0], &ratio, 256);

	uts.log.Info(_T("r_g=0x%02x,b_g=0x%02x"),ratio.r_g,ratio.b_g);

	otp->wb.wb1.r_g = ratio.r_g;
	otp->wb.wb1.b_g = ratio.b_g;

	//module info
	ret = get_minfo_from_db(&otp->minfo);

	//basic flag2
	otp->basic_flag2 = 0x14;


    return ret;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_GC8024_MF81B::get_uniform_otp_data(void *in, void *out, int maxlen)
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
int OtpCali_GC8024_MF81B::get_otp_group(void)
{
    return 0;
}
//-------------------------------------------------------------------------------------------------


}
