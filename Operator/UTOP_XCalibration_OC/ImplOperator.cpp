#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_XCalibration_OC.h"
#include "Algorithm.h"
#include "XCalibration.h"
#include "XCalibLib.h"
#include "otpdb.h"
#include "CommonFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace UTS::Algorithm;
using namespace UTS::Algorithm::Image;

#pragma comment(lib, "ddm.lib")
#pragma comment(lib, "CommonFunc.lib")

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

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dPixel_Size"), m_param.dPixel_Size, 3.75, _T("Pixel size"));

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nXPixelCountSpec"), m_param.nXPixelCountSpec, 50, _T("X Pixel count from image center spec"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nYPixelCountSpec"), m_param.nYPixelCountSpec, 50, _T("Y Pixel count from image center spec"));

		CString strValue;
		vector<int> vecValue;
		vector<double> vecDoubleValue;

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("WhitePatchROI"), strValue,  _T("640,480,200,200"), _T("ROI(Left,Top,width,Height)"));
		SplitInt(strValue, vecValue);
		m_param.WhitePatchROI.left = vecValue[0];
		m_param.WhitePatchROI.top = vecValue[1];
		m_param.WhitePatchROI.right = vecValue[0] + vecValue[2];
		m_param.WhitePatchROI.bottom = vecValue[1] + vecValue[3];

		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nBank_En"), m_param.nBank_En, 0, _T("0: Disable 1:Enable"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nBank0PixelsSpec"), m_param.nBank0PixelsSpec, 4, _T("Bank 0 distance spec"));


		return TRUE;
	}

	BOOL ImplOperator::OnPreview(unsigned char *pBmpBuffer, int nWidth, int nHeight, UI_MARK &uiMark)
	{
		//------------------------------------------------------------------------------
		// 判断画面平均亮度
		int nROISizeX,nROISizeY;
		nROISizeX = m_param.WhitePatchROI.right - m_param.WhitePatchROI.left;
		nROISizeY = m_param.WhitePatchROI.bottom - m_param.WhitePatchROI.top;

		unsigned char *pROIDataWhitePatch = new BYTE[nROISizeX * nROISizeY * 3];
		memset(pROIDataWhitePatch, 0, nROISizeX * nROISizeY * 3);
		CImageProc::GetInstance().GetBMPBlockBuffer(
			pBmpBuffer,
			pROIDataWhitePatch,
			nWidth,
			nHeight,
			m_param.WhitePatchROI.left,
			m_param.WhitePatchROI.top,
			nROISizeX,
			nROISizeY);

		UTS::Algorithm::CalYavg(pROIDataWhitePatch, nROISizeX, nROISizeY, m_result.dCenterYavg);

		RELEASE_ARRAY(pROIDataWhitePatch);

		//------------------------------------------------------------------------------
		// OSD绘制
		UI_RECT rc;
		UI_TEXT text;

		rc.color = COLOR_BLUE;

		if (m_result.dCenterYavg < m_param.dLTMinY || 
			m_result.dCenterYavg > m_param.dLTMaxY)
		{
			rc.color = COLOR_RED;
		}

		rc.rcPos = m_param.WhitePatchROI;
		uiMark.vecUiRect.push_back(rc);
		text.color = rc.color;
		text.ptPos = CPoint(rc.rcPos.left, rc.rcPos.bottom);
		text.strText.Format(_T("%.1f"), m_result.dCenterYavg);
		uiMark.vecUIText.push_back(text);

		//------------------------------------------------------------------------------

		return TRUE;
	}

	bool ReadBMPImage(const wchar_t* const _fileName,unsigned char *_imageBuffer,int& _width,int& _height,int& _channelCount)
	{
		bool bRes = true;
		/*
			The CreateFile function creates or opens a file, file stream, directory, physical disk, volume, console buffer, tape drive,
			communications resource, mailslot, or named pipe. The function returns a handle that can be used to access an object.
		*/
		HANDLE hFile = CreateFile(_fileName,GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL); 				

		if( hFile == INVALID_HANDLE_VALUE ) // return if error opening file
		{
			bRes = false;
			return bRes;
		} 
            
		BITMAPINFOHEADER bmpInfoHeader = {0};
		BITMAPFILEHEADER bfh = {0};

		DWORD nBytesRead;

		BOOL bResult1 = ReadFile(hFile, &bfh, sizeof(bfh), &nBytesRead, NULL) ;
		BOOL bResult2 = ReadFile(hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &nBytesRead, NULL );

		if (_width*_height*_channelCount < bmpInfoHeader.biWidth*bmpInfoHeader.biHeight*bmpInfoHeader.biBitCount/8)
		{
			bRes = false;
			return bRes;
		}
		else
		{
			_width = bmpInfoHeader.biWidth;
			_height =bmpInfoHeader.biHeight;
			_channelCount = bmpInfoHeader.biBitCount/8;	
		}

		RGBQUAD m_lpPalette[256];
		if (_channelCount == 1)
		{
			BOOL bResult3 = ReadFile(hFile, m_lpPalette, sizeof(RGBQUAD)*256, &nBytesRead, NULL );
		}

		//	建立Buffer
		unsigned char* Data = new unsigned char[_width*_height*_channelCount];
		memset(Data,0,sizeof(unsigned char)*_width*_height*_channelCount);

		int ScanLineSize = 0;
		int _tmpV = (_width)*_channelCount;
		int ModValue = int(fmod(double(_width)*_channelCount,4));

		if (ModValue == 0)
		{
			ScanLineSize = (_width)*_channelCount;
				
		}
		else
		{
			ScanLineSize = (_width)*_channelCount+int((ModValue == 0)? (0):(4-ModValue));					
		}

		if (bmpInfoHeader.biSizeImage!=ScanLineSize*_height)
		{
			bmpInfoHeader.biSizeImage = ScanLineSize*_height;
		}

		unsigned char* pBitmapBits = new unsigned char[bmpInfoHeader.biSizeImage];
		memset(pBitmapBits,0,sizeof(unsigned char)*bmpInfoHeader.biSizeImage);

		BOOL bResult3 = ReadFile(hFile, pBitmapBits, bmpInfoHeader.biSizeImage, &nBytesRead, NULL );
			
		for (int y=0;y<_height;y++)
		{
			for (int x=0;x<_width;x++)
			{	
				for (int i = 0;i<_channelCount;i++)
				{
					Data[(y*_width+x)*(_channelCount)+(_channelCount-i-1)] = pBitmapBits[(_height-1-y)*ScanLineSize+x*_channelCount+i];
				}
			}
		}

		delete[] pBitmapBits;
		pBitmapBits = 0;
		// Close the file handle
		CloseHandle( hFile );

		memcpy(_imageBuffer,Data,sizeof(unsigned char)*_width*_height*_channelCount);

		delete[] Data;
		Data = 0;
		return bRes;
	}


	BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
	{
		//------------------------------------------------------------------------------
		// 初始化
		USES_CONVERSION;
		// 初始化
		UI_MARK uiMark;
		UI_TEXT text;
		UI_LINE line;
		m_TimeCounter.SetStartTime();
		m_pDevice->GetBufferInfo(m_bufferInfo);
		//------------------------------------------------------------------------------
		// 初始化结果
		*pnErrorCode = uts.errorcode.E_Pass;
		m_result.m_OffsetX = 0;
		m_result.m_OffsetY = 0;
		memset(&m_result,0,sizeof(OPERATOR_RESULT));
		//------------------------------------------------------------------------------
		// 抓图
		if (!m_pDevice->Recapture(m_bufferObj))
		{
			uts.log.Error(_T("Recapture error. "));
			*pnErrorCode = uts.errorcode.E_NoImage;
			return m_bResult;
		}
		//------------------------------------------------------------------------------
		// 判断画面平均亮度
		int nROISizeX,nROISizeY;
		XIM_CALIB_RESULT calRes = {0};
		unsigned char *_imageBuffer[2];

		nROISizeX = m_param.WhitePatchROI.right - m_param.WhitePatchROI.left;
		nROISizeY = m_param.WhitePatchROI.bottom - m_param.WhitePatchROI.top;

		unsigned char *pROIDataWhitePatch = new BYTE[nROISizeX * nROISizeY * 3];
		memset(pROIDataWhitePatch, 0, nROISizeX * nROISizeY * 3);
		CImageProc::GetInstance().GetBMPBlockBuffer(
			m_bufferObj.pBmpBuffer,
			pROIDataWhitePatch,
			m_bufferInfo.nWidth,
			m_bufferInfo.nHeight,
			m_param.WhitePatchROI.left,
			m_param.WhitePatchROI.top,
			nROISizeX,
			nROISizeY);

		Algorithm::CalYavgExp(
			pROIDataWhitePatch,
			nROISizeX,
			nROISizeY,
			m_result.dCenterYavg);

		RELEASE_ARRAY(pROIDataWhitePatch);

		if (m_result.dCenterYavg < m_param.dLTMinY || 
			m_result.dCenterYavg > m_param.dLTMaxY)
		{
			*pnErrorCode = uts.errorcode.E_Linumance;
			goto end;
		}

		//------------------------------------------------------------------------------
		for(int i = 0 ;i < 2;i++)
		{
			_imageBuffer[i] = new unsigned char[m_bufferInfo.nWidth*m_bufferInfo.nHeight];
		}

		// 画图
		m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);

#ifdef _DEBUG
		int _channelCount = 1;
 		ReadBMPImage(_T("XCalibration_OC.bmp"),m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth ,m_bufferInfo.nHeight,_channelCount);
#endif

		memcpy(_imageBuffer[0],m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth*m_bufferInfo.nHeight);
		memcpy(_imageBuffer[1],_imageBuffer[0],m_bufferInfo.nWidth*m_bufferInfo.nHeight);

		TCHAR ConfigFileName[0x100];
		_stprintf(ConfigFileName, _T("%s\\%s"), uts.info.strRootPath,_T("XCalConfig\\config.yml"));

		if (!LoadSettingsAndSettingsures(W2A(ConfigFileName)))
		{
			uts.log.Error(_T("Load Config Error"));
			*pnErrorCode = uts.errorcode.E_Fail;
			goto end;
		}
		int ret = XStereoCalibrate_ViaBuffer(0, 2, _imageBuffer, "stereo.dat", &calRes);

		m_result.m_ptOcCenter.x = calRes.K0[2];
		m_result.m_ptOcCenter.y = calRes.K0[5];


		uts.log.Debug(_T("OC(%.1f,%.1f)"),m_result.m_ptOcCenter.x,m_result.m_ptOcCenter.y);

		m_result.m_OffsetX = (m_result.m_ptOcCenter.x - m_bufferInfo.nWidth/2) *m_param.dPixel_Size;
		m_result.m_OffsetY = (m_result.m_ptOcCenter.y - m_bufferInfo.nHeight/2)*m_param.dPixel_Size;

		if (m_result.m_OffsetX < (-1 * m_param.nXPixelCountSpec)
			|| m_result.m_OffsetX > ( 1 * m_param.nXPixelCountSpec)
			|| m_result.m_OffsetY < (-1 * m_param.nYPixelCountSpec)
			|| m_result.m_OffsetY > ( 1 * m_param.nYPixelCountSpec))
		{
			*pnErrorCode = uts.errorcode.E_OC;
		}
		else
		{
			*pnErrorCode = uts.errorcode.E_Pass;
		}


		if(m_param.nBank_En &&
		   (*pnErrorCode == uts.errorcode.E_Pass))
		{
			if(pow(pow(m_result.m_OffsetX,2) + pow(m_result.m_OffsetY,2) , 0.5)<
				(m_param.nBank0PixelsSpec * m_param.dPixel_Size))
				m_result.m_Bank=0;
			else
			{
				if(m_result.m_OffsetX == 0)
				{
					if(m_result.m_OffsetY > 0)
						m_result.m_Bank=1;
					else
						m_result.m_Bank=3;
				}
				else
				{
					if(m_result.m_OffsetY == 0)
					{
						if(m_result.m_OffsetX > 0)
							m_result.m_Bank=2;
						else
							m_result.m_Bank=4;
					}
					else
					{
						if(abs(m_result.m_OffsetY/m_result.m_OffsetX) >= 1)
						{
							if( m_result.m_OffsetY > 0)
								m_result.m_Bank=1;
							else
								m_result.m_Bank=3;
						}
						else
						{
							if( m_result.m_OffsetX > 0)
								m_result.m_Bank=2;
							else
								m_result.m_Bank=4;
						}
					}
				}
			}	
			text.ptPos = CPoint((m_bufferInfo.nWidth/2) - 200, 400);
			text.strText.Format(_T("Bank %d"),m_result.m_Bank);
			text.color = COLOR_YELLOW;
			uiMark.vecUIText.push_back(text);

			if(*pnErrorCode == uts.errorcode.E_Pass)
			{
				//Commit
				USES_CONVERSION;
				uint8_t tmp[1];

				put_le_val( m_result.m_Bank, tmp  , 1);
		
				int ret = 0;
				uts.otpdb->get_instance();

				int mid = uts.otpdb->GetModuleID(T2A(uts.info.strSN));
				if (mid < 0)
				{
					uts.log.Error(_T("update OC bank error!!!"));
					*pnErrorCode = uts.errorcode.E_Fail;
					for(int i = 0 ;i < 2;i++)
					{
						RELEASE_ARRAY(_imageBuffer[i]);
					}
					goto end;
				}

				if (is_otp_data_locked(mid) == FALSE)
				{
					ret = uts.otpdb->UpdateOtpByType(mid, 15, (char *)tmp, sizeof(tmp));
					if (ret < 0)
					{
						uts.log.Error(_T("update OC bank error!!!"));
						*pnErrorCode = uts.errorcode.E_DBConnect;
						for(int i = 0 ;i < 2;i++)
						{
							RELEASE_ARRAY(_imageBuffer[i]);
						}
						goto end;
					}
				}else
				{
					uts.log.Debug(_T("OC  data omit!!!"));
				}
			}
		}

		

		for(int i = 0 ;i < 2;i++)
		{
			RELEASE_ARRAY(_imageBuffer[i]);
		}

end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		if(m_param.nBank_En)
		{
			text.ptPos = CPoint((m_bufferInfo.nWidth/2) - 200, 15);
			text.strText.Format(_T("OC_X:[ %.1lf ], OC_Y:[ %.1lf ] um"),m_result.m_OffsetX,m_result.m_OffsetY);
			
			if(m_bResult == TRUE)
			text.color = COLOR_BLUE;
			else
			text.color = COLOR_RED;

			uiMark.vecUIText.push_back(text);
			m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);
		}
		//------------------------------------------------------------------------------
		// 保存数据文件
		SaveData();
		//SaveImage();
		CString strDirPath;
		CString strFilePath;

		SYSTEMTIME st;
		GetLocalTime(&st);

		strDirPath.Format(_T("%s\\Image-%s-%s-%s-%04d%02d%02d\\"),
			uts.info.strResultPath,
			uts.info.strProjectName,
			uts.info.strLineName,
			uts.info.strStationName,
			st.wYear,
			st.wMonth, 
			st.wDay);

		UTS::OSUtil::CreateMultipleDirectory(strDirPath);
		strFilePath.Format(_T("%s\\%s-%s-%04d%02d%02d-%02d%02d%02d"),
			strDirPath,
			m_strOperatorName,
			uts.info.strSN,
			st.wYear,
			st.wMonth, 
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond);

		strFilePath.Append(_T(".bmp"));
		if (!uts.imgFile.WriteBMPImage(strFilePath,m_bufferObj.pRaw8Buffer,m_bufferInfo.nWidth,m_bufferInfo.nHeight,1))
		{
			*pnErrorCode = uts.errorcode.E_Fail;
		}



		return m_bResult;
	}



	void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
	{
		vecReturnValue.clear();
		vecReturnValue.push_back(uts.errorcode.E_Fail);
		vecReturnValue.push_back(uts.errorcode.E_NoImage);
		vecReturnValue.push_back(uts.errorcode.E_Linumance);
		vecReturnValue.push_back(uts.errorcode.E_XCal_OCShift_X1);
		vecReturnValue.push_back(uts.errorcode.E_XCal_OCShift_Y1);
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

		strHeader = _T("Time,SN,TestTime(ms),Y_Avg,OC_Result,")
			_T("OC_OFFECT_X,OC_OFFSET_Y,Center_X,Center_Y,Bank")
			_T("Version,OP_SN\n");

		strData.Format(
			_T("%s,%s,%.1f,%.1f,%s,")
			_T("%.4f,%.4f,")
			_T("%.4f,%.4f,%d,")
			_T("%s,%s\n")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), m_result.dCenterYavg, strResult
			, m_result.m_OffsetX, m_result.m_OffsetY, m_result.m_ptOcCenter.x, m_result.m_ptOcCenter.y
			, m_result.m_Bank
			, strVersion, uts.info.strUserId);
	}

	BOOL ImplOperator::is_otp_data_locked(int module_id)
	{
		time_t time;
		if (uts.otpdb->get_otp_data_lock_time(module_id, &time) < 0)
		{
			return is_otp_programed(module_id);
		}
		return (time > 0 ? TRUE : FALSE);
	}

	BOOL ImplOperator::is_otp_programed(int module_id)
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
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
