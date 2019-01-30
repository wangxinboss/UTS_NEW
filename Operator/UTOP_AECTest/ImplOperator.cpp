#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_AECTest.h"


#include "VCM.h"
#pragma comment(lib, "UTS_VCM.lib")

#include "CommonFunc.h"
#pragma comment(lib, "CommonFunc.lib")

#include "otpdb.h"
#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "UTS_Sensor.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::Image;

namespace UTS
{
	ImplOperator::ImplOperator(void)
	{
		OPERATOR_INIT;
	}

	ImplOperator::~ImplOperator(void)
	{
	}

	BOOL ImplOperator::OnReadSpec()
	{
		CString strSection = OSUtil::GetFileName(m_strModuleFile);
		vector<double> vecDoubleValue;
		CString strValue;

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSaveImage"), m_param.nSaveImage, 0, _T("1.SaveImage 2.No "));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSaveOrReadGr"), m_param.nSaveOrReadGr, 0, _T("0.Save(for aux) 1.Read(for main)"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nTestIndex"), m_param.nTestIndex, 0, _T("0.Main 1.Aux 2.test on one computer"));


		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinG"), m_param.dLTMinG, 160, _T("G channel exposure value low 180"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxG"), m_param.dLTMaxG, 200, _T("G channel exposure value high 200"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("SensorBlack_level"), m_param.SensorBlack_level, 16, _T("SensorBlack_level"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dFPNSpec"), m_param.dFPNSpec, 3.5, _T("dFPNSpec"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("ROI"),strValue,  _T("0,0"), _T("width,height default: 1/5 center"));
		uts.log.Debug(_T("section:%s ROI:%s"),strSection,strValue);
		SplitDouble(strValue, vecDoubleValue);
		m_param.roi.width = (int)vecDoubleValue[0];
		m_param.roi.height  = (int)vecDoubleValue[1];

		return TRUE;
	}

	BOOL ImplOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		//------------------------------------------------------------------------------
		

		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_POINT pt;
		UI_TEXT text;
		UI_TEXT_REL textRel;
		UI_LINE line;
		rc.rcPos.left = m_param.roi.x;
		rc.rcPos.top =  m_param.roi.y;
		rc.rcPos.right = m_param.roi.x + m_param.roi.width;
		rc.rcPos.bottom = m_param.roi.y + m_param.roi.height;
		rc.color =  COLOR_RED;

		uiMark.vecUiRect.push_back(rc);

		//------------------------------------------------------------------------------

		 return TRUE;
	}


	BOOL ImplOperator::UploadData(BYTE *data, int nLength, int nType)
	{

		if (uts.otpdb->UpdateOtpByType(m_mid, nType, (char*)data, nLength) < 0)
		{
			uts.log.Error(_T("Failed to upload  data to DB!"));
			return FALSE;
		}
		else
		{
			uts.log.Debug(_T("success to  upload  data to DB!"));
		}

		return TRUE;
	}


	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);

		//------------------------------------------------------------------------------
		// 初始化结果
		*pnErrorCode = uts.errorcode.E_Pass;
		memset(&m_result,0,sizeof(OPERATOR_RESULT));
		//------------------------------------------------------------------------------
		sensor =   GetSensorInstance(m_pDevice,uts.info.nSensorType );

		CString strRegName = m_strOperatorName;
		if (!m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REGISTER_SET,
			strRegName.GetBuffer(), strRegName.GetLength() * sizeof(TCHAR)))
		{
			uts.log.Error(_T("Device WriteValue DWVT_REGISTER_SET [%s] Error."), strRegName);
			*pnErrorCode = uts.errorcode.E_Fail;
			return FALSE;
		}

		
		Sleep(200);
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

		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

		Sleep(200);
		if (!m_pDevice->Recapture(
			m_bufferObj,
			uts.info.nLTDD_DummyFrame,
			uts.info.nLTDD_AvgFrame))
		{
			uts.log.Error(_T("Recapture error. "));
			*pnErrorCode = uts.errorcode.E_NoImage;
			goto end;
		}


		//check FPN
		FixPatternNoiseY(m_bufferObj.pBmpBuffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax);
		if (m_FPNInfo.m_dRowDifMax > m_param.dFPNSpec || m_FPNInfo.m_dColDifMax > m_param.dFPNSpec)
		{
			*pnErrorCode = uts.errorcode.E_FixPatternNoise;
			uts.log.Error(_T("check FPN spec NG m_dRowDifMax %f m_dColDifMax %f dThreshold %f"), m_FPNInfo.m_dRowDifMax,m_FPNInfo.m_dColDifMax,m_param.dFPNSpec);
			goto end;
		}

		if(uts.info.nOtpDBType == 0)
		{
			uts.otpdb->get_instance();
			USES_CONVERSION;
			//获取mid
			m_mid = uts.otpdb->GetModuleID(T2A(uts.info.strSN));
		}

		if(m_mid < 0)
		{
			*pnErrorCode = OTPCALI_ERROR_CHKERR;
			goto end;
		}
		uts.log.Debug(_T("m_mid is %d"),m_mid);


		//------------------------------------------------------------------------------
		// 保存图片文件
		if (m_param.nSaveImage != 0)
		{
			SaveImage(_T("AEC_Raw"));
		}


		//计算ROI
		if (m_param.roi.width < 200 || m_param.roi.height < 200) 
		{
			uts.log.Error(_T("ROI size is abnormal!"));
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		} 


		m_param.roi.x = (m_bufferInfo.nWidth - m_param.roi.width) / 2;
		m_param.roi.y = (m_bufferInfo.nHeight - m_param.roi.height) / 2;


		int nOutMode = 0;
		if (!m_pDevice->ReadValue(
			eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
			&nOutMode, sizeof(nOutMode)))
		{
			uts.log.Error(_T("GetSensorOutMode error."));
			*pnErrorCode = OTPCALI_ERROR_WBCALI;
			goto end;
		}

		
		WB_DATA_DOUBLE wb_data_d;
		WB_DATA_UCHAR wb_data_c;
		memset(&wb_data_d,0,sizeof(WB_DATA_DOUBLE));
		memset(&wb_data_c,0,sizeof(WB_DATA_UCHAR));

		UTS::Algorithm::WB::WBCalibration(m_bufferObj.pRaw8Buffer, m_bufferInfo.nWidth,m_bufferInfo.nHeight, nOutMode,
			m_param.roi.x, m_param.roi.y, m_param.roi.width, m_param.roi.height,
			m_param.SensorBlack_level, (double*)&wb_data_d, 
			(uint8_t*)&wb_data_c);

		uts.log.Debug(_T("get iamge data_c :R[%d], Gr[%d], Gb[%d], B[%d]"),wb_data_c.R,wb_data_c.Gr,wb_data_c.Gb,wb_data_c.B);
		uts.log.Debug(_T("get iamge data_d :R[%f], Gr[%f], Gb[%f], B[%f]"),wb_data_d.R,wb_data_d.Gr,wb_data_d.Gb,wb_data_d.B);


		// 判断影像中心G值是否符合要求
		if (!IS_IN_RANGE(wb_data_c.Gr,m_param.dLTMinG,m_param.dLTMaxG))
		{
			uts.log.Error(_T("Gr value %d not in spec %.2f~%.2f"),wb_data_c.Gr,m_param.dLTMinG,m_param.dLTMaxG);
			*pnErrorCode = uts.errorcode.E_Linumance;
			
			goto end;
		}



		m_result.TestGrValue = wb_data_d.Gr;
		m_nExposureValue = sensor->get_exposure();
		uts.log.Info(_T("m_nExposureValue is %d"),m_nExposureValue);
		
		if(0 == m_param.nTestIndex)  //测试主摄
		{
			uts.log.Debug(_T("start upload main AEC data!"));
			memcpy(m_data,&m_result.TestGrValue,sizeof(m_result.TestGrValue));
			if(!UploadData(m_data,sizeof(m_result.TestGrValue),OTPDB_AEC_MAIN))
			{
				*pnErrorCode = OTPCALI_ERROR_DB;
				uts.log.Error(_T("upload main AEC data fail!"));
				goto end;
			}
		}
		else if(1 == m_param.nTestIndex) //测试辅摄
		{
			uts.log.Debug(_T("start upload aux AEC data!"));
			memcpy(m_data,&m_result.TestGrValue,sizeof(m_result.TestGrValue));
			if(!UploadData(m_data,sizeof(m_result.TestGrValue),OTPDB_AEC_Aux))
			{
				*pnErrorCode = OTPCALI_ERROR_DB;
				uts.log.Error(_T("upload aux AEC data fail!"));
				goto end;
			}
		}
		else  //同一台电脑测试
		{
			//双摄组装之后因为在一台电脑测试，可以使用文件保存，在主摄测试计算AEC ratio.
			char* file_name = "d:\\Gr_Value";
			if(0 == m_param.nSaveOrReadGr)  //辅摄测试保存数据
			{
				uts.log.Debug(_T("write Gr value %f to file"),m_result.TestGrValue);
				file_write(file_name,0,&m_result.TestGrValue,sizeof(m_result.TestGrValue));

			}
			else if(1 == m_param.nSaveOrReadGr)  //主摄测试读取辅摄数据，并计算AEC ratio.
			{
				file_read(file_name,0,&m_result.ReadGrValue,sizeof(m_result.ReadGrValue));
				uts.log.Debug(_T("read Gr value %f from file"),m_result.ReadGrValue);

				//读到之后写0，避免下次使用相同的值
				double value = 0;
				file_write(file_name,0,&value,sizeof(value));

				if(IS_IN_RANGE(m_result.ReadGrValue,40,60))
				{
					m_result.nBrightNessRatio = int(m_result.ReadGrValue/m_result.TestGrValue*1024);
					uts.log.Debug(_T("nBrightNessRatio is  %d"),m_result.nBrightNessRatio);
					BYTE tmp[2]={0};
					put_be_val( m_result.nBrightNessRatio , tmp, 2);

					uts.log.Debug(_T("start upload aux AEC ratio data!"));
					if(!UploadData(tmp,2,OTPDB_AEC_Ratio))
					{
						*pnErrorCode = OTPCALI_ERROR_DB;
						uts.log.Error(_T("upload aux AEC data fail!"));
						goto end;
					}
				}
				else
				{
					//没有读到辅摄测试数据或者数据超出范围
					uts.log.Error(_T("Read Gr value is %.2f Not in rage 40~60!"),m_result.ReadGrValue);
					*pnErrorCode = uts.errorcode.E_Fail;
					goto end;
				}
			}

		}
	


end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);
		//------------------------------------------------------------------------------


		// 保存数据文件
		SaveData();

		return m_bResult;
	}



	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
		vecReturnValue.push_back(uts.errorcode.E_Linumance);
	}

	void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
	{
		vecRegister.clear();
		vecRegister.push_back(m_strOperatorName);
	}

	void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
	{
		CString strVersion;
		UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
		CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

		strHeader = _T("Time,SN,TestTime(ms),TestGrValue,ReadGrValue,nBrightNessRatio,nExposureTime,Result,")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.2f,%.2f,%f,%d,%d,%s,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(),m_result.TestGrValue,m_result.ReadGrValue,m_result.nBrightNessRatio,m_nExposureValue,strResult
			, strVersion, uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
