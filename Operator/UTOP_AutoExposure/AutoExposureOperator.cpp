#include "StdAfx.h"
#include "AutoExposureOperator.h"
#include "UTOP_AutoExposure.h"
#include "UTSBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "UTS_Sensor.lib")


namespace UTS
{
	AutoExposureOperator::AutoExposureOperator(void)
	{
		OPERATOR_INIT;
	}

	AutoExposureOperator::~AutoExposureOperator(void)
	{
	}

	BOOL AutoExposureOperator::OnReadSpec()
	{
		CString strSection = OSUtil::GetFileName(m_strModuleFile);

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AE_En"), m_param.en, 0, _T("0: Do nothing / 1: Set register, capture image, save image"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AE_Target"), m_param.target, 170.0, _T("Exposure Target"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AE_Limit"), m_param.limit, 10.0, _T("AE Range"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AE_Try"), m_param.trycnt, 10, _T("AE work count"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AE_Filter"), m_param.filter, 20, _T("Black Level"));

		memset(&ROI_AE,0,sizeof(ROI));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AE_Width"), ROI_AE.width, 100, _T("Exposure Target"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AE_Height"), ROI_AE.height, 100, _T("AE Range"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("AE_Channel"), m_param.chn, 1, _T("0:R 1:G 2:B 3:Y"));
	
		uts.log.Debug(_T("Sensor type:%d"),uts.info.nSensorType);
		uts.log.Debug(_T("AE enable:%d  Target = %.2f  limit:%.2f  Retry:%d"),m_param.en,m_param.target,m_param.limit,m_param.trycnt);
	
		return TRUE;
	}

	BOOL AutoExposureOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);
		
		//------------------------------------------------------------------------------
		// 初始化结果
		m_dvalue = 0.0;
		*pnErrorCode = uts.errorcode.E_Pass;
		//------------------------------------------------------------------------------
		SIZE size;
		size.cx = m_bufferInfo.nWidth;
		size.cy = m_bufferInfo.nHeight;

		GetAEROI(size,ROI_AE);
		m_param.rect.left = ROI_AE.x;
		m_param.rect.top =  ROI_AE.y;
		m_param.rect.right = ROI_AE.x + ROI_AE.width;
		m_param.rect.bottom = ROI_AE.y + ROI_AE.height;

		sensor =   GetSensorInstance(m_pDevice,uts.info.nSensorType );

		
		// 重新设定Sensor序列
		CString strRegName = m_strOperatorName;
		if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
			strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
		{
			uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}

		Sleep(200);
	

		if (m_param.en && (auto_exposure(&m_param) < 0)) 
		{
			*pnErrorCode = uts.errorcode.E_Fail;
		}


		//------------------------------------------------------------------------------
end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		//------------------------------------------------------------------------------
		// 保存图片文件
		//if(m_param.en)
		//SaveImage();
		
		//------------------------------------------------------------------------------
		// 保存数据文件
		SaveData();

		return m_bResult;
	}

	void AutoExposureOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
		vecReturnValue.push_back(uts.errorcode.E_Linumance);
	}

	void AutoExposureOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		vecRegister.push_back(m_strOperatorName);
	}

	void AutoExposureOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		strHeader = _T("Time,SN,TestTime(ms),Avg,AE_Result,")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%.1f,%s,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime()
			, m_dvalue, strResult
			, strVersion, uts.info.strUserId);
	}

	void AutoExposureOperator::GetAEROI(SIZE imagesz, ROI &roi)
	{
		if (roi.x < 0 || roi.y < 0) 
		{
			roi.width = imagesz.cx / 5;
			roi.height = imagesz.cy / 5;
		}

		roi.x = (imagesz.cx - roi.width) / 2;
		roi.y = (imagesz.cy - roi.height) / 2;
	}


	int AutoExposureOperator::auto_exposure(AE_Param *ae)
	{
		int i;
		RGBTRIPLE rgb;

		int val = sensor->get_exposure();

		uts.log.Debug(_T("get_exposure = %x"),val);

		if (val < 0) {
			return -1;
		}

		int reg_delta = 0;
		double p_last = -1.0;

		for (i = 0; i < ae->trycnt; i++) 
		{
			if (!m_pDevice->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
			{
				uts.log.Error(_T("Recapture error. "));
				continue;
			}

			UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer, 
				m_bufferInfo.nWidth, 
				m_bufferInfo.nHeight, 
				ae->filter, ae->rect, rgb);

			switch (ae->chn) 
			{
			case AE_CHN_R: m_dvalue = rgb.rgbtRed; break;
			case AE_CHN_G: m_dvalue = rgb.rgbtGreen; break;
			case AE_CHN_B: m_dvalue = rgb.rgbtBlue; break;
			case AE_CHN_Y:
			default: m_dvalue = YVALUE(rgb.rgbtBlue, rgb.rgbtGreen, rgb.rgbtRed); break;
			}

			uts.log.Debug(_T("AE: E[%.1lf]"),m_dvalue);
			//------------------------------------------------------------------------------
			// OSD绘制
			UI_TEXT text;
			UI_MARK uiMark;
			UI_RECT rc;

			rc.color = text.color = COLOR_GREEN;
			memcpy(&rc.rcPos, &ae->rect, sizeof(RECT));
			uiMark.vecUiRect.push_back(rc);

			text.ptPos = CPoint((m_bufferInfo.nWidth/2) - 200, 15);
			text.strText.Format(_T("AE:[ %.1lf ]"),m_dvalue);
			text.color = COLOR_GREEN;
			uiMark.vecUIText.push_back(text);

			// 画图
			m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

			if (i == 0 && abs( m_dvalue - ae->target) <= ae->limit) return 0;

			if (i > ae->trycnt-3 && abs( m_dvalue - ae->target) <= ae->limit) goto done;

			double limit = ae->limit;
			if (limit < 2) limit = 2;

			if (abs( m_dvalue - ae->target) <= limit) goto done;
			reg_delta = get_next_exposure(m_dvalue, p_last, ae->target, reg_delta);   
			val += reg_delta;
			if (val < 0) val = 0;
			if (sensor->set_exposure(val) < 0) {
				uts.log.Debug(_T("set_exposure error: %d"),val);
				return -1;
			}

			uts.log.Debug(_T("AE: Try[%d], W[%x]"),i, val);
			Sleep(500);

			p_last = m_dvalue;
		}

		if (i == ae->trycnt) return -1;

done:

		//Save current setting
		map<int, int> regs;
		CString strRegName;
		CString valbuf; 

		if (sensor->get_exposure_settings(val, regs) < 0) {
			uts.log.Debug(_T("get_exposure_settings : [%x = %s]"),regs,val);
			return -1;
		}
		for (auto iter = regs.begin(); iter != regs.end(); ++iter) {
			valbuf.Format(_T("0x%04x 0x%x \n"), iter->first,iter->second);
			strRegName = strRegName + valbuf;
		}
		CString strSectionName;

		strSectionName.Format(_T("[%s]"), m_strOperatorName);
		uts.dbCof.DeleteV5UDeviceRegister(strSectionName);
		uts.dbCof.AddV5UDeviceRegister(strSectionName,strRegName);

		return 0;
	}

	int AutoExposureOperator::get_next_exposure(double p, double p_last, double target, int reg_delta)
	{
		if (p_last < 0) {
			return target - p > 0 ? sensor->exposure_first_step : -1*sensor->exposure_first_step;
		}

		double delta = 0;

		if (p > 254.5) {
			int val = sensor->get_exposure();
			delta = val*-2.0/3.0;
			return abs(delta) < sensor->exposure_max_step ? 
				ROUND(delta) : -1 * sensor->exposure_max_step; 
		}

		delta = (target - p) * reg_delta / (p - p_last);
		if (abs(delta) > sensor->exposure_max_step) {
			delta = delta > 0 ? sensor->exposure_max_step : -1*sensor->exposure_max_step;
		} else if (abs(delta) < 1.0) {
			delta = delta > 0 ? 1 : -1;
		}
		return ROUND(delta); 
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new AutoExposureOperator);
	}
	//------------------------------------------------------------------------------
}
