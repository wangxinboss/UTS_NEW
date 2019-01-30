#include "StdAfx.h"

#include "ImplOperator.h"

#include "UTOP_RelShift_MTK.h"
#include "Algorithm.h"
#include "mtk_stereo_verify.h"

#pragma comment(lib, "mtk_stereo_verify.lib")
#pragma comment(lib, "UTSAlgorithm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
	void ChangeHLToLH(unsigned char *cRAW_imageBuffer,unsigned short *sRAW_imageBuffer,int width,int height)
	{
		for (int j = 0 ;j < height ; j++)
		{
			for (int i = 0 ;i< width ; i++)
			{
				sRAW_imageBuffer [j*width + i] = cRAW_imageBuffer [2*(j*width + i)] + (cRAW_imageBuffer [2*(j*width + i) + 1] << 8);
			}
		}

	}


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

 		uts.dbCof.GetOperatorSingleSpec(strSection, _T("ResolutionX"), m_param.nResolutionX, 4208, _T("ResolutionX"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("ResolutionY"), m_param.nResolutionY, 3120, _T("ResolutionY"));
	
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
		// 抓图
		//------------------------------------------------------------------------------
		// 弹出选择单个文件的对话框
		unsigned short **RAW_imageBuffer = new unsigned short*[4];
		for ( int i = 0; i < 4; i++ )
		{
			RAW_imageBuffer[i] = new unsigned short[m_param.nResolutionX * m_param.nResolutionY];
			memset(RAW_imageBuffer[i],0,sizeof(unsigned short)*m_param.nResolutionX * m_param.nResolutionY);
		}

		TEST_BUFFER TestBuffer={0};

		TestBuffer.pFrameBuffer = new unsigned char[m_param.nResolutionX * m_param.nResolutionY*2];
		TestBuffer.pBmpBuffer = new unsigned char[m_param.nResolutionX * m_param.nResolutionY*3];
		TestBuffer.pRaw8Buffer = new unsigned char[m_param.nResolutionX * m_param.nResolutionY];

		CString strFileToHandle;
		TCHAR szFileName[MAX_PATH] = {0};
		OPENFILENAME SFN;
		CString filename;

		memset(&SFN, 0, sizeof(OPENFILENAME));
		SFN.lStructSize = sizeof(OPENFILENAME);
		SFN.hwndOwner = uts.info.hMainWnd;
		SFN.lpstrFilter = _T("Image File(*.bmp;*.raw;*.yuvbmp)\0*.bmp;*.raw;*.yuvbmp\0\0");
		SFN.lpstrFile = szFileName;
		SFN.nMaxFile = MAX_PATH;
		SFN.Flags = OFN_FILEMUSTEXIST;

		for (int i = 0 ; i < 4 ;i++)
		{
			if (!GetOpenFileName(&SFN))
			{
				uts.log.Error(_T("Did not select file."));

				*pnErrorCode = uts.errorcode.E_NoImage;
				goto end;
			}
			strFileToHandle = szFileName;

			// 读取图片
			if (!uts.imgFile.LoadRawFile(
				strFileToHandle,
				m_param.nResolutionX*2,
				m_param.nResolutionY,
				TestBuffer.pFrameBuffer))
			{
				uts.log.Error(_T("LoadRawFile Error. file = %s"), strFileToHandle);
				*pnErrorCode = uts.errorcode.E_NoImage;
				goto end;
			}

			//memcpy(RAW_imageBuffer[i],TestBuffer.pFrameBuffer,
			//sizeof(unsigned short)*m_param.nResolutionX * m_param.nResolutionY);

			ChangeHLToLH(TestBuffer.pFrameBuffer,RAW_imageBuffer[i],m_param.nResolutionX , m_param.nResolutionY);
		}


		RELEASE_ARRAY(TestBuffer.pFrameBuffer);
		RELEASE_ARRAY(TestBuffer.pBmpBuffer);
		//------------------------------------------------------------------------------
// 		for (int i = 0 ; i < 4 ;i++)
// 		{
// 			filename.Format(_T("test_%d"),i);
// 
// 			if (!uts.imgFile.SaveRawFile(filename,(BYTE*)RAW_imageBuffer[i],m_param.nResolutionX*2,m_param.nResolutionY))
// 			{
// 				*pnErrorCode = uts.errorcode.E_Fail;
// 			}
// 		}

		// 测试
		int res = -1;
		char ConfigName[] = "D:\\MTK_Primax_MT6797_13M_13M.CFG";

		//int *gVerify_Items  = new int[180];
		memset(m_param.Verify_Items,0,sizeof(int)*180);
		
		//int Verify_Items[19]={0};

		res = IQC(m_param.Verify_Items, 
			RAW_imageBuffer[0], 
			RAW_imageBuffer[1], 
			m_param.nResolutionX,
			m_param.nResolutionY, 
			RAW_imageBuffer[2], 
			RAW_imageBuffer[3], 
			m_param.nResolutionX,
			m_param.nResolutionY, 
			ConfigName);
		
		uts.log.Debug(_T("Res : %d"),res);

		for ( int i = 0; i < 19; i++ )
		uts.log.Debug(_T("Item%d : %d"),i,m_param.Verify_Items[i]);

		//------------------------------------------------------------------------------
		// 判断规格
		if ( res != 1)
		{
			*pnErrorCode = uts.errorcode.E_RelativeShift;
		}
end:
		// 根据Errorcode设置结果
		m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

		for ( int i = 0; i < 4; i++ )
		RELEASE_ARRAY(RAW_imageBuffer [i]);


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
		vecReturnValue.push_back(uts.errorcode.E_RelativeShift);
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

		strHeader = _T("Time,SN,TestTime(ms),Result,");
		strData.Format(_T("%s,%s,%.1f,%s,")
			, lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult);

		for ( int i = 0; i < 19; i++ )
		{
			strHeader.AppendFormat(_T("Item_%d,"),i);
			strData.AppendFormat(_T("%d,"), m_param.Verify_Items[i]);
		}

		strHeader.Append(_T("Version,OP_SN\n"));
		strData.AppendFormat(_T("%s,%s\n"), strVersion, uts.info.strUserId);
	}

	//------------------------------------------------------------------------------
	BaseOperator* GetOperator(void)
	{
		return (new ImplOperator);
	}
	//------------------------------------------------------------------------------
}
