#include "StdAfx.h"
#include "OtpCali_S5K4H8_MA703.h"

#define OFFSETMACRO  56
#define OFFSETINF  124

namespace OtpCali_S5K4H8_MA703 {
//-------------------------------------------------------------------------------------------------
OtpCali_S5K4H8_MA703::OtpCali_S5K4H8_MA703(BaseDevice *dev) : SamsungOtp(dev)
{
	otp_type = OTP_TYPE_OTP;
    otp_data_len = sizeof(OTPData);
    otp_lsc_len = 360;//MTK_LSC_LEN;
    otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA703::do_prog_otp()
{
	{
	//MINFO
 	if(sensor->do_prog_otp(0xF,0x0A04,otp_data_in_db,64))
 		return SET_ERROR(OTPCALI_ERROR_SENSOR);

// 	//page9 0x0A1C~0x0A43 LenC Setting 2
// 	if(sensor->do_prog_otp(0x9,0x0A33,lscbuf,40))
// 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
// 	//page10 0x0A04~0x0A43 LenC Setting 2
// 	if(sensor->do_prog_otp(0xA,0x0A04,lscbuf+40+64,64))
// 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
// 	//page11 0x0A04~0x0A43 LenC Setting 2
// 	if(sensor->do_prog_otp(0xB,0x0A04,lscbuf+40+64*2,64))
// 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
// 	//page12 0x0A04~0x0A43 LenC Setting 2
// 	if(sensor->do_prog_otp(0xC,0x0A04,lscbuf+40+64*3,64))
// 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
// 	//page13 0x0A04~0x0A43 LenC Setting 2
// 	if(sensor->do_prog_otp(0xD,0x0A04,lscbuf+40+64*4,64))
// 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
// 	//page14 0x0A04~0x0A43 LenC Setting 1
// 	if(sensor->do_prog_otp(0xE,0x0A04,lscbuf+40+64*5,64))
// 		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	}
	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA703::get_otp_data_from_sensor(void *args)
{
    OTPData *otp = (OTPData*)args;
	BOOL b2ndSet = TRUE;
	char lscbuf[360];
	//MINFO
	if(sensor->do_read_otp(0xF,0x0A04,otp,64))
		return SET_ERROR(OTPCALI_ERROR_SENSOR);
	if(b2ndSet)
	{
		//LSC 2
		if(sensor->do_read_otp(0x9,0x0A1C,lscbuf,40))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0xA,0x0A04,lscbuf+40,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0xB,0x0A04,lscbuf+40+64*1,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0xC,0x0A04,lscbuf+40+64*2,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0xD,0x0A04,lscbuf+40+64*3,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0xE,0x0A04,lscbuf+40+64*4,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		memcpy(otp->lsc.lsc,lscbuf,360);

	}
	else
	{
		//LSC 1
		if(sensor->do_read_otp(0x3,0x0A33,lscbuf,17))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0x4,0x0A04,lscbuf+17,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0x5,0x0A04,lscbuf+17+64*1,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0x6,0x0A04,lscbuf+17+64*2,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0x7,0x0A04,lscbuf+17+64*3,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0x8,0x0A04,lscbuf+17+64*4,64))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		if(sensor->do_read_otp(0x9,0x0A04,lscbuf+17+64*5,23))
			return SET_ERROR(OTPCALI_ERROR_SENSOR);
		memcpy(otp->lsc.lsc,lscbuf,360);

	}

	//if (eeprom->Read(0, otp, otp_data_len) < 0)
	//	return SET_ERROR(OTPCALI_ERROR_SENSOR);

	return SET_ERROR(OTPCALI_ERROR_NO);
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA703::LscCali(void *args)
{
	return do_lsc_cali();
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA703::get_minfo_from_db(void *args)
{

    MINFO *m = (MINFO *)args;
	
	m->flag = 0x40;	//0x40:Group1 valid, 0xD0:Group2 valid
	m->minfo1.mid = 0x1C;	//0x1c:Holitech
	
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

    m->minfo1.lens_id = 0x2C;	//XA-0802B:0x2C
    m->minfo1.vcm_id = 0x27;	//ZET-V84B8601:0x27
    m->minfo1.vcmdriver_id = 0x01;	//DW9718S:0x27
	m->minfo1.ir=0x00;			//IR:0x00
	m->minfo1.etc=0x14;			//5100K:[7:4]=0b0001 AF:[3:2]=0b01
		
	USES_CONVERSION;
		
	char sum = CheckSum(&m->minfo1.mid, 9);
    put_be_val(sum % 0xFF + 1, &m->minfo1.sum, sizeof(m->minfo1.sum));

    return sizeof(MINFO);
}
int OtpCali_S5K4H8_MA703::get_otp_data_from_db(void *args)
{
	OTPData *otp = (OTPData*)args;
	USES_CONVERSION;

	//module info
	int ret = get_minfo_from_db(&otp->minfo);

	//AF
	otp->af.flag = 0x40;
	otp->af.af1.vcmdir = 0x01;	//0x01:Up, 0x02:Horizon, 0x03:Down

	int inf = 0, marcro = 0;
	ret = get_af_from_raw_data(NULL, &inf, &marcro);
	if (ret < 0) return ret;

	//marcro = marcro - OFFSETINF;
	if(marcro < 0) 
	{
		ret = OTPCALI_ERROR_NODATA;
		return ret;
	}

	//inf = inf - OFFSETINF;
	if(inf < 0)
	{
		ret = OTPCALI_ERROR_NODATA;
		return ret;
	}

	put_be_val(marcro, otp->af.af1.mup, sizeof(otp->af.af1.mup));
	put_be_val(inf, otp->af.af1.inf, sizeof(otp->af.af1.inf));

	char sum = CheckSum(&otp->af.af1.vcmdir, 5);
	put_be_val(sum % 0xFF + 1, &otp->af.af1.sum, sizeof(otp->af.af1.sum));

	//WB
	otp->wb.flag = 0x40;
	WB_DATA_UCHAR wb[2];
	ret = get_wb_from_raw_data(&wb[0], &wb[1]);
	if (ret < 0) { return ret;}

	//Golden
	ret = otpDB->GetOtpByType(uts.otpdb->GetModuleID(T2A(otp_param.wb_param.goldenSampleName)), 
		3, (char *)& wb[1], 4);
	if (ret < 0) return ret;

	WB_RATIO ratio, ratio_g;
	get_wb_ratio(&wb[0], &ratio, 16384.0);
	get_wb_ratio(&wb[1], &ratio_g, 16384.0);

	put_be_val(ratio.r_g, otp->wb.wb1.r_g, 2);
	put_be_val(ratio.b_g, otp->wb.wb1.b_g, 2);
	put_be_val(ratio.gr_gb, otp->wb.wb1.gr_gb, 2);
	put_be_val(ratio_g.r_g, otp->wb.wb1.r_g_g, 2);
	put_be_val(ratio_g.b_g, otp->wb.wb1.b_g_g, 2);
	put_be_val(ratio_g.gr_gb, otp->wb.wb1.gr_gb_g, 2);

	//Check sum
	sum = CheckSum(otp->wb.wb1.r_g, 12);
	put_be_val(sum % 0xFF + 1, &otp->wb.wb1.sum, sizeof(otp->wb.wb1.sum));

	//LSC
	ret = get_lsc_from_raw_data(otp->lsc.lsc, sizeof(otp->lsc.lsc));
	if (ret < 0) return ret;
	
    return 0;
}
//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA703::get_uniform_otp_data(void *in, void *out, int maxlen)
{
    OTPData *otp = (OTPData*)in;
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
    return sizeof(OtpDataHeader) + hdr->len;
}

//-------------------------------------------------------------------------------------------------
int OtpCali_S5K4H8_MA703::get_otp_group(void)
{
    return 0;
}
//-------------------------------------------------------------------------------------------------


}
