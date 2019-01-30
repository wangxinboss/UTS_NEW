#include "StdAfx.h"
#include "Algorithm.h"
//#include "BaseOtp.h"
#include "CommonFunc.h"
#include "dlmalloc.h"
#include "UTS.h"
#include <direct.h>
#include "BaseOtp.h"


int BaseOtp::do_crosstalk()
{
	
	uts.log.Debug(_T("Start CrossTalk"));
	int ret= OTPCALI_ERROR_NO;	
	

	WB_PARAM *pwb = &otp_param.wb_param;


	const char *pResult,*pText;
	
	CString tempstr,temp_line;

	USES_CONVERSION;
	using namespace UTS::Algorithm::RI::RI_OpticalCenter;
	using namespace UTS::Algorithm::Image;
	//--------
	
	TCHAR cmd[0x100];
	_stprintf(cmd, _T("%s\\%s\\%s\\"), uts.info.strRootPath, _T("Debug"), _T("OTP_Tool_XTLK"));

	if (_wchdir(cmd) != 0) {
		uts.log.Error(_T("Can not change to path[%s]!"), cmd);
		return SET_ERROR(OTPCALI_ERROR_LSCCALI);
	}

	//OSUtil::ExcuteCommand(_T("cmd.exe /C \"del /F result.txt eeprom_slim.txt\""), TRUE);
// 	remove("eeprom_slim.txt");
// 	remove("result.txt");


	//-------------
	FILE* fp_4pixel_xtlk_setting = fopen("4pixel_xtlk_setting-3p9.xml","rt");
	FILE* fp_Run_3p9xtlk = fopen("Run_3p9xtlk.bat","rt");   
	FILE* fp_Run_3p9corr = fopen("Run_3p9corr.bat","rt");   	
	CStdioFile file;
	
	int nLength,strValue;
	int cnt_line,cnt_reg;
	BYTE pLenCReg[2048];
// 	CString aft[16]={
// 	_T("DIVIDE_ratio_R_0.txt"),_T("DIVIDE_ratio_R_1.txt"),_T("DIVIDE_ratio_R_2.txt"),_T("DIVIDE_ratio_R_3.txt"),
// 	_T("DIVIDE_ratio_Gr_0.txt"),_T("DIVIDE_ratio_Gr_1.txt"),_T("DIVIDE_ratio_Gr_2.txt"),_T("DIVIDE_ratio_Gr_3.txt"),
// 	_T("DIVIDE_ratio_Gb_0.txt"),_T("SDIVIDE_ratio_Gb_1.txt"),_T("DIVIDE_ratio_Gb_2.txt"),_T("DIVIDE_ratio_Gb_3.txt"),
// 	_T("DIVIDE_ratio_B_0.txt"),_T("DIVIDE_ratio_B_1.txt"),_T("DIVIDE_ratio_B_2.txt"),_T("DIVIDE_ratio_B_3.txt")
// 	};
	CString res[16]={
		_T("STATUS_block_B0.txt"),_T("STATUS_block_B1.txt"),_T("STATUS_block_B2.txt"),_T("STATUS_block_B3.txt"),
		_T("STATUS_block_Gr0.txt"),_T("STATUS_block_Gr1.txt"),_T("STATUS_block_Gr2.txt"),_T("STATUS_block_Gr3.txt"),
		_T("STATUS_block_Gb0.txt"),_T("STATUS_block_Gb1.txt"),_T("STATUS_block_Gb2.txt"),_T("STATUS_block_Gb3.txt"),
		_T("STATUS_block_R0.txt"),_T("STATUS_block_R1.txt"),_T("STATUS_block_R2.txt"),_T("STATUS_block_R3.txt")
	};
	//////////////////////////////////////////////////////////////////////////
/*	struct LSC_PARAM *lsc = &otp_param.lsc_param;*/
	dev->GetBufferInfo(m_bufferInfo);
	 
	
	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;	
	}
	 

	dev->DisplayImage(m_bufferObj.pBmpBuffer);

	//Check Light 
	RECT rect;
	RGBTRIPLE rgb;
	WB_DATA_UCHAR wb_tmp;
	SIZE image_size; 

	image_size.cx = m_bufferInfo.nWidth;
	image_size.cy = m_bufferInfo.nHeight;

	ROI roi;
	GetWBROI(&otp_param.wb_param, &image_size, &roi);
	uts.log.Debug(_T("GetWBROI:%d %d"),roi.width,roi.height);

	rect.left = 2434;
	rect.top = 1754;
	rect.right = 2454;
	rect.bottom = 1734;

	UTS::Algorithm::GetROIAvgRGB(m_bufferObj.pBmpBuffer, 
		m_bufferInfo.nWidth, m_bufferInfo.nHeight, 
		0, rect, rgb);

	wb_tmp.B = rgb.rgbtBlue*4;
	wb_tmp.Gb = rgb.rgbtGreen*4;
	wb_tmp.Gr = rgb.rgbtGreen*4;
	wb_tmp.R = rgb.rgbtRed*4;
	//
	if (CheckWBLimit(&wb_tmp) < 0) {
		uts.log.Debug(_T("R,Gr,Gb,B out of spec,R=%d,Gr=%d,Gb=%d,B=%d"),wb_tmp.R,wb_tmp.Gr,wb_tmp.Gb,wb_tmp.B);
		uts.log.Debug(_T("maybe you  should check light source or change auto expouse"));
		ret = OTPCALI_ERROR_WBCALI;
		goto out;
	}
	//
	dev->GetBufferInfo(m_bufferInfo);
	if (!dev->Recapture(m_bufferObj,uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));

		return OTPCALI_ERROR_SENSOR;
	}

	WORD *RAW10Image_input = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
	WORD *RAW10Image_output= new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight*2];
		 

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_input,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	//纗璶矪瞶郎
	if (!uts.imgFile.SaveRawFile(_T("CrossTalk_input"),(BYTE*)RAW10Image_input,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("XTLK: failed to save raw_input file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

 
	 

	//判断有没有这些文件
	if(fp_4pixel_xtlk_setting==NULL)
	{
		uts.log.Error(_T("bayer_setting.xml Open Error."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}

	if(fp_Run_3p9xtlk==NULL)
	{
		uts.log.Error(_T("Run_3p9xtlk.bat Open Error."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}
	if(fp_Run_3p9corr==NULL)
	{
		uts.log.Error(_T("Run_3p9corr.bat Open Error."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}
	fclose(fp_4pixel_xtlk_setting);
	fclose(fp_Run_3p9xtlk);
	fclose(fp_Run_3p9corr);

	//做xtlk
	ShellExecute (NULL,L"open",_T("Run_3p9xtlk.bat"),NULL,NULL,SW_HIDE);
	Sleep(200);
	//lsc/ 
	uts.log.Debug(_T("xtlk 判断"));
	//判断有没有成功
 
	for(int i=0;i<16;i++){
		if (!file.Open(res[i], CFile::modeRead | CFile::typeText ))
		{
			uts.log.Error(_T("open %s fail."),res[i]);
			ret = SENSORDRIVER_ERROR_LSCCALI;
			goto out;
		}
		while(file.ReadString(tempstr))
		 {
			if(tempstr.Find(_T("1"),0)!=-1||
				tempstr.Find(_T("2"),0)!=-1||
				tempstr.Find(_T("3"),0)!=-1)
			{
				uts.log.Error(_T("do cali error "));
				file.Close();
				ret= OTPCALI_ERROR_XTLKCALI;
				goto out;
			}
		 }
		file.Close(); 
	}
	//do correction
	dev->GetBufferInfo(m_bufferInfo);

	if (!dev->Recapture(m_bufferObj, uts.info.nLTDD_DummyFrame,uts.info.nLTDD_AvgFrame))
	{
		uts.log.Error(_T("m_pDevice->Recapture error."));
		ret = OTPCALI_ERROR_SENSOR;
		goto out;	
	}

 

	//WORD *RAW10Image_input = new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];
	//WORD *RAW10Image_output= new WORD[m_bufferInfo.nWidth* m_bufferInfo.nHeight];

	CImageProc::GetInstance().Make10BitMode(m_bufferObj.pFrameBuffer,RAW10Image_output,m_bufferInfo.nWidth, m_bufferInfo.nHeight);

	//纗璶矪瞶郎
	if (!uts.imgFile.SaveRawFile(_T("Correction_input"),(BYTE*)RAW10Image_output,m_bufferInfo.nWidth*2,m_bufferInfo.nHeight))
	{
		uts.log.Error(_T("Correction: failed to save raw_input file."));
		ret = SENSORDRIVER_ERROR_LSCCALI;
		goto out;
	}
	ShellExecute (NULL,L"open",_T("Run_3p9corr.bat"),NULL,NULL,SW_HIDE);
	Sleep(200);
	 //TODO change  res;
	for(int i=0;i<16;i++){
		if (!file.Open(res[i], CFile::modeRead | CFile::typeText ))
		{
			uts.log.Error(_T("open %s fail."),res[i]);
			ret = SENSORDRIVER_ERROR_LSCCALI;
			goto out;
		}
		while(file.ReadString(tempstr))
		 {
			if(tempstr.Find(_T("1"),0)!=-1||
				tempstr.Find(_T("2"),0)!=-1||
				tempstr.Find(_T("3"),0)!=-1)
			{
				uts.log.Error(_T("do xtla error "));
				file.Close();
				ret= OTPCALI_ERROR_XTLKCALI;
				goto out;
			}
		 }
		file.Close(); 
	}
	//
		//将生成的数据写入数据库	
		if ( !file.Open( _T("[eeprom]3P9SP.dat"), CFile::modeRead|CFile::typeBinary))
		{
			uts.log.Error(_T("[eeprom]3P9SP.dat open fail"));
			ret = SENSORDRIVER_ERROR_LSCCALI;
			goto out;
		}
		memset(pLenCReg,0,2048);
 		file.Read(pLenCReg,file.GetLength());
	 	file.Close(); 
		uts.log.Info(_T("get otp_xtlk pass."));
	 //upload_xtlk:
		if(uts.info.nOtpDBType>=0)
		{
			if (otpDB->UpdateOtpByType(mid, DDM::OTPDB_OTPTYPE_CrossTalk,(char *)pLenCReg, 2048) < 0) 
			{
				uts.log.Error(_T("Failed to update XTLK Calibration data to DB!!"));
				ret=OTPCALI_ERROR_DB;
				goto out;
			}
		}	
out:
   // delete *pLenCReg;
	*pLenCReg=NULL;
	
	RELEASE_ARRAY(RAW10Image_input);
	RELEASE_ARRAY(RAW10Image_output);

	return SET_ERROR(ret);
}


 
