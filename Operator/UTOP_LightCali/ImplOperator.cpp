#include "StdAfx.h"
#include "UTOP_SN.h"
#include "ImplOperator.h"
#include "UTSBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define T2A W2A
#define A2T A2W

#define IDP_SOCKETS_INIT_FAILED         103

BOOL WriteIniData(CString sSection, CString sSectionKey, CString nValue,CString sIniFileName)
{
	return WritePrivateProfileString(sSection,sSectionKey,nValue,sIniFileName);
}
//--------------------------

int ReadIniData(CString sSection,CString sSectionKey,int nDefault,CString sIniFileName)
{
	return GetPrivateProfileInt(sSection,sSectionKey,nDefault,sIniFileName);
}


namespace UTS
{
    ImplOperator::ImplOperator(void)
    {
        OPERATOR_INIT;

		sIniFileName = _T("D:\\ColorTest\\123.ini");
		
        //m_bMustStopOnFail = TRUE;   // fix bug #11: SN、LightOn失败不受StopOnFail限制。
    }

    ImplOperator::~ImplOperator(void)
    {
    }

    BOOL ImplOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("LightCaliEn"), otp_param.lightCali.LightCaliEn, 1, _T("Message shown when wait scan SN"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("BlackLevel"), BlackLel, 16, _T("Regex string for checking SN"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("IndexSpec"), otp_param.lightCali.IndexSpec, 0.1, _T("Regex string for checking SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("IndexSpec_RGain"), otp_param.lightCali.IndexSpec_RGain, 0.05, _T("Regex string for checking SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("IndexSpec_BGain"), otp_param.lightCali.IndexSpec_BGain, 0.05, _T("Regex string for checking SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("IndexSpec_GGain"), otp_param.lightCali.IndexSpec_GGain, 0.05, _T("Regex string for checking SN"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("GoldenR"), otp_param.lightCali.GoldenR, 80, _T("Regex string for checking SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("GoldenGr"), otp_param.lightCali.GoldenGr, 170, _T("Regex string for checking SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("GoldenGb"), otp_param.lightCali.GoldenGb, 170, _T("Regex string for checking SN"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("GoldenB"), otp_param.lightCali.GoldenB, 100, _T("Regex string for checking SN"));



// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strHintMessage"), m_param.strHintMessage, _T("1"), _T("Regex string for checking SN"));
// 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strRegStrForCheck"), m_param.strRegStrForCheck, _T(".*"), _T("Regex string for checking SN"));


        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//////////////////////////////////////////////////////////////////////////
		//光源点检管控
		//获取从今年1月1号开始的小时数
		if (otp_param.lightCali.LightCaliEn == 1)
		{
			int Hour;
			time_t nowtime;
			time(&nowtime);
			tm *_time;

			_time = localtime(&nowtime);
			Hour = (_time->tm_yday)*24 + _time->tm_hour;

			ReadHour = ReadIniData(_T("LightCali"),_T("Hour"),0, sIniFileName);

			if ( (Hour - ReadHour) > 24 )
			{
				uts.log.Error(_T("光源点检超时：%d > 24"), Hour - ReadHour);
				*pnErrorCode = uts.errorcode.E_Fail;
				return FALSE;
			}
		}
		else
		{
			*pnErrorCode = LightCalibra();
			if (*pnErrorCode == 0)
			{
				goto end;
			}
			else
			{
				*pnErrorCode = uts.errorcode.E_Fail;
				return FALSE;
			}
		}
		

end:
        *pnErrorCode = uts.errorcode.E_Pass;
        return TRUE;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_SNScan);
    }

	//光源点检
	int ImplOperator::LightCalibra()
	{
		USES_CONVERSION;

		int ret = uts.errorcode.E_Pass;

		//////////////////////////////////////////////////////////////////////////
		//获取从今年1月1号开始的小时数
		int Hour;
		time_t nowtime;
		time(&nowtime);
		tm *_time;

		_time=localtime(&nowtime);
		Hour = (_time->tm_yday)*24 + _time->tm_hour;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//保存blemish图片文件夹
		CString strDirPath;
		CString strFilePath;

		strDirPath.Format(_T("%sImage-%s-%s-%s-LightCali\\"),
			uts.info.strResultPath,
			uts.info.strProjectName,
			uts.info.strLineName,
			uts.info.strStationName);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		////////////////////////////////////////////////////////////////////////////////

		SIZE image_size; 
//		dev->GetBufferInfo(m_bufferInfo);
		m_pDevice->GetBufferInfo(m_bufferInfo);

		image_size.cx = m_bufferInfo.nWidth;
		image_size.cy = m_bufferInfo.nHeight;

		ROI roi;
	
		roi.width = image_size.cx / 5;
		roi.height = image_size.cy / 5;
		roi.x = (image_size.cx - roi.width) / 2;
		roi.y = (image_size.cy - roi.height) / 2;

		int nOutMode = 0;

		WB_DATA_DOUBLE* dwb = new WB_DATA_DOUBLE;
		WB_DATA_UCHAR *wb = new WB_DATA_UCHAR;

		if (!m_pDevice->ReadValue(
			eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
			&nOutMode, sizeof(nOutMode)))
		{
			uts.log.Error(_T("GetSensorOutMode error."));
			ret = uts.errorcode.E_Fail;
			return ret;
		}

		if (!m_pDevice->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("m_pDevice->Recapture error."));
			ret = uts.errorcode.E_Fail;
			return ret;
		}

		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		////////////////////////////////减BLC/////////////////////////////////
		//获取单通道值
		UTS::Algorithm::WB::WBCalibration(m_bufferObj.pRaw8Buffer, image_size.cx, image_size.cy, nOutMode,
			roi.x, roi.y, roi.width, roi.height,
			BlackLel, (double*)dwb, 
			(uint8_t*)wb);

		uts.log.Debug(_T("WB: R[%d]Gr[%d]Gb[%d]B[%d]"), wb->R,wb->Gr,wb->Gb,wb->B);

		int RGain = (int)((1.0*wb->R) / ((wb->Gb+wb->Gr)/2) * 1024);
		int BGain = (int)((1.0*wb->B) / ((wb->Gb+wb->Gr)/2) * 1024);
		int GGain = (int)((1.0*wb->Gb) / (wb->Gr) * 1024);

		int RGain_Golden = (int)((1.0*otp_param.lightCali.GoldenR) / ((otp_param.lightCali.GoldenGb+otp_param.lightCali.GoldenGr)/2)*1024);
		int BGain_Golden = (int)((1.0*otp_param.lightCali.GoldenB) / ((otp_param.lightCali.GoldenGb+otp_param.lightCali.GoldenGr)/2)*1024);
		int GGain_Golden = (int)((1.0*otp_param.lightCali.GoldenGb) / (otp_param.lightCali.GoldenGr)*1024);

		double Index_RGain = (1.0*RGain) / RGain_Golden;
		double Index_BGain = (1.0*BGain) / BGain_Golden;
		double Index_GGain = (1.0*GGain) / GGain_Golden;


		//////////////////////////////////////////////////////////////////////////
		//保存数据
		FILE  *fp  = NULL;

		long fileSizeTemp = 0;

		strFilePath.Format(_T("%s%s_LightCali.csv"),
			strDirPath,
			uts.info.strSN);

		fp = freopen(T2A(strFilePath),"at",stdout);

		if(fp == NULL)  
		{
			uts.log.Error(_T("打开光源校准文件失败"));
			ret = uts.errorcode.E_Fail;
			return ret;
		}

		fileSizeTemp = ftell(fp); 
		fseek( fp, 0, SEEK_END );
		fileSizeTemp = ftell(fp);

		if(fileSizeTemp == 0)
		{
			printf("SN,R,Gr,Gb,B,Golden_R,Golden_Gr,Golden_Gb,Golden_B,Index_R,Index_Gr,Index_Gb,Index_B,RGain,BGain,GGain,RGain_Golden,BGain_Golden,GGain_Golden,Index_RGain,Index_BGain,Index_GGain\n");
		}

		double Index_R = (1.0 * wb->R) / otp_param.lightCali.GoldenR;
		double Index_Gr = (1.0 * wb->Gr) / otp_param.lightCali.GoldenGr;
		double Index_Gb = (1.0 * wb->Gb) / otp_param.lightCali.GoldenGb;
		double Index_B = (1.0 * wb->B) / otp_param.lightCali.GoldenB;

		printf(("%s,%d,%d,%d,%d,%d,%d,%d,%d,%.4f,%.4f,%.4f,%.4f,%d,%d,%d,%d,%d,%d,%.4f,%.4f,%.4f\n"),uts.info.strSN,
			wb->R,wb->Gr,wb->Gb,wb->B,
			otp_param.lightCali.GoldenR,otp_param.lightCali.GoldenGr,otp_param.lightCali.GoldenGb,otp_param.lightCali.GoldenB,
			Index_R,Index_Gr,Index_Gb,Index_B,
			RGain,BGain,GGain,RGain_Golden,BGain_Golden,GGain_Golden,Index_RGain,Index_BGain,Index_GGain);

		fclose(fp);
		//////////////////////////////////////////////////////////////////////////


		if ( ( abs(Index_R-1) > otp_param.lightCali.IndexSpec ) || ( abs(Index_Gr-1) > otp_param.lightCali.IndexSpec ) || 
			( abs(Index_Gb-1) > otp_param.lightCali.IndexSpec ) || ( abs(Index_B-1) > otp_param.lightCali.IndexSpec ))
		{
			uts.log.Error(_T("光源单通道系数超范围，请重新校准光源"));
			ret = uts.errorcode.E_Fail;
			return ret;
		}

		if ( ( abs(Index_RGain-1) > otp_param.lightCali.IndexSpec_RGain ) || ( abs(Index_BGain-1) > otp_param.lightCali.IndexSpec_BGain ) || 
			( abs(Index_GGain-1) > otp_param.lightCali.IndexSpec_GGain ) )
		{
			uts.log.Error(_T("光源Gain系数超范围，请重新校准光源"));
			ret = uts.errorcode.E_Fail;
			return ret;
		}

		delete dwb;
		dwb = NULL;

		delete wb;
		wb = NULL;

		CString strHour;
		strHour.Format(_T("%d"),Hour);

		WriteIniData(_T("LightCali"),_T("Hour"),strHour, sIniFileName);

		return ret;
	}

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
