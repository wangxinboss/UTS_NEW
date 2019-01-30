#include "StdAfx.h"
#include "BlemishCircleOperator.h"
#include "UTOP_BlemishCircle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int imageAverage(byte *YBuffer,int inWidth,int inHigh,int averageSize)
{
	int bigWidth = inWidth+averageSize*2;
	int bigHigh = inHigh+averageSize*2;
	byte *bigBuf = (byte *)malloc(bigWidth*bigHigh);
	memset(bigBuf,0,bigWidth*bigHigh);
	int i,j,m,n;
	double sum = 0;

	//copy center buffer
	for(i=0; i<inHigh; i++)
	{
		for(j=0; j<inWidth; j++)
		{
			bigBuf[(i+averageSize)*bigWidth+j+averageSize] = YBuffer[i*inWidth+j];
		}
	}

	//copy up buffer
	for(i=0; i<averageSize; i++)
	{
		for(j=0; j<bigWidth; j++)
		{
			bigBuf[i*bigWidth+j] = bigBuf[(averageSize*2-i)*bigWidth+j];
		}
	}

	//copy down buffer
	for(i=averageSize+inHigh; i<bigHigh; i++)
	{
		for(j=0; j<bigWidth; j++)
		{
			bigBuf[i*bigWidth+j] = bigBuf[(i-averageSize)*bigWidth+j];
		}
	}

	//copy left buffer
	for(i=0; i<bigHigh; i++)
	{
		for(j=0; j<averageSize; j++)
		{
			bigBuf[i*bigWidth+j] = bigBuf[(i*bigWidth)+averageSize*2-j];
		}
	}


	//copy Right buffer
	for(i=0; i<bigHigh; i++)
	{
		for(j=averageSize+inWidth; j<bigWidth; j++)
		{
			bigBuf[i*bigWidth+j] = bigBuf[(i*bigWidth)+j-averageSize];
		}
	}

	//average Y buffer
	for(i=0; i<inHigh; i++)
	{
		for(j=0; j<inWidth; j++)
		{
			sum = 0;
			for(m=0; m<=averageSize*2; m++)
			{
				for(n=0; n<=averageSize*2; n++)
				{
						sum += bigBuf[(i+m)*bigWidth+j+n];
				}
			}
			YBuffer[i*inWidth+j] = (byte)(sum/((averageSize*2+1)*(averageSize*2+1)));
		}
	}

	free(bigBuf);
	bigBuf = NULL;

	return 0;
}

int imageRgbAverage(byte *Buffer,int inWidth,int inHigh,int averageSize)
{
	int i,j;
	byte *BBuf = (byte *)malloc(inWidth*inHigh);
	byte *GBuf = (byte *)malloc(inWidth*inHigh);
	byte *RBuf = (byte *)malloc(inWidth*inHigh);

	for(i=0; i<inHigh; i++)
	{
		for(j=0; j<inWidth; j++)
		{
			BBuf[i*inWidth+j] = Buffer[i*3*inWidth+j*3];
			GBuf[i*inWidth+j] = Buffer[i*3*inWidth+j*3+1];
			RBuf[i*inWidth+j] = Buffer[i*3*inWidth+j*3+2];		
		}
	}
	imageAverage(BBuf,inWidth,inHigh,averageSize);
	imageAverage(GBuf,inWidth,inHigh,averageSize);
	imageAverage(RBuf,inWidth,inHigh,averageSize);


	for(i=0; i<inHigh; i++)
	{
		for(j=0; j<inWidth; j++)
		{
			Buffer[i*3*inWidth+j*3] = BBuf[i*inWidth+j];
			Buffer[i*3*inWidth+j*3+1] = GBuf[i*inWidth+j];
			Buffer[i*3*inWidth+j*3+2] = RBuf[i*inWidth+j];		
		}
	}

	free(RBuf);
	free(GBuf);
	free(BBuf);
	RBuf = NULL;
	GBuf = NULL;
	BBuf = NULL;

	return 0;
}

namespace UTS
{
    BlemishCircleOperator::BlemishCircleOperator(void)
    {
        OPERATOR_INIT;
    }

    BlemishCircleOperator::~BlemishCircleOperator(void)
    {
    }

    BOOL BlemishCircleOperator::OnReadSpec()
    {
        CString strSection = OSUtil::GetFileName(m_strModuleFile);
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMinY"), m_param.dLTMinY, 100.0, _T("Min Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dLTMaxY"), m_param.dLTMaxY, 140.0, _T("Max Y value"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nReCapture"), m_param.nReCapture, 1, _T("0: Do nothing / 1: Set register, capture image, save image"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nShowROI"), m_param.nShowROI, 0, _T("0: Disable  1: Enable"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("blemishParam.nAngleStep"), m_param.blemishParam.nAngleStep, 3, _T("angle step"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("blemishParam.nRadiusStep"), m_param.blemishParam.nRadiusStep, 1, _T("radius step"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("blemishParam.nMoveStep"), m_param.blemishParam.nMoveStep, 5, _T("move step"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("blemishParam.nMaxRadius"), m_param.blemishParam.nMaxRadius, 75, _T("max blemish radius"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("blemishParam.nMinRadius"), m_param.blemishParam.nMinRadius, 20, _T("min blemish radius"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("blemishParam.dThreshold"), m_param.blemishParam.dThreshold, 2.10, _T("blemish circle threshold"));
		uts.dbCof.GetOperatorSingleSpec(strSection, _T("blemishParam.naverageNum"), m_param.naverageNum, 0, _T("average pixel number"));

        return TRUE;
    }

    BOOL BlemishCircleOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();
        m_pDevice->GetBufferInfo(m_bufferInfo);

        //------------------------------------------------------------------------------
        // 初始化结果
        m_dYvalue = 0.0;
        m_result.clear();

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
		if(m_param.naverageNum > 0 && m_param.naverageNum < 100)
		{
			imageRgbAverage(m_bufferObj.pBmpBuffer,
							m_bufferInfo.nWidth,
							m_bufferInfo.nHeight,
							m_param.naverageNum);
		}

        // 画图
        m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer);
        
        // 获取画面平均亮度
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_YUV24) != 0)
        {
            UTS::Algorithm::Image::CImageProc::GetInstance().GetYUV24_YBuffer(
                m_bufferObj.pYuv24Buffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight,
                m_bufferObj.pYBuffer);

            // 画面亮度
            RECT rcWholeImage = {0, 0, m_bufferInfo.nWidth, m_bufferInfo.nHeight};
            UTS::Algorithm::GetBlockAvgY(
                m_bufferObj.pYBuffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight,
                rcWholeImage,
                m_dYvalue);
        }
        else
        {
            UTS::Algorithm::CalYavg(m_bufferObj.pBmpBuffer, m_bufferInfo.nWidth, m_bufferInfo.nHeight, m_dYvalue);
        }
        //------------------------------------------------------------------------------
        // 判断画面平均亮度
        if (m_dYvalue < m_param.dLTMinY || m_dYvalue > m_param.dLTMaxY)
        {
            *pnErrorCode = uts.errorcode.E_Linumance;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 测试
        if ((m_bufferInfo.dwBufferType & BUFFER_TYPE_MASK_BMP) != 0)
        {
            // 算Blemish
            Blemish_Circle::CircleBlemish_RGB(
                m_bufferObj.pBmpBuffer,
                m_bufferInfo.nWidth,
                m_bufferInfo.nHeight,
                m_param.blemishParam,
                m_result);
        }
        else
        {
            uts.log.Error(_T("Operator %s do not support buffer type %d."), m_strOperatorName, m_bufferInfo.dwBufferType);
            *pnErrorCode = uts.errorcode.E_NoImage;
            goto end;
        }

        //------------------------------------------------------------------------------
        // 判断规格
        if (m_result.size() > 0)
        {
            *pnErrorCode = uts.errorcode.E_Blemish;

            if (m_param.nShowROI != 0)
            {
                // 画图
                UI_MARK uiMark;
                UI_CIRCLE uiCircle;
                uiCircle.color = COLOR_RED;
                for (size_t i = 0; i < m_result.size(); i++)
                {
                    uiCircle.ptCenter = CPoint(m_result[i].x, m_bufferInfo.nHeight - m_result[i].y - 1);
                    uiCircle.dRadius = m_result[i].r;
                    uiMark.vecUiCircle.push_back(uiCircle);
                }
                m_pDevice->DisplayImage(m_bufferObj.pBmpBuffer, &uiMark);

                uts.board.ShowMsg(_T("Continue"));
                uts.keyboard.BeginListen();
                if (uts.info.nWaitKeyType == 0)
                {
                    uts.keyboard.WaitSpace(pbIsRunning);
                }
                else
                {
                    uts.keyboard.WaitReturn(pbIsRunning);
                }
                uts.keyboard.EndListen();
                //*pnErrorCode = uts.errorcode.E_Pass;
                uts.board.ShowMsg(EMPTY_STR);
            }

        }
        else
        {
            *pnErrorCode = uts.errorcode.E_Pass;
        }

end:
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

    void BlemishCircleOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_Fail);
        vecReturnValue.push_back(uts.errorcode.E_NoImage);
        vecReturnValue.push_back(uts.errorcode.E_Linumance);
        vecReturnValue.push_back(uts.errorcode.E_Blemish);
    }

    void BlemishCircleOperator::OnGetRegisterList(vector<CString> &vecRegister)
    {
        vecRegister.clear();
        vecRegister.push_back(m_strOperatorName);
    }

    void BlemishCircleOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,SensorID,TestTime(ms),Y_Avg,BLM_Result,")
            _T("BLM_Count,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%s,%.1f,%.1f,%s,")
            _T("%d,")
            _T("%s,%s\n")
            , lpTime
            , uts.info.strSN
            , uts.info.strSensorId
            , m_TimeCounter.GetPassTime()
            , m_dYvalue
            , strResult
            , m_result.size()
            , strVersion, uts.info.strUserId);

		if (uts.info.nMesEn!=0)
		{
			CString  str;  
			str.Format(_T("POG_YValue=%.1f,POG_Result=%s,POG_Size=%d"),m_dYvalue, strResult, m_result.size());
			uts.log.Debug((LPCTSTR)str);
			uts.info.strMESInsData += str;
		}
    }

    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new BlemishCircleOperator);
    }
    //------------------------------------------------------------------------------
}
