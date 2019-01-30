#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_XCalibration.h"
#include "Algorithm.h"
#include "XCalibration.h"
#include "XCalibLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("1.dRoll"), m_param.dRoll, 1.0, _T("roll"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("2.dPitch"), m_param.dPitch, 1.0, _T("pitch"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("3.dYaw"), m_param.dYaw, 1.2, _T("yaw"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("4.dTilt"), m_param.dTilt, 1.2, _T("tilt"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("5.dShift_X"), m_param.dShift_X, 50.0, _T("One eye shift X"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("6.dShift_Y"), m_param.dShift_Y, 50.0, _T("One eye shift Y"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("7.dShift_All_X"), m_param.dShift_All_X, 120.0, _T("Total shift X"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("8.dShift_All_Y"), m_param.dShift_All_Y, 120.0, _T("Total shift Y"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("9.dPixel_Size"), m_param.dPixel_Size, 3.75, _T("Pixel size"));
		//uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
		//uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSaveimageMode"), m_param.nSaveImageMode, 1, _T("0: Null 1: BMP 2:RAW8 3:RAW10"));
		
		CString strValue;
		vector<double> vecDoubleValue;

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("Spec.dTx"), strValue, _T("160,163"), _T("TxSpec(Min,Max)"));
		SplitDouble(strValue, vecDoubleValue);
		m_param.dTxSpec.min = vecDoubleValue[0];
		m_param.dTxSpec.max = vecDoubleValue[1];

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
		USES_CONVERSION;
		//Save txt & dat
		CString strDirPath;
		CString strFilePath,strFilePathR,strFilePathL,strTXTFilePath;

		SYSTEMTIME st;
		GetLocalTime(&st);

		strFilePath.Format(_T("%sXCal_%04d%02d%02d\\%s\\stereo.dat"),
			uts.info.strResultPath,
			st.wYear,
			st.wMonth, 
			st.wDay,
			uts.info.strSN);
		strDirPath.Format(_T("%sXCal_%04d%02d%02d\\%s"),
			uts.info.strResultPath,
			st.wYear,
			st.wMonth, 
			st.wDay,
			uts.info.strSN);
		strTXTFilePath.Format(_T("%s\\stereo.txt"),strDirPath);

		//------------------------------------------------------------------------------
		// 抓图
		TCHAR ConfigFileName[0x100];
		_stprintf(ConfigFileName, _T("%s\\%s"), uts.info.strRootPath,_T("XCalConfig\\config.yml"));

		if (!LoadSettingsAndSettingsures(W2A(ConfigFileName)))
		{
			uts.log.Error(_T("Load Config Error"));
			*pnErrorCode = uts.errorcode.E_Fail;
		}

// 		TCHAR ImagePath[0x100];
// 		_stprintf(ImagePath, _T("%s\\%s"), uts.info.strRootPath,_T("XCalImage\\"));

		vector<char*> strCalibFiles;

// 		strDirPath.Format(_T("%s\\XCal_%04d%02d%02d\\%s"),
// 			uts.info.strResultPath,
// 			st.wYear,
// 			st.wMonth, 
// 			st.wDay,
// 			uts.info.strSN);

		strFilePathL.Format(_T("%s\\L.bmp"),strDirPath);
		strFilePathR.Format(_T("%s\\R.bmp"),strDirPath);

		strCalibFiles.push_back(W2A(strFilePathL));
		strCalibFiles.push_back(W2A(strFilePathR));

		m_result.Result_Cali = XStereoCalibrate(0, strCalibFiles.size(), strCalibFiles.data(), T2A(strFilePath));
		if (X_OK != m_result.Result_Cali)
		{
			uts.log.Error(_T("XStereoCalibrate Error:%d"),m_result.Result_Cali);
			*pnErrorCode = uts.errorcode.E_XCal_Fail;
		}
		else
		{
			XCalibration = new XCalibration_t;
			TCHAR *Xfilename = (LPTSTR)(LPCTSTR)strTXTFilePath;

			ParseXCalibrationTxt(Xfilename,XCalibration);
			//砏癸莱...
			dXTilt = pow(pow(XCalibration->roll,2) + 
					     pow(XCalibration->pitch,2) +
						 pow(XCalibration->yaw,2),0.5);

			XShift1_X = (XCalibration->K0Matrix13-m_bufferInfo.nWidth/2)*m_param.dPixel_Size;
			XShift1_Y = (XCalibration->K0Matrix23-m_bufferInfo.nHeight/2)*m_param.dPixel_Size;
			XShift2_X = (XCalibration->K1Matrix13-m_bufferInfo.nWidth/2)*m_param.dPixel_Size;
			XShift2_Y = (XCalibration->K1Matrix23-m_bufferInfo.nHeight/2)*m_param.dPixel_Size;
			XShift_ALL_X = XShift1_X-XShift2_X;
			XShift_ALL_Y = XShift1_Y-XShift2_Y;
			dTx = XCalibration->TMatrix11;

			if(abs(XCalibration->roll) >= m_param.dRoll)
			{
				uts.log.Error(_T("XStereoCalibrate [roll] Fail:%2f (%2f)"),XCalibration->roll,m_param.dRoll);
				*pnErrorCode = uts.errorcode.E_XCal_Roll;
			}
			if(abs(XCalibration->pitch) >= m_param.dPitch)
			{
				uts.log.Error(_T("XStereoCalibrate [pitch] Fail:%2f (%2f)"),XCalibration->pitch,m_param.dPitch);
				*pnErrorCode = uts.errorcode.E_XCal_Pitch;
			}
			if(abs(XCalibration->yaw) >= m_param.dYaw)
			{
				uts.log.Error(_T("XStereoCalibrate [yaw] Fail:%2f (%2f)"),XCalibration->yaw,m_param.dYaw);
				*pnErrorCode = uts.errorcode.E_XCal_Yaw;
			}
			if(abs(dXTilt) >= m_param.dTilt)
			{
				uts.log.Error(_T("XStereoCalibrate [Tilt] Fail:%2f (%2f)"),dXTilt,m_param.dTilt);
				*pnErrorCode = uts.errorcode.E_XCal_Tilt;
			}
			if(abs(XShift1_X) >= m_param.dShift_X)
			{
				uts.log.Error(_T("XStereoCalibrate [XShift1_X] Fail:%2f (%2f)"),XShift1_X,m_param.dShift_X);
				*pnErrorCode = uts.errorcode.E_XCal_OCShift_X1;
			}
			if(abs(XShift1_Y) >= m_param.dShift_Y)
			{
				uts.log.Error(_T("XStereoCalibrate [XShift1_Y] Fail:%2f (%2f)"),XShift1_Y,m_param.dShift_Y);
				*pnErrorCode = uts.errorcode.E_XCal_OCShift_Y1;
			}
			if(abs(XShift2_X) >= m_param.dShift_X)
			{
				uts.log.Error(_T("XStereoCalibrate [XShift2_X] Fail:%2f (%2f)"),XShift2_X,m_param.dShift_X);
				*pnErrorCode = uts.errorcode.E_XCal_OCShift_X2;
			}
			if(abs(XShift2_Y) >= m_param.dShift_Y)
			{
				uts.log.Error(_T("XStereoCalibrate [XShift2_Y] Fail:%2f (%2f)"),XShift2_Y,m_param.dShift_Y);
				*pnErrorCode = uts.errorcode.E_XCal_OCShift_Y2;
			}
			if(abs(XShift_ALL_X) >= m_param.dShift_All_X)
			{
				uts.log.Error(_T("XStereoCalibrate [XShift_ALL_X] Fail:%2f (%2f)"),XShift_ALL_X,m_param.dShift_All_X);
				*pnErrorCode = uts.errorcode.E_XCal_OCShift_TotalX;
			}
			if(abs(XShift_ALL_Y) >= m_param.dShift_All_Y)
			{
				uts.log.Error(_T("XStereoCalibrate [XShift_ALL_Y] Fail:%2f (%2f)"),XShift_ALL_Y,m_param.dShift_All_Y);
				*pnErrorCode = uts.errorcode.E_XCal_OCShift_TotalY;
			}
			if(dTx >= m_param.dTxSpec.max || 
			   dTx <= m_param.dTxSpec.min )
			{
				uts.log.Error(_T("XStereoCalibrate [dTx] Fail:%2f (%2f,%2f)"),dTx,m_param.dShift_All_Y);
				*pnErrorCode = uts.errorcode.E_XCal_Tx;
			}


		}


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
		vecReturnValue.push_back(uts.errorcode.E_XCal_Fail);
		vecReturnValue.push_back(uts.errorcode.E_XCal_Roll);
		vecReturnValue.push_back(uts.errorcode.E_XCal_Pitch);
		vecReturnValue.push_back(uts.errorcode.E_XCal_Yaw);
		vecReturnValue.push_back(uts.errorcode.E_XCal_Tilt);
		vecReturnValue.push_back(uts.errorcode.E_XCal_OCShift_X1);
		vecReturnValue.push_back(uts.errorcode.E_XCal_OCShift_Y1);
		vecReturnValue.push_back(uts.errorcode.E_XCal_OCShift_X2);
		vecReturnValue.push_back(uts.errorcode.E_XCal_OCShift_Y2);
		vecReturnValue.push_back(uts.errorcode.E_XCal_OCShift_TotalX);
		vecReturnValue.push_back(uts.errorcode.E_XCal_OCShift_TotalY);
		vecReturnValue.push_back(uts.errorcode.E_XCal_Tx);
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

		strHeader.Format(
			_T("Time,SN,TestTime(ms),Result,")
			_T("rms0,rms1,rms,")
			_T("fx0,fy0,ocx0,ocy0,")
			_T("fx1,fy1,ocx1,ocy1,")
			_T("ocx0-ocx1,ocy0-ocy1,roll,pitch,yaw,tilt,")
			_T("Tx,Ty,Tz,")
			_T("Result Cali,")
			_T("Version,OP_SN\n")
			_T("Lower Limit,,,,,,,")
			_T(",,-%.1f,-%.1f,")
			_T(",,-%.1f,-%.1f,")
			_T("-%.1f,-%.1f,-%.1f,-%.1f,-%.1f,0,%.1f")
			_T(",,,,,\n")
			_T("Normal,,,,,,,,,,,,,,,,,,,,,%.1f,,\n")
			_T("Upper Limit,,,,,,,")
			_T(",,%.1f,%.1f,")
			_T(",,%.1f,%.1f,")
			_T("%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f")
			_T(",,,,,\n")
			,m_param.dShift_X,m_param.dShift_Y
			,m_param.dShift_X,m_param.dShift_Y
			,m_param.dShift_All_X,m_param.dShift_All_Y,m_param.dRoll,m_param.dPitch,m_param.dYaw,m_param.dTxSpec.min
			,(m_param.dTxSpec.min + m_param.dTxSpec.max)/2
			,m_param.dShift_X,m_param.dShift_Y
			,m_param.dShift_X,m_param.dShift_Y
			,m_param.dShift_All_X,m_param.dShift_All_Y,m_param.dRoll,m_param.dPitch,m_param.dYaw,m_param.dTilt,m_param.dTxSpec.max);

		strData.Format(
			_T("%s,%s,%.1f,%s,")
			_T("%.4f,%.4f,%.4f,")
			_T("%.4f,%.4f,%.4f,%.4f,")
			_T("%.4f,%.4f,%.4f,%.4f,")
			_T("%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,")
			_T("%.4f,%.4f,%.4f,")
			_T("%d,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
			, XCalibration->rms0,XCalibration->rms1,XCalibration->rms
			, XCalibration->K0Matrix13,XCalibration->K0Matrix23,XShift1_X,XShift1_Y
			, XCalibration->K1Matrix13,XCalibration->K1Matrix23,XShift2_X,XShift2_Y
			, XShift_ALL_X,XShift_ALL_Y,XCalibration->roll,XCalibration->pitch,XCalibration->yaw,dXTilt
			,XCalibration->TMatrix11,XCalibration->TMatrix12,XCalibration->TMatrix13
			, m_result.Result_Cali
			, strVersion, uts.info.strUserId);
// 		strHeader = _T("Time,SN,TestTime(ms),Result,")
// 			_T("Roll,Pitch,Yaw,Tilt,")
// 			_T("X_0,Y_0,ShiftX_0,ShiftY_0,")
// 			_T("X_1,Y_1,ShiftX_1,ShiftY_1,")
// 			_T("ShiftX_All,ShiftY_All,Tx,Ty,Tz,")
// 			_T("Result Cali,")
// 			_T("Version,OP_SN\n");
// 
// 		strData.Format(
// 			_T("%s,%s,%.1f,%s,")
// 			_T("%.4f,%.4f,%.4f,%.4f,")
// 			_T("%.1f,%.1f,%.4f,%.4f,")
// 			_T("%.1f,%.1f,%.4f,%.4f,")
// 			_T("%.4f,%.4f,%.4f,%.4f,%.4f,")
// 		    _T("%d,")
// 			_T("%s,%s\n")
// 			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
// 			, XCalibration->roll,XCalibration->pitch,XCalibration->yaw,dXTilt
// 			, XCalibration->K0Matrix13,XCalibration->K0Matrix23,XShift1_X,XShift1_Y
// 			, XCalibration->K1Matrix13,XCalibration->K1Matrix23,XShift2_X,XShift2_Y
// 			, XShift_ALL_X,XShift_ALL_Y,XCalibration->TMatrix11,XCalibration->TMatrix12,XCalibration->TMatrix13
// 			, m_result.Result_Cali
// 			, strVersion, uts.info.strUserId);
	}
	//------------------------------------------------------------------------------
	void ImplOperator::ParseXCalibrationTxt(TCHAR *FileName, XCalibration_t *XCalibration)
	{
		HANDLE hFile;
		BYTE buf[4096];
		BYTE StrBuf[1024];
		UINT32 count;
		DWORD dwBytesWritten;
		char *content;

		hFile = CreateFile(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE != hFile) {
			memset(buf, 0, 4096);
			dwBytesWritten = 0;
			ReadFile(hFile, buf, 4096, &dwBytesWritten, NULL);
			CloseHandle(hFile);

			for (count = 0; count < dwBytesWritten; count++) {
				content = (char *)&buf[count];

				if (0 == strncmp("rms0: ", (const char *)&buf[count], 6)) {
					sscanf((const char *)&buf[count + 6], "%s", &StrBuf);
					XCalibration->rms0 = atof((const char *)StrBuf);
					count = count + 6 + strlen((const char *)StrBuf);
				}

				if (0 == strncmp("rms1: ", (const char *)&buf[count], 6)) {
					sscanf((const char *)&buf[count + 6], "%s", &StrBuf);
					XCalibration->rms1 = atof((const char *)StrBuf);
					count = count + 6 + strlen((const char *)StrBuf);
				}

				if (0 == strncmp("rms: ", (const char *)&buf[count], 5)) {
					sscanf((const char *)&buf[count + 6], "%s", &StrBuf);
					XCalibration->rms = atof((const char *)StrBuf);
					count = count + 5 + strlen((const char *)StrBuf);
				}

				if (0 == strncmp("roll: ", (const char *)&buf[count], 6)) {
					sscanf((const char *)&buf[count + 6], "%s", &StrBuf);
					XCalibration->roll = atof((const char *)StrBuf);
					count = count + 6 + strlen((const char *)StrBuf);
				}

				if (0 == strncmp("pitch: ", (const char *)&buf[count], 7)) {
					sscanf((const char *)&buf[count + 7], "%s", &StrBuf);
					XCalibration->pitch = atof((const char *)StrBuf);
					count = count + 7 + strlen((const char *)StrBuf);
				}

				if (0 == strncmp("yaw: ", (const char *)&buf[count], 5)) {
					sscanf((const char *)&buf[count + 5], "%s", &StrBuf);
					XCalibration->yaw = atof((const char *)StrBuf);
					count = count + 5 + strlen((const char *)StrBuf);
				}

				if (0 == strncmp("K0: !!opencv-matrix\r\n   rows: 3\r\n   cols: 3\r\n   dt: d\r\n   data: [ ", (const char *)&buf[count], 66)) {
					sscanf((const char *)&buf[count + 66], "%s", &StrBuf);
					XCalibration->K0Matrix11 = atof((const char *)StrBuf);
					count = count + 66 + strlen((const char *)StrBuf);

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K0Matrix12 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1; // 1 space

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K0Matrix13 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K0Matrix21 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1; // 1 space

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K0Matrix22 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 2 + 7; // \r\n + 7 space

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K0Matrix23 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K0Matrix31 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K0Matrix32 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K0Matrix33 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1 + 4; // space ] \r\n
				}

				if (0 == strncmp("K1: !!opencv-matrix\r\n   rows: 3\r\n   cols: 3\r\n   dt: d\r\n   data: [ ", (const char *)&buf[count], 66)) {
					sscanf((const char *)&buf[count + 66], "%s", &StrBuf);
					XCalibration->K1Matrix11 = atof((const char *)StrBuf);
					count = count + 66 + strlen((const char *)StrBuf);

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K1Matrix12 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1; // 1 space

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K1Matrix13 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K1Matrix21 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1; // 1 space

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K1Matrix22 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 2 + 7; // \r\n + 7 space

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K1Matrix23 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K1Matrix31 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K1Matrix32 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->K1Matrix33 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1 + 4; // space ] \r\n
				}

				if (0 == strncmp("R: !!opencv-matrix\r\n   rows: 3\r\n   cols: 3\r\n   dt: d\r\n   data: [ ", (const char *)&buf[count], 65)) {
					sscanf((const char *)&buf[count + 65], "%s", &StrBuf);
					XCalibration->RMatrix11 = atof((const char *)StrBuf);
					count = count + 65 + strlen((const char *)StrBuf);

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->RMatrix12 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 2 + 7;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->RMatrix13 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->RMatrix21 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 2 + 7;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->RMatrix22 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->RMatrix23 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 2 + 7;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->RMatrix31 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->RMatrix32 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 2 + 7;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->RMatrix33 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1 + 4; // space ] \r\n
				}

				if (0 == strncmp("T: !!opencv-matrix\r\n   rows: 3\r\n   cols: 1\r\n   dt: d\r\n   data: [ ", (const char *)&buf[count], 65)) {
					sscanf((const char *)&buf[count + 65], "%s", &StrBuf);
					XCalibration->TMatrix11 = atof((const char *)StrBuf);
					count = count + 65 + strlen((const char *)StrBuf);

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->TMatrix12 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;

					sscanf((const char *)&buf[count], "%s", &StrBuf);
					XCalibration->TMatrix13 = atof((const char *)StrBuf);
					count = count + strlen((const char *)StrBuf) + 1;
				}

			}

		}
	}
	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
