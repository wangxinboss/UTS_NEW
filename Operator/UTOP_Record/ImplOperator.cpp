#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_Record.h"

#include <cstdio>
#include <../../3rdparty/opencv245/include/opencv2/opencv.hpp>
using namespace cv;



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
      
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("strFileName"), m_param.strFileName, _T("D:\\VideoTest.avi"), _T("File Name"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("nFrameRate"), m_param.nFrameRate, 10, _T("Frame Rate"));

        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // ÇÐ»»SensorÐòÁÐ 
        BUFFER_INFO bfInfo;
        m_pDevice->GetBufferInfo(bfInfo);
	
		VideoWriter writer;
		Size videoSize = Size(bfInfo.nWidth,bfInfo.nHeight);
		
		USES_CONVERSION;

		writer.open(T2A(m_param.strFileName), -1 , m_param.nFrameRate, videoSize);
		Mat image2(bfInfo.nHeight,bfInfo.nWidth,CV_8UC3, Scalar(0));
        
        uts.keyboard.BeginListen();
        BOOL bKeyPress = FALSE;
        do
        {
			if (!m_pDevice->Recapture(
				m_bufferObj,
				uts.info.nLTDD_DummyFrame,
				uts.info.nLTDD_AvgFrame))
            {
				uts.log.Error(L"Recapture error!");
				continue;
            }
            //------------------------------------------------------------------------------
			for(int y = 0; y< bfInfo.nHeight ; y++)
			{
				for(int x = 0; x<bfInfo.nWidth ; x++)
				{
					image2.at<cv::Vec3b>(bfInfo.nHeight - 1- y,x)[0] = m_bufferObj.pBmpBuffer[y*bfInfo.nWidth*3 + x*3 + 0];
					image2.at<cv::Vec3b>(bfInfo.nHeight - 1- y,x)[1] = m_bufferObj.pBmpBuffer[y*bfInfo.nWidth*3 + x*3 + 1];
					image2.at<cv::Vec3b>(bfInfo.nHeight - 1- y,x)[2] = m_bufferObj.pBmpBuffer[y*bfInfo.nWidth*3 + x*3 + 2];
				}
			}

			writer.write(image2);


            // ÆÁÄ»»æÖÆ£¬with UI Mark
            m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);
            if (uts.info.nWaitKeyType == 0)
            {
                bKeyPress = uts.keyboard.IsSpacePress();
            }
            else
            {
                bKeyPress = uts.keyboard.IsReturnPress();
            }
        } while (*pbIsRunning && !bKeyPress);
        uts.keyboard.EndListen();

      
        *pnErrorCode = uts.errorcode.E_Pass;
        uts.board.ShowMsg(EMPTY_STR);
        return TRUE;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
    }

    void ImplOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
