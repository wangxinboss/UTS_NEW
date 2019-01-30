#include "StdAfx.h"
#include "ImplOperator.h"
#include "UTOP_Microphone.h"
#include "MMSLib.h"
#include "fftw3.h"
#include "DSPlayer.h"

#define MAX(a,b) a>b?a:b
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

        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dSoundVolume"), m_param.dSoundVolume, 0.1, _T("音量"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nSoundFreq"), m_param.nSoundFreq, 1000, _T("发声频率"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nWaitTime"), m_param.nWaitTime, 200, _T("播放开始后，等待用时(ms)"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("nBypassTime"), m_param.nBypassTime, 200, _T("录音前，忽略用时(ms)"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dPeakValueSpec"), m_param.dPeakValueSpec, 20000.0, _T("频率中的Peak，要大于某值"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dPeakPositionSpec"), m_param.dPeakPositionSpec, 50.0, _T("Peak的位置范围（例10为±10）"));
        uts.dbCof.GetOperatorSingleSpec(strSection, _T("dSNRSpec"), m_param.dSNRSpec, 50.0, _T("信噪比要大于某值"));
        
        return TRUE;
    }

    BOOL ImplOperator::OnTest(BOOL *pbIsRunning, int *pnErrorCode)
    {
        //------------------------------------------------------------------------------
        // 初始化
        m_TimeCounter.SetStartTime();

        //------------------------------------------------------------------------------
        // 初始化结果
        m_result.dMaxPeakValue = 0.0;
        m_result.dMaxPeakPosition = 0.0;
        m_result.dSNR = 0.0;
        
        short *pSoundBuffer = nullptr;
        double *pdSoundChannelBuffer = nullptr;
        double *pFFTResult = nullptr;

        int	iBufferSize	= 44100;
        uts.log.Debug(_T("Sound Freq = %d"), m_param.nSoundFreq);

        //-------------------------------------------------------------------------
        ISoundFormat m_Format;

        m_Format.NbBitsPerSample    = 16;
        m_Format.NbChannels         = 1;
        m_Format.SamplingRate       = 44100;

        pSoundBuffer = new short[iBufferSize];
        CreateBufferByFreq(m_param.nSoundFreq, m_Format.SamplingRate, iBufferSize, pSoundBuffer);

        //system("del SoundFile.csv");
        //std::string TextString;
        //int iCount = m_Format.SamplingRate/512;
        //for (int i=0;i < 512;i = i++)
        //{	
        //    int Index = i*iCount;
        //    TextString += IntegerToString(pSoundBuffer[Index]);
        //    TextString += ",";
        //    TextString += "\n";
        //}
        //WriteCSVFile(L"SoundFile.csv",TextString);
        DSPlayer_Init(uts.info.hMainWnd, iBufferSize, m_Format);
        DSPlayer_UpdateBuffer(pSoundBuffer, 0, iBufferSize);
        DSPlayer_SetVolume(m_param.dSoundVolume);
        DSPlayer_Play(1);

        Sleep(m_param.nWaitTime);

        //-------------------------------------------------------------------------
        int iDev = 0;
        int nChannels = 1;			// Same as m_Format.NbChannels
        int nBitsPerSample = 16;	// Same as m_Format.NbBitsPerSample
        const static int LogSampleSize = 2048;;
		static const int samplesPerBuf = LogSampleSize; 
        int samplesPerSec = 44100;	// m_Format.SamplingRate
        int nTapSize = samplesPerBuf;
        //-------------------------------------------------------------------------

		pdSoundChannelBuffer = new double[samplesPerBuf];
        if (SoundInit(NULL, iDev, nChannels, nBitsPerSample, samplesPerBuf, samplesPerSec, nTapSize) == S_OK)
        {
            unsigned int nBufferSize = 0;
            SoundGetSampleBuffer(NULL, &nBufferSize);

            // ByPass X sec
            for (int i = 0; i < 10; i++)
            {		
                // Recording
                SoundGetChannelBuffer(0, pdSoundChannelBuffer, NULL);
                Sleep(m_param.nBypassTime / 10);
            }
            // Recording
            SoundGetChannelBuffer(0, pdSoundChannelBuffer, NULL);

			DSPlayer_Stop();
            // Analysis
            //---------------------------------------------------------------------


			double Max = 0;
			double Min = 65535;

			for (int i=0;i<LogSampleSize;i++)
				 {
					 Max = (Max < pdSoundChannelBuffer[i] )?(pdSoundChannelBuffer[i]):(Max);
					 Min = (Min > pdSoundChannelBuffer[i] )?(pdSoundChannelBuffer[i]):(Min);
				 }

			pFFTResult = new double[samplesPerBuf];
            fftw_complex in[samplesPerBuf], out[samplesPerBuf];
            fftw_plan p;

			p = fftw_plan_dft_1d(LogSampleSize, in, out, FFTW_FORWARD, FFTW_MEASURE );		

			for (int i = 0; i < LogSampleSize; i++)
			{
				in[i][0] = (pdSoundChannelBuffer[i]-Min)/(Max-Min)-0.5;
				in[i][1] = 0.0;					 
			}
			fftw_execute(p);
			fftw_destroy_plan(p);
			//------------------------------------------------------------
			double* pR_FFTResult = new double[LogSampleSize];
			for (int i = 0; i < LogSampleSize; i++)
			{
				pR_FFTResult[i] = sqrt(pow((float)(out[i][0]),2)+pow((float)(out[i][1]),2));
			}

			//最終結果?出
				 
				 // Find the Max Peak
				 double MaxPeak[2] = {0};	
				 int iFCount = (LogSampleSize / 2);	
				 for (int i = 1; i < iFCount; i++)
				 {
					 if (MaxPeak[0] < pR_FFTResult[i])
					 {
						 MaxPeak[0] = pR_FFTResult[i];
						 MaxPeak[1] = i;
					 }
				 }

				 double MaxPeak2[2] = {0};	
				 //double iFCount = (LogSampleSize / 2);	
				 for (int i = iFCount; i < LogSampleSize; i++)
				 {
					 if (MaxPeak2[0] < pR_FFTResult[i])
					 {
						 MaxPeak2[0] = pR_FFTResult[i];
						 MaxPeak2[1] = LogSampleSize-i;
					 }
				 }
				 wchar_t tempChar[MAX_PATH] = {0};
				 double Result_Frequence = (MaxPeak[1]+MaxPeak2[1])/2/LogSampleSize*iBufferSize;

				 double RecordMaxStdev = 0;
				 double RecordMinStdev = 0;
				 double RecordAmpStdev = 0;
				 double AvgRecordAmplitude =  VerifyAmplitude(pdSoundChannelBuffer,LogSampleSize,double(Result_Frequence)/double(iBufferSize),RecordMaxStdev,RecordMinStdev,RecordAmpStdev);


				 double NoiseRate = RecordAmpStdev/((AvgRecordAmplitude==0)?(0.0001):(AvgRecordAmplitude));
				 NoiseRate = -20*log(NoiseRate);

				 
            //---------------------------------------------------------------------
            m_result.dMaxPeakValue = AvgRecordAmplitude;
            m_result.dMaxPeakPosition = Result_Frequence;
            m_result.dSNR = NoiseRate;
            uts.log.Debug(_T("MaxPeak Value = %f"), m_result.dMaxPeakValue);
            uts.log.Debug(_T("MaxPeak Position = %f"), m_result.dMaxPeakPosition);
            uts.log.Debug(_T("SNR = %f"), m_result.dSNR);
			
            //------------------------------------------------------------------------------
            // 判断规格
            if (m_result.dMaxPeakValue <= m_param.dPeakValueSpec
                || abs(m_result.dMaxPeakPosition - m_param.nSoundFreq) >= m_param.dPeakPositionSpec
                || m_result.dSNR <= m_param.dSNRSpec)
            {
                *pnErrorCode = uts.errorcode.E_MIC;
            }
            else
            {
                *pnErrorCode = uts.errorcode.E_Pass;
            }

            // Save Record data
            SaveRecordFile(_T("RecordFile"), nBufferSize, pdSoundChannelBuffer, pFFTResult);

            SoundRelease();
        }
        else
        {
            uts.log.Error(_T("SoundInit Error."));
            *pnErrorCode = uts.errorcode.E_MIC;
            goto end;
        }

end:
        //-------------------------------------------------------------------------
        
        RELEASE_ARRAY(pSoundBuffer);
        RELEASE_ARRAY(pdSoundChannelBuffer);
        RELEASE_ARRAY(pFFTResult);
        //-------------------------------------------------------------------------

        // 根据Errorcode设置结果
        m_bResult = (*pnErrorCode == uts.errorcode.E_Pass);

        //-------------------------------------------------------------------------
        // 保存数据文件
        SaveData();

        return m_bResult;
    }

    void ImplOperator::OnGetErrorReturnValueList(vector<int> &vecReturnValue)
    {
        vecReturnValue.clear();
        vecReturnValue.push_back(uts.errorcode.E_MIC);
    }

    void ImplOperator::GetDataContent(LPCTSTR lpTime, CString &strHeader, CString &strData, CString &strSFCFilter)
    {
        CString strVersion;
        UTS::OSUtil::GetFileVersion(m_strModuleFile, strVersion);
        CString strResult = (m_bResult ? PASS_STR : FAIL_STR);

        strHeader = _T("Time,SN,TestTime(ms),MIC_Result,")
            _T("MaxPeakValue,MaxPeakPosition,SNR,")
            _T("Version,OP_SN\n");

        strData.Format(
            _T("%s,%s,%.1f,%s,")
            _T("%.1f,%.1f,%.1f,")
            _T("%s,%s\n")
            , lpTime, uts.info.strSN, m_TimeCounter.GetPassTime(), strResult
            , m_result.dMaxPeakValue, m_result.dMaxPeakPosition, m_result.dSNR
            , strVersion, uts.info.strUserId);
    }

    BOOL ImplOperator::SaveRecordFile(
        LPCTSTR lpFileName,
        int nBufferSize,
        const double *pdSoundChannelBuffer,
        const double *pFFTResult)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        //-------------------------------------------------------------------------
        // 输出Data file
        CString strDirPath;
        strDirPath.Format(_T("%s\\Data-%s-%s-%s-%04d%02d%02d\\"),
            uts.info.strResultPath,
            uts.info.strProjectName,
            uts.info.strLineName,
            uts.info.strStationName,
            st.wYear,
            st.wMonth, 
            st.wDay);
        UTS::OSUtil::CreateMultipleDirectory(strDirPath);
        CString strFilePath;
        strFilePath.Format(_T("%s\\%s-%04d%02d%02d.csv"),
            strDirPath,
            lpFileName,
            st.wYear,
            st.wMonth, 
            st.wDay);

        FILE* fp = NULL;
        long fileLength ;
        errno_t etRet = _tfopen_s(&fp, strFilePath, _T("w"));
        if (0 != etRet)
        {
            uts.log.Error(_T("Open file Fail. path = %s"), strFilePath);
            return FALSE;
        }
        fseek(fp, 0, SEEK_END);
        fileLength = ftell(fp);

        for (int i = 0; i < nBufferSize; i++)
        {
            _ftprintf_s(fp, _T("%f,%f\n"), pdSoundChannelBuffer[i], pFFTResult[i]);
        }

        fclose(fp);
        return TRUE;
    }

    void ImplOperator::CreateBufferByFreq(int iFrequence, int iSamplesPerSec, int iBufLen, short* psData)
{
	memset(psData, 0, sizeof(short) * iBufLen);

	int		iExpression	= 0;
	double		iT			= (int)((double)iSamplesPerSec / iFrequence);
	double	dVal		= 0.0;

	for (int i = 0; i < iBufLen; i++)
	{
		dVal      = DOUBLE(i) * 2.0 * PI / iT;
		psData[i] = (short)((pow(2.0,15)-1)* sin(dVal));
	}
}

double ImplOperator::VerifyAmplitude(double* pData,int iBufLen,double dFrequence,double &MaxStdev,double &MinStdev,double &AmpStdev)
{
	double AverageAmplitude;
	MaxStdev = 0;
	MinStdev = 0;
	double Max = 0;;
	double Min = 65535;

	int CycleSize = int(iBufLen*dFrequence);
	if(CycleSize!=0)
	{
		int CycleLen = int(double(iBufLen)/double(CycleSize))+1;

		double* MaxArray = new double[CycleSize];
		memset(MaxArray,0,sizeof(double)*CycleSize);

		double* MinArray = new double[CycleSize];
		memset(MinArray,0,sizeof(double)*CycleSize);

		double* AmpArray = new double[CycleSize];
		memset(AmpArray,0,sizeof(double)*CycleSize);

		int count = 0;
		for (int i=0;i<iBufLen;i+=CycleLen)
		{
			Max = 0;
			Min = 65536;
			for (int j=i;(j<(i+CycleLen))&&(j<iBufLen);j++)
			{
				Max = (Max < pData[j] )?(pData[j]):(Max);
				Min = (Min > pData[j] )?(pData[j]):(Min);
			}
			MaxArray[count] = Max;
			MinArray[count] = Min;
			AmpArray[count] = Max-Min;
			//DEBUGINFO(L"Max = %.5f ; Min = %.5f",Max,Min);
			count++;
		}

		//DEBUGINFO(L"Count = %d;",count);
		double AvgMax = 0;
		double AvgMin = 0;
		for (int i=0;i<count;i++)
		{
			AvgMax += MaxArray[i];
			AvgMin += MinArray[i];
		}
		AvgMax /= double(count);
		AvgMin /= double(count);

		AverageAmplitude = AvgMax-AvgMin;
		/*double*/ AmpStdev = 0;

		if (count>2)
		{
			MaxStdev =  UTS::Algorithm::Math::StandardDeviation(MaxArray, count);
			MinStdev =  UTS::Algorithm::Math::StandardDeviation(MinArray, count);
			AmpStdev =  UTS::Algorithm::Math::StandardDeviation(AmpArray, count);
		}
		else
		{
			MaxStdev = abs(AvgMax);
			MinStdev = abs(AvgMin);
		}

		RELEASE_ARRAY(MaxArray);
		RELEASE_ARRAY(MinArray);
		RELEASE_ARRAY(AmpArray);
	}

	return AverageAmplitude;
}
double ImplOperator::VerifyAmplitude(short* pData,int iBufLen,double dFrequence,double &MaxStdev,double &MinStdev,double &AmpStdev)
{
	double* tmpData = new double[iBufLen];
	for (int i=0;i<iBufLen;i++)
	{
		tmpData[i] = pData[i];
	}
	double AvgAmplitude = VerifyAmplitude(tmpData,iBufLen,dFrequence,MaxStdev,MinStdev,AmpStdev);


	RELEASE_ARRAY(tmpData);

	return AvgAmplitude;
}



    //------------------------------------------------------------------------------
    BaseOperator* GetOperator(void)
    {
        return (new ImplOperator);
    }
    //------------------------------------------------------------------------------
}
