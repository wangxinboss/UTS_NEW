#include "StdAfx.h"
#include "OC_SmartisanOperator.h"
#include "UTOP_OC_Smartisan.h"
#include "otpdb.h"
#include "CommonFunc.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "CommonFunc.lib")

namespace UTS
{
    OC_SmartisanOperator::OC_SmartisanOperator(void)
    {
        OPERATOR_INIT;
    }

    OC_SmartisanOperator::~OC_SmartisanOperator(void)
    {
    }

    BOOL OC_SmartisanOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nXPixelCountSpec"), m_param.nXPixelCountSpec, 50, _T("X Pixel count from image center spec"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nYPixelCountSpec"), m_param.nYPixelCountSpec, 50, _T("Y Pixel count from image center spec"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nPixelSize"), m_param.nPixelSize, 1.1, _T("um of 1 pixel"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nCommitData_En"), m_param.nCommit_En, 0, _T("0: Disable 1:Enable"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nBank_En"), m_param.nBank_En, 0, _T("0: Disable 1:Enable"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nBank0PixelsSpec"), m_param.nBank0PixelsSpec, 4, _T("Bank 0 distance spec"));
        return TRUE;
    }

    BOOL OC_SmartisanOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
		// 初始化
		UI_MARK uiMark;
		UI_TEXT text;
		UI_LINE line;
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
		m_dYvalue = 0.0;
		m_OffsetX = 0.0;
		m_OffsetY = 0.0;
		m_ptOcCenter.x = 0;
		m_ptOcCenter.y = 0;
		m_OffsetMaxY_X = 0.0;
		m_OffsetMaxY_Y = 0.0;
		m_ptOcCenterMaxY.x = 0;
		m_ptOcCenterMaxY.y = 0;

        //------------------------------------------------------------------------------
        // 抓图
        if (m_param.nReCapture != 0)
        {
            // 重新设定Sensor序列
            CString strRegName = m_strOperatorName;
            if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
                strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
            {
                uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
                *pnErrorCode = uts.errorcode.E_Fail;
                goto end;
            }

            // 抓图
            if (!m_pDevice->Recapture(
                m_bufferObj,
                uts.info.nLTDD_DummyFrame,
                uts.info.nLTDD_AvgFrame))
            {
                uts.log.Error(_T("Recapture error. "));
                *pnErrorCode = uts.errorcode.E_NoImage;
                goto end;
            }
        }
        else
        {
            // 使用上次的抓图
            m_pDevice->GetCapturedBuffer(m_bufferObj);
        }

        // 画图
        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

        //------------------------------------------------------------------------------
        // 判断画面平均亮度
        UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
        if (m_dYvalue < m_param.dLTMinY || m_dYvalue > m_param.dLTMaxY)
        {
            *pnErrorCode = uts.errorcode.E_Linumance;
            goto end;
        }

        //------------------------------------------------------------------------------

		unsigned char *RAW8Image = new unsigned char[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
		memcpy(RAW8Image,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth* m_bufferInfo.nHeight);
		unsigned char *bmpImage = new unsigned char[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
		memcpy(bmpImage,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth* m_bufferInfo.nHeight*3);
		// 测试
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
		{
			line.color = COLOR_LIGHTGREY;
			line.ptBegin = CPoint(m_bufferInfo.nWidth / 2, 0);
			line.ptEnd = CPoint(m_bufferInfo.nWidth / 2, m_bufferInfo.nHeight);
			uiMark.vecUiLine.push_back(line);
			line.ptBegin = CPoint(0, m_bufferInfo.nHeight / 2);
			line.ptEnd = CPoint(m_bufferInfo.nWidth, m_bufferInfo.nHeight / 2);
			uiMark.vecUiLine.push_back(line);

			int nOutMode = 0;

			if (!m_pDevice->ReadValue(
				eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
				&nOutMode, sizeof(nOutMode)))
			{
				uts.log.Error(_T("GetSensorOutMode error."));
				*pnErrorCode = uts.errorcode.E_NoImage;
				goto end;
			}


			if (!UTS::Algorithm::Image::CImageProc::GetInstance().GetRaw8AndBmpBuffer(
				m_bufferObj.pFrameBuffer,
				m_bufferInfo.nWidth,
				m_bufferInfo.nHeight,
				Raw10Bit,
				nOutMode,
				Line4,
				RAW8Image,
				bmpImage))
			{
				RELEASE_ARRAY(RAW8Image);
				RELEASE_ARRAY(bmpImage);
				uts.log.Error(_T("GetBmpBuffer Error."));
				*pnErrorCode = uts.errorcode.E_NoImage;
				goto end;
			}

			RELEASE_ARRAY(RAW8Image);
			RELEASE_ARRAY(bmpImage);

            OpticalCenterRGB(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_ptOcCenter);
            m_OffsetX = (m_ptOcCenter.x - m_bufferInfo.nWidth / 2.0) * m_param.nPixelSize;
			m_OffsetY = (m_ptOcCenter.y - m_bufferInfo.nHeight / 2.0) * m_param.nPixelSize;
			//cross OC_std
			line.color = COLOR_RED;
			line.ptBegin = CPoint(m_ptOcCenter.x, 0);
			line.ptEnd = CPoint(m_ptOcCenter.x, m_bufferInfo.nHeight);
			uiMark.vecUiLine.push_back(line);
			line.ptBegin = CPoint(0, m_ptOcCenter.y);
			line.ptEnd = CPoint(m_bufferInfo.nWidth, m_ptOcCenter.y);
			uiMark.vecUiLine.push_back(line);
			text.ptPos = CPoint(50, 50);
			text.strText.Format(_T("OC_Std %d,%d p"),m_ptOcCenter.x-m_bufferInfo.nWidth/2,m_ptOcCenter.y-m_bufferInfo.nHeight/2);
			text.color = COLOR_RED;
			uiMark.vecUIText.push_back(text);		
        }
        else
        {
            uts.log.Error(_T("Operator %s do not support buffer type %d."), m_strOperatorName, m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 判断规格
        if (m_ptOcCenter.x < (m_bufferInfo.nWidth / 2 - m_param.nXPixelCountSpec)
            || m_ptOcCenter.x > (m_bufferInfo.nWidth / 2 + m_param.nXPixelCountSpec)
            || m_ptOcCenter.y < (m_bufferInfo.nHeight / 2 - m_param.nYPixelCountSpec)
            || m_ptOcCenter.y > (m_bufferInfo.nHeight / 2 + m_param.nYPixelCountSpec))
        {
            *pnErrorCode = uts.errorcode.E_OC;
        }
        else
        {
			*pnErrorCode = uts.errorcode.E_Pass;
// 			m_OffsetX=3;
// 			m_OffsetY=3;
			if(m_param.nBank_En)
			{
				if(pow(pow(m_OffsetX,2)+pow(m_OffsetY,2),0.5)<(m_param.nBank0PixelsSpec*m_param.nPixelSize))
					m_Bank=0;
				else
				{
					if(m_OffsetX==0)
					{
						if(m_OffsetY>0)
							m_Bank=1;
						else
							m_Bank=3;
					}
					else
					{
						if(m_OffsetY==0)
						{
							if(m_OffsetX>0)
								m_Bank=2;
							else
								m_Bank=4;
						}
						else
						{
							if(abs(m_OffsetY/m_OffsetX)>=1)
							{
								if(m_OffsetY>0)
									m_Bank=1;
								else
									m_Bank=3;
							}
							else
							{
								if(m_OffsetX>0)
									m_Bank=2;
								else
									m_Bank=4;
							}
						}
					}
				}			
				text.ptPos = CPoint((m_bufferInfo.nWidth/2) - 200, 400);
				text.strText.Format(_T("Bank %d"),m_Bank);
				text.color = COLOR_YELLOW;
				uiMark.vecUIText.push_back(text);
			}
			if(m_param.nCommit_En == 1)
			{
				USES_CONVERSION;

				uint8_t tmp[4];
			
				put_le_val( m_ptOcCenter.x, tmp  , 2);
				put_le_val( m_ptOcCenter.y, tmp +2, 2);

				int ret = 0;
				uts.otpdb->get_instance();

				if (is_otp_data_locked(uts.otpdb->GetModuleID(T2A(uts.info.strSN))) == FALSE)
				{
					ret = uts.otpdb->UpdateOtpByType(uts.otpdb->GetModuleID(T2A(uts.info.strSN)), 14, (char *)tmp, sizeof(tmp));
					if (ret < 0)
					{
						uts.log.Error(_T("update OC  data error!!!"));
					}
				}else
				{
					uts.log.Debug(_T("OC  data omit!!!"));
				}
			}
		}

end:
		if(m_param.nBank_En)
		{
			text.ptPos = CPoint((m_bufferInfo.nWidth/2) - 200, 15);
			text.strText.Format(_T("OC_X:[ %.1lf ], OC_Y:[ %.1lf ] um"),m_OffsetX,m_OffsetY);
			text.color = COLOR_BLUE;
			uiMark.vecUIText.push_back(text);
			m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
		}
        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        //------------------------------------------------------------------------------
        // 保存图片文件
        if (m_param.nReCapture != 0)
        {
            SaveImage();
        }

        //------------------------------------------------------------------------------
        // 保存数据文件
        SaveData();

        return m_bResult;
    }

    void OC_SmartisanOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_OC);
    }

    void OC_SmartisanOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void OC_SmartisanOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),Y_Avg,OC_Result,")
            _T("OC_OFFECT_X,OC_OFFSET_Y,Center_X,Center_Y,OC_OFFECT_MaxY_X,OC_OFFSET_MaxY_Y,Center_MaxY_X,Center_MaxY_Y,Bank,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%.1f,%s,")
            _T("%.1f,%.1f,")
			_T("%d,%d,")
			_T("%.1f,%.1f,")
			_T("%d,%d,%d,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_dYvalue, strResult
            , m_OffsetX, m_OffsetY, m_ptOcCenter.x, m_ptOcCenter.y,m_OffsetMaxY_X, m_OffsetMaxY_Y, m_ptOcCenterMaxY.x, m_ptOcCenterMaxY.y,m_Bank
            , strVersion, uts.info.strUserId);
    }

	BOOL OC_SmartisanOperator::is_otp_data_locked(int module_id)
	{
		time_t time;
		if (uts.otpdb->get_otp_data_lock_time(module_id, &time) < 0)
		{
			return is_otp_programed(module_id);
		}
		return (time > 0 ? TRUE : FALSE);
	}

	BOOL OC_SmartisanOperator::is_otp_programed(int module_id)
	{
		time_t time;
		if (uts.otpdb->GetOtpBurnTime(module_id, &time) < 0)
		{
			return FALSE;
		}
		return (time > 0 ? TRUE : FALSE);
	}

    
    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new OC_SmartisanOperator);
    }
    //------------------------------------------------------------------------------
}
