#include "StdAfx.h"
#include "OtpCali_Hi846_MF80U.h"

#define OFFSETMACRO  56
#define OFFSETINF  124


namespace OtpCali_Hi846_MF80U {
//-------------------------------------------------------------------------------------------------
OtpCali_Hi846_MF80U::OtpCali_Hi846_MF80U(BaseDevice *dev) : HynixOtp(dev)
{
	otp_type = OTP_TYPE_EEPROM;
    otp_data_len = sizeof(OTPData);
    otp_lsc_len = MTK_LSC_LEN;
    otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi846_MF80U::do_prog_otp()
{
	otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_MODULE,
		(char *)otp_data_in_db, sizeof(MINFO));

	if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
		return SET_ERROR(OTPCALI_ERROR_EEPROM);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi846_MF80U::get_otp_data_from_sensor(void *args)
{
    OTPData *otp = (OTPData*)args;

	if (eeprom->Read(0, otp, otp_data_len) < 0)
		return SET_ERROR(OTPCALI_ERROR_SENSOR);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi846_MF80U::LscCali(void *args)
{
    return do_mtk_lsc_cali();
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi846_MF80U::get_minfo_from_db(void *args)
{
    MINFO *m = (MINFO *)args;
	
    m->flag = 0x80;
	m->ver[0] = 0xff;
	m->ver[1] = 0x00;
	m->ver[2] = 0x0b;
	m->ver[3] = 0x01;

	SYSTEMTIME stime;
	time_t time;

	otpDB->get_otp_data_lock_time(mid, &time);
	if (time < 0)
	{
		GetLocalTime(&stime);
		m->y  = stime.wYear % 100;
		m->m = (uint8_t)stime.wMonth;
		m->d   = (uint8_t)stime.wDay;
	}else
	{
		struct tm today;
		_localtime64_s( &today, &time );

		m->y   = today.tm_year % 100;
		m->m  = (uint8_t)today.tm_mon + 1;
		m->d    = (uint8_t)today.tm_mday;
	}

	m->mid = 0x1c;
    m->lensid = 0x45;
    m->vcmid = 0x0c;
    m->drvericid = 0x06;
    m->eepromid = 0x01;
	m->color_temprature = 0x01;
	m->bit_enable = 0x0f;
	
	int sum = CheckSum(&m->mid, 14);
    put_be_val(sum % 0xFF + 1, &m->sum, sizeof(m->sum));

    return sizeof(MINFO);
}
int OtpCali_Hi846_MF80U::get_otp_data_from_db(void *args)
{
	OTPData *otp = (OTPData*)args;
	USES_CONVERSION;

	//module info
	int ret = get_minfo_from_db(&otp->minfo);
    
	//WB
	otp->wb.flag = 0x80;
	WB_DATA_UCHAR wb[2];
	ret = otpDB->GetOtpByType(mid, 3, (char *)& wb[0], 4);
	if (ret < 0) return ret;

	//Golden
	//ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
	//	3, (char *)& wb[1], 4);
	//if (ret < 0) return ret;

	wb[1].R = 85;
	wb[1].Gr = 169;
	wb[1].Gb = 170;
	wb[1].B = 108;

	WB_RATIO ratio, ratio_g;
	get_wb_ratio(&wb[0], &ratio, 1024);

	ratio_g.r_g = 461;
	ratio_g.b_g = 615;
	ratio_g.gr_gb = 1028;

	uts.log.Info(_T("rg   =0x%04x, bg   =0x%04x, grgb   =0x%04x"),ratio.r_g,ratio.b_g,ratio.gr_gb);
	uts.log.Info(_T("rg_g =0x%04x, bg_g =0x%04x, grgb_g =0x%04x"),ratio_g.r_g,ratio_g.b_g,ratio_g.gr_gb);

	put_be_val(ratio.r_g, otp->wb.rg, 2);
	put_be_val(ratio.b_g, otp->wb.bg, 2);
	put_be_val(ratio.gr_gb, otp->wb.grgb, 2);

	put_be_val(ratio_g.r_g, otp->wb.rg_g, 2);
	put_be_val(ratio_g.b_g, otp->wb.bg_g, 2);
	put_be_val(ratio_g.gr_gb, otp->wb.grgb_g, 2);

	otp->wb.r = wb[0].R;
	otp->wb.gr = wb[0].Gr;
	otp->wb.gb = wb[0].Gb;
	otp->wb.b = wb[0].B;

	otp->wb.r_g = wb[1].R;
	otp->wb.gr_g = wb[1].Gr;
	otp->wb.gb_g = wb[1].Gb;
	otp->wb.b_g = wb[1].B;

	//Check sum
	int sum = CheckSum(otp->wb.rg, 14);
	put_be_val(sum % 0xFF + 1, &otp->wb.sum, sizeof(otp->wb.sum));

	//LSC
	otp->lsc.flag = 0x80;
	ret = get_lsc_from_raw_data(otp->lsc.lsc, sizeof(otp->lsc.lsc));
	if (ret < 0) return ret;
	sum = CheckSum(otp->lsc.lsc, MTK_LSC_LEN);
	put_be_val(sum % 0xFF + 1, &otp->lsc.sum, sizeof(otp->lsc.sum));
	
    return 0;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_Hi846_MF80U::get_uniform_otp_data(void *in, void *out, int maxlen)
{
    OTPData *otp = (OTPData*)in;
    OtpDataHeader *hdr = (OtpDataHeader*)out;
    hdr->len = 0;

   /* for (int i = 0; i < OtpDataType_Num; i++) {
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
                wb->ratio.dr_g = otp->wb.r/((otp->wb.gr + otp->wb.gb)/2);
                wb->ratio.db_g = otp->wb.b/((otp->wb.gr + otp->wb.gb)/2);
                wb->ratio_g.dr_g = otp->wb.r_g/((otp->wb.gr_g + otp->wb.gb_g)/2);
                wb->ratio_g.db_g = otp->wb.b_g/((otp->wb.gr_g + otp->wb.gb_g)/2);
                wb->raw_len = sizeof(otp->wb);
                memcpy(wb->raw, &otp->wb, wb->raw_len);
                item->len = sizeof(OtpDataWB) + wb->raw_len;
                item->sum_len = sizeof(otp->wb.sum);
                item->sum = get_be_val(otp->wb.sum, sizeof(otp->wb.sum));
                item->sum_sta = wb->raw + offsetof(WB, r_g);
                item->sum_calc_len = 12;
            }
            break;
        case OtpDataType_AF:
            {
                OtpDataAF *af = (OtpDataAF*)item->data;
                af->af.start = -1;
                af->af.inf = get_be_val(otp->af.inf, sizeof(otp->af.inf));
                af->af.mup = get_be_val(otp->af.mup, sizeof(otp->af.mup));
                af->raw_len = sizeof(otp->af);
                memcpy(af->raw, &otp->af, af->raw_len);
                item->len = sizeof(OtpDataAF) + af->raw_len;
                item->sum_len = sizeof(otp->af.sum);
                item->sum = get_be_val(otp->af.sum, sizeof(otp->af.sum));
                item->sum_sta = af->raw + offsetof(AF, lens);
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
                item->sum = get_be_val(otp->lsc.sum, sizeof(otp->lsc.sum));
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
    }*/
    return sizeof(OtpDataHeader) + hdr->len;
}

//-------------------------------------------------------------------------------------------------
int OtpCali_Hi846_MF80U::get_otp_group(void)
{
    return 0;
}
//-------------------------------------------------------------------------------------------------


}
