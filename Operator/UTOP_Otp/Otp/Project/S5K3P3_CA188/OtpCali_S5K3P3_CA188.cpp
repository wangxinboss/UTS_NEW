#include "StdAfx.h"
#include "OtpCali_S5K3P3_CA188.h"
#include "./OISDriver/Onsemi_LC898123/OnSemi_LC898123.h"
#include "../../3rdparty/PDAF/QualComm/PDAF_QulComm_RevJ.h"

//-----------------------------------------------------------------------------


UINT32 FromFloatTo2sComplement(float a_fData)
{
	UINT64 u64Result = 0x7FFFFFFF;
	double dTemp =0; 

	if(a_fData >= 0)
		dTemp = 0x7FFFFFFF * a_fData;
	else
		dTemp = ((DWORD)(0x7FFFFFFF * (-a_fData)) ^ 0xFFFFFFFF) +1;

	u64Result = (UINT32) dTemp & 0xFFFFFFFF;

	return (UINT32)u64Result;
}

//-----------------------------------------------------------------------------

namespace OtpCali_S5K3P3_CA188 {
	//-------------------------------------------------------------------------------------------------
	OtpCali_S5K3P3_CA188::OtpCali_S5K3P3_CA188(BaseDevice *dev) : SamsungOtp(dev)
	{
		otp_type = OTP_TYPE_EEPROM;
		otp_data_len = sizeof(OTPData);
		otp_lsc_len = QULCOMM_LSC_LEN;
		otp_operator_attr |= OTP_OPERATOR_ATTR_USECACHE;
	
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P3_CA188::do_prog_otp()
	{
		otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_MODULE,
			(char *)otp_data_in_db, sizeof(MINFO));

		if (eeprom->Write(0, otp_data_in_db, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_EEPROM);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P3_CA188::get_otp_data_from_sensor(void *args)
	{
		OTPData *otp = (OTPData*)args;

		if (eeprom->Read(0, otp, otp_data_len) < 0)
			return SET_ERROR(OTPCALI_ERROR_SENSOR);

		return SET_ERROR(OTPCALI_ERROR_NO);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P3_CA188::LscCali(void *args)
	{
		return do_qulcomm_lsc_awb_cali();
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P3_CA188::get_minfo_from_db(void *args)
	{
		MINFO *m = (MINFO *)args;

		USES_CONVERSION;

		m->lsc_ver = 0x01;
		m->pdaf_ver = 0x01;
		m->vendor_id = 0x09;
		m->lens_id = 0x01;
		m->vcm_id = 0x01;
		m->vcmdriver_id = 0x01;
		m->otpmapping_id = 0x01;
		m-> sum_info  = CheckSum(&m->lsc_ver, 7) % 0xFF + 1;
		memset(&m->sn,0,16 * sizeof(uint8_t));
		memcpy(&m->sn, T2A(m_szSN), 16);
		m->sum_sn = CheckSum(&m->sn, 16) % 0xFF + 1;

		return sizeof(MINFO);
	}
	int OtpCali_S5K3P3_CA188::get_otp_data_from_db(void *args)
	{
		OTPData *otp = (OTPData*)args;

	//	memset(&otp,0,sizeof(OTPData));

		//module info
		int ret = get_minfo_from_db(&otp->minfo);

		//WB
		struct WB_DATA_SHORT wbtemp[2];

		ret = get_otp_from_raw_data(OTPDB_OTPTYPE_AWB, (unsigned short*)wbtemp, sizeof(wbtemp));
		if (ret < 0) return ret;

		put_le_val(wbtemp[0].RG, otp->wb.rg, sizeof(otp->wb.rg));
		put_le_val(wbtemp[0].BG, otp->wb.bg, sizeof(otp->wb.bg));
		put_le_val(wbtemp[0].GbGr, otp->wb.gbgr, sizeof(otp->wb.gbgr));

		//AF
		int start = 0, inf = 0, marcro = 0;
		ret = get_af_from_raw_data(&start, &inf, &marcro);
		if (ret < 0) return ret;
		
		put_le_val(start, otp->af.start, sizeof(otp->af.start));
		put_le_val(marcro, otp->af.mup, sizeof(otp->af.mup));
		put_le_val(inf, otp->af.inf, sizeof(otp->af.inf));

		// For LSC
		otp->lsc.lscwidth = 17;
		otp->lsc.lscheight = 13;

		ret = get_lsc_from_raw_data(otp->lsc.lsc, sizeof(otp->lsc.lsc));
		if (ret < 0) return ret;

		// For PDAF - SPC
		GainMap2DDataStruct GainMapData2D;
		uint8_t SPCTable[0x1200];

		ret = otpDB->GetOtpByType(mid, OTPDB_OTPTYPE_SPC, (char *)SPCTable,	sizeof(GainMap2DDataStruct));
		if (ret < 0) return ret;

		memcpy(&GainMapData2D,SPCTable,sizeof(GainMap2DDataStruct));
		
		put_le_val(GainMapData2D.OffsetX, otp->pdaf.offsetX, sizeof(otp->pdaf.offsetX));
		put_le_val(GainMapData2D.OffsetY, otp->pdaf.offsetY, sizeof(otp->pdaf.offsetY));
		put_le_val(GainMapData2D.RatioX, otp->pdaf.ratioX, sizeof(otp->pdaf.ratioX));
		put_le_val(GainMapData2D.RatioY, otp->pdaf.ratioY, sizeof(otp->pdaf.ratioY));
		put_le_val(GainMapData2D.MapHeight, otp->pdaf.mapHeight, sizeof(otp->pdaf.mapHeight));
		put_le_val(GainMapData2D.MapWidth, otp->pdaf.mapWidth, sizeof(otp->pdaf.mapWidth));
		memcpy(otp->pdaf.leftgain,GainMapData2D.Left_GainMap, sizeof(otp->pdaf.leftgain));
		memcpy(otp->pdaf.rightgain,GainMapData2D.Right_GainMap, sizeof(otp->pdaf.rightgain));
		
		ret = otpDB->GetOtpByType(mid, 10, (char *)otp->pdaf.pd,	sizeof(otp->pdaf.pd));
		if (ret < 0) return ret;

		//checksum
		int checksum = CheckSum(&otp->af.start, 5894);

		put_le_val(checksum % 0xFFFF + 1, otp->checksum, sizeof(otp->checksum));
		if (ret < 0) return ret;

		return sizeof(OTPData);
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P3_CA188::get_uniform_otp_data(void *in, void *out, int maxlen)
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
					OtpDataMInfo *minfo = (OtpDataMInfo *)item->data;
					minfo->tm = NULL;
					minfo->raw_len = sizeof(otp->minfo);
					memcpy(minfo->raw, &otp->minfo, minfo->raw_len);
				}
				break;
			case OtpDataType_WB:
				{
					OtpDataWB *wb = (OtpDataWB *)item->data;
					wb->raw_len = sizeof(otp->wb);
					memcpy(wb->raw, &otp->wb, wb->raw_len);
					item->len = sizeof(OtpDataWB) + wb->raw_len;
				}
				break;
			case OtpDataType_AF:
				{
					OtpDataAF *af = (OtpDataAF*)item->data;
					af->af.start = get_be_val(otp->af.start, sizeof(otp->af.start));
					af->af.inf = get_be_val(otp->af.inf, sizeof(otp->af.inf));
					af->af.mup = get_be_val(otp->af.mup, sizeof(otp->af.mup));
					af->raw_len = sizeof(otp->af);
					memcpy(af->raw, &otp->af, af->raw_len);
					item->len = sizeof(OtpDataAF) + af->raw_len;
				}
				break;
			case OtpDataType_LSC:
				{
					OtpDataLSC *lsc = (OtpDataLSC*)item->data;
					lsc->lsc = lsc->raw + offsetof(LSC, lsc);
					lsc->raw_len = sizeof(otp->lsc);
					memcpy(lsc->raw, &otp->lsc, lsc->raw_len);
					item->len = sizeof(OtpDataLSC) + lsc->raw_len;
				}
				break;
			case OtpDataType_PDAF:
				{
					memcpy(&item->data[item->len], otp->pdaf.offsetX, sizeof(otp->pdaf.offsetX));
					item->len += sizeof(otp->pdaf.offsetX);
					memcpy(&item->data[item->len], otp->pdaf.offsetY, sizeof(otp->pdaf.offsetY));
					item->len += sizeof(otp->pdaf.offsetY);
					memcpy(&item->data[item->len], otp->pdaf.ratioX, sizeof(otp->pdaf.ratioX));
					item->len += sizeof(otp->pdaf.ratioX);
					memcpy(&item->data[item->len], otp->pdaf.ratioY, sizeof(otp->pdaf.ratioY));
					item->len += sizeof(otp->pdaf.ratioY);
					memcpy(&item->data[item->len], otp->pdaf.mapWidth, sizeof(otp->pdaf.mapWidth));
					item->len += sizeof(otp->pdaf.mapWidth);
					memcpy(&item->data[item->len], otp->pdaf.mapHeight, sizeof(otp->pdaf.mapHeight));
					item->len += sizeof(otp->pdaf.mapHeight);
					memcpy(&item->data[item->len], otp->pdaf.leftgain, sizeof(otp->pdaf.leftgain));
					item->len += sizeof(otp->pdaf.leftgain);
					memcpy(&item->data[item->len], otp->pdaf.rightgain, sizeof(otp->pdaf.rightgain));
					item->len += sizeof(otp->pdaf.rightgain);
					memcpy(&item->data[item->len], otp->pdaf.pd, sizeof(otp->pdaf.pd));
					item->len += sizeof(otp->pdaf.pd);
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
	int OtpCali_S5K3P3_CA188::get_otp_group(void)
	{
		return 0;
	}
	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P3_CA188::do_hallCali()
	{
		int error = OTPCALI_ERROR_NO;
		
		unHall_errorCode = ois->HallCal(HallCaliData);

		uts.log.Debug(_T("Hall Result: 0x%x"),unHall_errorCode);
		if(unHall_errorCode != 2) error = OTPCALI_ERROR_HALLCALI;

		return error;
	}

	//-------------------------------------------------------------------------------------------------
	int OtpCali_S5K3P3_CA188::do_SetgyroX(double gain)
	{
		float x = (float)gain;
		UINT Fix = FromFloatTo2sComplement(x);

		return ois->SetGyroGainX(Fix);
	}

	int OtpCali_S5K3P3_CA188::do_SetgyroY(double gain)
	{
		float x = (float)gain;
		UINT Fix = FromFloatTo2sComplement(x);

		return ois->SetGyroGainY(Fix);
	}

	int OtpCali_S5K3P3_CA188::do_OISON()
	{
		return ois->OISControl(true);
	}

	int OtpCali_S5K3P3_CA188::do_OISOFF()
	{
		return ois->OISControl(false);
	}

	int OtpCali_S5K3P3_CA188::do_SaveGyroGainData(double gainX,double gainY)
	{
		return ois->SaveGyroGainData();
	}

	int OtpCali_S5K3P3_CA188::do_LoadFW(CString FWFilePath)
	{
		int error = OTPCALI_ERROR_NO;

		int result = ois->LoadFW();
			
		if(result != 0x00) 
		error = OTPCALI_ERROR_LOADFW;

		return error;		
	}

	int OtpCali_S5K3P3_CA188::do_CheckFW(int FWVer)
	{
		return ois->CheckFW(FWVer);
	}

	int OtpCali_S5K3P3_CA188::do_SPCCali(uint16_t *pRaw10,int width, int height, uint8_t out[], int max_len)
	{
		PDAF_QulComm_RevJ pdaf;

		return pdaf.SPCCali(pRaw10,width,height,out,max_len);
	}

	int OtpCali_S5K3P3_CA188::DCC0Cali(void *args)
	{
		return do_qulcomm_dcc_cali(0);
	}
	int OtpCali_S5K3P3_CA188::DCC1Cali(void *args)
	{
		return do_qulcomm_dcc_cali(1);
	}

	int OtpCali_S5K3P3_CA188::PDAFVerify(void *args)
	{
		return qulcomm_dcc_verify();
	}

	int OtpCali_S5K3P3_CA188::do_qualcomm_LSCAWBCali(int v5u_bayer,unsigned char *pRaw10, int w, int h, uint8_t out[],uint8_t awbout[])
	{
		PDAF_QulComm_RevJ pdaf;
		return pdaf.LSCAWBCali(v5u_bayer,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,out,awbout);
	}

	int OtpCali_S5K3P3_CA188::do_qualcomm_DCCCali(uint8_t *SPCTable,uint16_t *raw_negative, uint16_t *raw_positive, int Width, int Height, int dac_negative, int dac_positive)
	{
		PDAF_QulComm_RevJ pdaf;
		return pdaf.DCCCali(SPCTable,raw_negative, raw_positive,
							Width, Height, 
							dac_negative, dac_positive);
	}

	bool OtpCali_S5K3P3_CA188::do_qulcomm_DCCVerify(uint8_t *SPCTable,uint16_t *img, int w, int h,int PD_ConversionCoeff, int ToleranceError, int VerifyDAC,int solbelfocusdac, int &PDAFDAC, double &VerifyError)
	{
		PDAF_QulComm_RevJ pdaf;

		return pdaf.DCCVerify(SPCTable,img,
			w, h,
			PD_ConversionCoeff,
			ToleranceError,
			VerifyDAC,
			solbelfocusdac,
			PDAFDAC,
			VerifyError);
	}
	//Ryan@20160506
	int OtpCali_S5K3P3_CA188::do_SetSltPos(unsigned int UcPos)
	{
		return ois->SetSltPos(UcPos);
	}

	int OtpCali_S5K3P3_CA188::do_SetVrtPos(unsigned int UcPos)
	{
		return ois->SetVrtPos(UcPos);
	}

	int OtpCali_S5K3P3_CA188::do_SetHrzPos(unsigned int UcPos)
	{
		return ois->SetHrzPos(UcPos);
	}

	int OtpCali_S5K3P3_CA188::do_GetGyroGainX(UINT32 *Gain)
	{
		return ois->GetGyroGainX(Gain);
	}

	int OtpCali_S5K3P3_CA188::do_GetGyroGainY(UINT32 *Gain)
	{
		return ois->GetGyroGainY(Gain);
	}


}
