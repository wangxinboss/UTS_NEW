#include "StdAfx.h"
#include "OtpCali_Hi546_MA741.h"

#define OFFSETMACRO  0
#define OFFSETINF  0

namespace OtpCali_Hi546_MA741 {
//-------------------------------------------------------------------------------------------------
OtpCali_Hi546_MA741::OtpCali_Hi546_MA741(BaseDevice *dev) : HynixOtp(dev)
{
	otp_type = OTP_TYPE_OTP;
    otp_data_len = sizeof(OTPData);
    otp_lsc_len = 0;//MTK_LSC_LEN;
    otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi546_MA741::do_prog_otp()
{
	
	//MINFO
 	if(!sensor->do_prog_otp(0,0x401,otp_data_in_db,otp_data_len))
 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	
	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi546_MA741::get_otp_data_from_sensor(void *args)
{
    uint8_t *otp = (uint8_t*)args;

	if(!sensor->do_read_otp(0x0,0x401,otp,otp_data_len))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi546_MA741::LscCali(void *args)
{
	return SET_ERROR(OTPCALI_ERROR_OPTNOTSUPPORT);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi546_MA741::get_minfo_from_db(void *args)
{

    MINFO *m = (MINFO *)args;

	m->flag = 0x01;	//0x01:Group1  0x13:Group2  0x37:Group3
	m->minfo1.mid = 0x08;	//0x08:Holitech
	m->minfo1.af_flag=0x01;
	
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

	m->minfo1.sensor_id=0x01;
	m->minfo1.lens_id=0x01;
	m->minfo1.vcm_id=0x01;
	m->minfo1.driveric_id=0x01;
	m->minfo1.f_number_id=0x01;

	m->minfo1.sum = CheckSum(&m->minfo1.mid, sizeof(m->minfo1)-1) % 0xFF+1;
	
    return sizeof(MINFO);
}
int OtpCali_Hi546_MA741::get_otp_data_from_db(void *args)
{
	OTPData *otp = (OTPData*)args;
	USES_CONVERSION;

	memset(otp,0x00,sizeof(OTPData));

	//module info
	int ret = get_minfo_from_db(&otp->minfo);
	//WB
	WB_DATA_UCHAR wb[2];
	ret = get_wb_from_raw_data(&wb[0], &wb[1]);
	if (ret < 0) { return ret;}


	//Golden
//	ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
//		3, (char *)& wb[1], 4);
//	if (ret < 0) return ret;

	WB_RATIO ratio, ratio_g;
	get_wb_ratio(&wb[0], &ratio, 512);
//	get_wb_ratio(&wb[1], &ratio_g, 512);
	ratio_g.r_gr=277;
	ratio_g.b_gb=320;
	ratio_g.gr_gb=512;

	uts.log.Info(_T("r_gr=0x%04x,b_gb=0x%04x,gb_gr=0x%04x"),ratio.r_gr,ratio.b_gb,ratio.gb_gr);
	uts.log.Info(_T("g_r_gr=0x%04x,g_b_gb=0x%04x,g_gb_gr=0x%04x"),ratio_g.r_gr,ratio_g.b_gb,ratio_g.gb_gr);

	otp->wb.flag=0x01;

	put_be_val(ratio.r_gr,  otp->wb.wb1.rg, sizeof(otp->wb.wb1.rg));
	put_be_val(ratio.b_gb,  otp->wb.wb1.bg, sizeof(otp->wb.wb1.bg));
	put_be_val(ratio.gb_gr, otp->wb.wb1.gg, sizeof(otp->wb.wb1.gg));

	put_be_val(ratio_g.r_gr,  otp->wb.wb1.g_rg, sizeof(otp->wb.wb1.g_rg));
	put_be_val(ratio_g.b_gb,  otp->wb.wb1.g_bg, sizeof(otp->wb.wb1.g_bg));
	put_be_val(ratio_g.gb_gr, otp->wb.wb1.g_gg, sizeof(otp->wb.wb1.g_gg));

	otp->wb.wb1.sum = CheckSum(otp->wb.wb1.rg, sizeof(otp->wb.wb1)-1) % 0xFF+1;


	//AF

	otp->af.flag=0x01;
	otp->af.af1.direction = 0x01;	//0x01:Up, 0x02:Horizon, 0x03:Down

	int inf = 0, marcro = 0;
	ret = get_af_from_raw_data(NULL, &inf, &marcro);
	if (ret < 0) return ret;

	if(marcro < 0||inf < 0) 
	{
		ret = OTPCALI_ERROR_NODATA;
		return ret;
	}

	put_be_val(inf, otp->af.af1.inf, sizeof(otp->af.af1.inf));
	put_be_val(marcro, otp->af.af1.mup, sizeof(otp->af.af1.mup));
	
	otp->af.af1.sum = CheckSum(&otp->af.af1.direction, sizeof(otp->af.af1)-1) % 0xFF+1;

    return 0;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi546_MA741::get_uniform_otp_data(void *in, void *out, int maxlen)
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
int OtpCali_Hi546_MA741::get_otp_group(void)
{
    return 0;
}
//-------------------------------------------------------------------------------------------------


}
