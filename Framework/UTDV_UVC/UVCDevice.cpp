#include "StdAfx.h"
#include "UVCDevice.h"
#include "UVCDeviceSettingDlg.h"
#include "UTDV_UVC.h"
#include "DspDummy.h"
#include "DspFullboom.h"
#include "Realtek/RT_5806/DspRealtek.h"
#include "Realtek/RT_5838/DspRealtek.h"
#include "Algorithm.h"

#pragma comment(lib, "UTSAlgorithm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace UTS
{
    UVCDevice *g_pInstance = nullptr;
    UVCDevice* GetUVCDeviceInstance(void)
    {
        if (nullptr == g_pInstance)
        {
            g_pInstance = new UVCDevice;
        }
        return g_pInstance;
    }

    UVCDevice::UVCDevice(void)
    {
        m_pDsp = nullptr;
    }
    
    UVCDevice::~UVCDevice(void)
    {
    }

    BOOL UVCDevice::Initialize(HWND hDisplay)
    {
        m_dLowlevelFPS = m_lowLevelFpsCounter.GetFPS();
        m_dDisplayFPS = m_displayFpsCounter.GetFPS();
        m_hDisplay = hDisplay;

        BOOL bRet = ReadUVCRef();
        if (!bRet)
        {
            uts.log.Error(_T("[%s]ReadUVCRef error!"), UTS_FN);
            return FALSE;
        }

        m_nWidth = m_uvcRef.nResolutionX;
        m_nHeight = m_uvcRef.nResolutionY;
        //------------------------------------------------------------------------------
        // 分配空间
        CreateTestBufferMemory();
        
        // 先尝试停止播放
        m_USBHandler.m_cap_vdo.GrabVideoFrames(FALSE, NULL);
        m_USBHandler.m_cap_vdo.Close();

        return TRUE;
    }

    BOOL UVCDevice::Start(LPCTSTR lpRegister)
    {
        UNREFERENCED_PARAMETER(lpRegister);

        // 如果分辨率变化，重新分配空间
        if (m_nWidth != m_uvcRef.nResolutionX || m_nHeight != m_uvcRef.nResolutionY)
        {
            ReleaseTestBufferMemory();
            m_nWidth = m_uvcRef.nResolutionX;
            m_nHeight = m_uvcRef.nResolutionY;
            CreateTestBufferMemory();
        }
        else
        {
            m_nWidth = m_uvcRef.nResolutionX;
            m_nHeight = m_uvcRef.nResolutionY;
        }

        if (m_pDsp != nullptr)
        {
            RELEASE_POINTER(m_pDsp);
        }
        switch (m_uvcRef.nDspType)
        {
        case 1:
            m_pDsp = new DspFullboom;
            break;

		case 2:
			m_pDsp = new DspRealtek_5806;
			break;

		case 3:
			m_pDsp = new DspRealtek_5838;
			break;

        default:
            m_pDsp = new DspDummy;
            break;
        }

        if (FAILED(CoInitialize(NULL)))
        {
            uts.log.Error(_T("[%s]CoInitialize Failed."), UTS_FN);
            return FALSE;
        }
        // 开始播放
        if (m_USBHandler.m_cap_vdo.Open(0, NULL, m_nWidth, m_nHeight) != S_OK)
		{
			uts.log.Error(_T("UVC Device open fail!"));
			return FALSE;
		}
        m_USBHandler.m_cap_vdo.GrabVideoFrames(TRUE, &m_USBHandler);

        CoUninitialize();


        SetForegroundWindow(uts.info.hMainWnd);

        return TRUE;
    }

    BOOL UVCDevice::Stop()
    {
        //if (FAILED(CoInitialize(NULL)))
        //{
        //    UTS::log.Error(_T("[%s]CoInitialize Failed."), UTS_FN);
        //    return FALSE;
        //}
        m_USBHandler.m_cap_vdo.GrabVideoFrames(FALSE, NULL);
        m_USBHandler.m_cap_vdo.Close();

        //-------------------------------------------------------------------------
        // fix bug #5 产品点不亮但是preview后会出现画面
        ClearTestBufferMemory();
        //-------------------------------------------------------------------------

		if (m_pDsp != nullptr)
		{
			RELEASE_POINTER(m_pDsp);
		}

        //CoUninitialize();

        return TRUE;
    }

    BOOL UVCDevice::Close()
    {
        ReleaseTestBufferMemory();
        return TRUE;
    }

	BOOL UVCDevice::WriteValue(eDeviceWriteValueType eWriteType, void *pBuffer, int nBufferSize)
    {
        assert(nullptr != pBuffer);
        assert(nBufferSize > 0);

        BOOL bRet = TRUE;
		HRESULT hr = S_OK;

        long lValue = 0;
		long Flag = 0;
		long Property;
		long Buffer[2] = {0};
		const int BUFFER_SIZE = 1024;
		TCHAR szBuffer[BUFFER_SIZE] = {0};

		if (m_pDsp == nullptr)
		{
			
			switch (m_uvcRef.nDspType)
			{
			case 1:
				m_pDsp = new DspFullboom;
				break;

			case 2:
				m_pDsp = new DspRealtek_5806;
				break;

			case 3:
				m_pDsp = new DspRealtek_5838;
				break;

			default:
				m_pDsp = new DspDummy;
				break;
			}
		}
		switch (eWriteType)
		{
		case eDeviceWriteValueType::DWVT_WRITE_SENSOR_PROPERTY_BRIGHTNESS:
			if (nBufferSize >= sizeof(long)*2)
			{
				memcpy(Buffer,pBuffer,sizeof(Buffer));
				Property = VideoProcAmp_Brightness;
				if (0 == Buffer[0])
				{
					Flag = VideoProcAmp_Flags_Manual;
				}
				else
				{
					Flag = VideoProcAmp_Flags_Auto;
				}
				lValue = Buffer[1];
				hr = m_USBHandler.m_cap_vdo.SetProcAmpProperty(Property, lValue, Flag);
				if(S_OK != hr) 
					bRet = FALSE;	
			}
			else
			{
				bRet = FALSE;
			}
			break;

		case eDeviceWriteValueType::DWVT_WRITE_SENSOR_PROPERTY_SHARPNESS:
			if (nBufferSize >= sizeof(long)*2)
			{
				memcpy(Buffer,pBuffer,sizeof(Buffer));
				Property = VideoProcAmp_Sharpness;
				if (0 == Buffer[0])
				{
					Flag = VideoProcAmp_Flags_Manual;
				}
				else
				{
					Flag = VideoProcAmp_Flags_Auto;
				}
				lValue = Buffer[1];
				hr = m_USBHandler.m_cap_vdo.SetProcAmpProperty(Property, lValue, Flag);
				if(S_OK != hr) 
					bRet = FALSE;	
			}
			else
			{
				bRet = FALSE;
			}
			break;

		case eDeviceWriteValueType::DWVT_WRITE_SENSOR_PROPERTY_WHITEBALANCE:
			if (nBufferSize >= sizeof(long)*2)
			{
				memcpy(Buffer,pBuffer,sizeof(Buffer));
				Property = VideoProcAmp_WhiteBalance;
				if (0 == Buffer[0])
				{
					Flag = VideoProcAmp_Flags_Manual;
				}
				else
				{
					Flag = VideoProcAmp_Flags_Auto;
				}
				lValue = Buffer[1];
				hr = m_USBHandler.m_cap_vdo.SetProcAmpProperty(Property, lValue, Flag);
				if(S_OK != hr) 
					bRet = FALSE;	
			}
			else
			{
				bRet = FALSE;
			}
			break;

		case eDeviceWriteValueType::DWVT_WRITE_SENSOR_PROPERTY_FOCUS:
			if (nBufferSize >= sizeof(long)*2)
			{
				memcpy(Buffer,pBuffer,sizeof(Buffer));
				Property = CameraControl_Focus;
				if (0 == Buffer[0])
				{
					Flag = VideoProcAmp_Flags_Manual;
				}
				else
				{
					Flag = VideoProcAmp_Flags_Auto;
				}
				lValue = Buffer[1];
				hr = m_USBHandler.m_cap_vdo.SetCameraControlProperty(Property, lValue, Flag);
				if(S_OK != hr) 
					bRet = FALSE;	
			}
			else
			{
				bRet = FALSE;
			}
			break;

		case eDeviceWriteValueType::DWVT_WRITE_SENSOR_PROPERTY_EXPOSURE:
			if (nBufferSize >= sizeof(long)*2)
			{
				memcpy(Buffer,pBuffer,sizeof(Buffer));
				Property = CameraControl_Exposure;
				if (0 == Buffer[0])
				{
					Flag = VideoProcAmp_Flags_Manual;
				}
				else
				{
					Flag = VideoProcAmp_Flags_Auto;
				}
				lValue = Buffer[1];
				hr = m_USBHandler.m_cap_vdo.SetCameraControlProperty(Property, lValue, Flag);
				if(S_OK != hr) 
					bRet = FALSE;	
			}
			else
			{
				bRet = FALSE;
			}
			break;

		case eDeviceWriteValueType::DWVT_REGISTER_SET:
			if (nBufferSize > BUFFER_SIZE)
			{
				nBufferSize = BUFFER_SIZE;
			}
			memcpy(szBuffer, pBuffer, nBufferSize);
	
			bRet = RegisterSet(szBuffer);
			if (bRet)
			{
				uts.log.Info(_T("Register %s"), pBuffer);
			}	
			
			break;

		case eDeviceWriteValueType::DWVT_WRITE_EEPROM:
			if(nBufferSize >= sizeof(BYTE *)+sizeof(int)*2)
			{
				BYTE *Value = NULL;
				int Address = 0;
				int Length = 0;

				BYTE *pTmp = (BYTE *)pBuffer;
				memcpy(Value,pTmp,sizeof(BYTE *));
				pTmp += sizeof(BYTE *);
				memcpy(&Address,pTmp,sizeof(int));
				pTmp += sizeof(int);
				memcpy(&Length,pTmp,sizeof(int));

				hr = m_pDsp->DspWriteEEPROM(Value,Address,Length);
				if(S_OK != hr) 
					bRet = FALSE;			
			}
			else
			{
				bRet = FALSE;
			}			
			break;

		case eDeviceWriteValueType::DWVT_SET_LED:
			if(nBufferSize >= sizeof(int))
			{
				int ledValue = 0;
				memcpy(&ledValue,pBuffer,sizeof(int));

				hr = m_pDsp->DspSetLEDValue(ledValue);
				if(S_OK != hr) 
					bRet = FALSE;			
			}
			else
			{
				bRet = FALSE;
			}			
			break;

		case eDeviceWriteValueType::DWVT_LOADFW_EEPROM:
			hr = m_pDsp->DspLoadFW_EEProm((LPBYTE)pBuffer,nBufferSize);
			if(S_OK != hr) 
				bRet = FALSE;			
			break;

		case eDeviceWriteValueType::DWVT_LOADFW_USBOTP:
			hr = m_pDsp->DspLoadFW_USBOTP((LPBYTE)pBuffer,nBufferSize);
			if(S_OK != hr) 
				bRet = FALSE;			
			break;

		default:
			break;
		}
		if (!bRet)
		{
			uts.log.Error(_T("[%s]WriteValue Error. ReadType = %d, nBufferSize = %d"),
				UTS_FN, (int)eWriteType, nBufferSize);
		}
		return bRet;
	}

	BOOL UVCDevice::ReadValue(eDeviceReadValueType eReadType, void *pBuffer, int nBufferSize)
	{
        assert(nullptr != pBuffer);
        assert(nBufferSize > 0);

        BOOL bRet = TRUE;
		HRESULT hr = S_OK;

        long lValue = 0;
		long Flag = 0;
		long Property;
		long Buffer[2] = {0};
	
		if (m_pDsp == nullptr)
		{

			switch (m_uvcRef.nDspType)
			{
			case 1:
				m_pDsp = new DspFullboom;
				break;

			case 2:
				m_pDsp = new DspRealtek_5806;
				break;

			case 3:
				m_pDsp = new DspRealtek_5838;
				break;

			default:
				m_pDsp = new DspDummy;
				break;
			}
		}

		switch (eReadType)
		{
		case eDeviceReadValueType::DRVT_READ_SENSOR_PROPERTY_BRIGHTNESS:
			Property = VideoProcAmp_Brightness;
			hr = m_USBHandler.m_cap_vdo.GetProcAmpProperty(Property, &lValue, &Flag);
			if(S_OK != hr)
			{
				bRet = FALSE;
			}

			if (Flag == VideoProcAmp_Flags_Manual)
			{
				Buffer[0] = 0;
			}
			else if(Flag == VideoProcAmp_Flags_Auto)
			{
				Buffer[0] = 1;
			}
			else
			{
				bRet = FALSE;
			}

			Buffer[1] = lValue;
			memcpy(pBuffer,Buffer,nBufferSize);
			break;

		case eDeviceReadValueType::DRVT_READ_SENSOR_PROPERTY_SHARPNESS:
			Property = VideoProcAmp_Sharpness;
			hr = m_USBHandler.m_cap_vdo.GetProcAmpProperty(Property, &lValue, &Flag);
			if(S_OK != hr)
			{
				bRet = FALSE;
			}

			if (Flag == VideoProcAmp_Flags_Manual)
			{
				Buffer[0] = 0;
			}
			else if(Flag == VideoProcAmp_Flags_Auto)
			{
				Buffer[0] = 1;
			}
			else
			{
				bRet = FALSE;
			}

			Buffer[1] = lValue;
			memcpy(pBuffer,Buffer,nBufferSize);
			break;

		case eDeviceReadValueType::DRVT_READ_SENSOR_PROPERTY_WHITEBALANCE:
			Property = VideoProcAmp_WhiteBalance;
			hr = m_USBHandler.m_cap_vdo.GetProcAmpProperty(Property, &lValue, &Flag);
			if(S_OK != hr)
			{
				bRet = FALSE;
			}

			if (Flag == VideoProcAmp_Flags_Manual)
			{
				Buffer[0] = 0;
			}
			else if(Flag == VideoProcAmp_Flags_Auto)
			{
				Buffer[0] = 1;
			}
			else
			{
				bRet = FALSE;
			}

			Buffer[1] = lValue;
			memcpy(pBuffer,Buffer,nBufferSize);
			break;

		case eDeviceReadValueType::DRVT_READ_SENSOR_PROPERTY_FOCUS:
			Property = CameraControl_Focus;
			hr = m_USBHandler.m_cap_vdo.GetCameraControlProperty(Property, &lValue, &Flag);
			if(S_OK != hr)
			{
				bRet = FALSE;
			}

			if (Flag == VideoProcAmp_Flags_Manual)
			{
				Buffer[0] = 0;
			}
			else if(Flag == VideoProcAmp_Flags_Auto)
			{
				Buffer[0] = 1;
			}
			else
			{
				Buffer[0] = 0;
			}

			Buffer[1] = lValue;
			memcpy(pBuffer,Buffer,nBufferSize);
			break;

		case eDeviceReadValueType::DRVT_READ_SENSOR_PROPERTY_EXPOSURE:
			Property = CameraControl_Exposure;
			hr = m_USBHandler.m_cap_vdo.GetCameraControlProperty(Property, &lValue, &Flag);
			if(S_OK != hr)
			{
				bRet = FALSE;
			}

			if (Flag == VideoProcAmp_Flags_Manual)
			{
				Buffer[0] = 0;
			}
			else if(Flag == VideoProcAmp_Flags_Auto)
			{
				Buffer[0] = 1;
			}
			else
			{
				bRet = FALSE;
			}

			Buffer[1] = lValue;
			memcpy(pBuffer,Buffer,nBufferSize);
			break;

		case eDeviceReadValueType::DRVT_READ_SENSOR_REGISTER:
			if (nBufferSize >= sizeof(int) * 2)
			{
				int buffer[2] = {0};
				USHORT Value;
				int Address;

				memcpy(buffer, pBuffer, sizeof(buffer));
				Address = buffer[1];

				hr = m_pDsp->DspGetSensorRegister(&Value, Address, m_uvcRef.sensor_i2c_mode);
				if (S_OK != hr)
				{
					bRet = FALSE;
				}
				else
				{
					buffer[0] = Value;
					memcpy(pBuffer, buffer, sizeof(buffer[0]));
				}			
			}
			else
			{
				bRet = FALSE;
			}			
			break;

		case eDeviceReadValueType::DRVT_READ_EEPROM:
			if(nBufferSize >= sizeof(BYTE *)+sizeof(int)*2)
			{
				BYTE *Value = NULL;
				int Address = 0;
				int Length = 0;

				BYTE *pTmp = (BYTE *)pBuffer;
				memcpy(&Value,pTmp,sizeof(BYTE *));
				pTmp += sizeof(BYTE *);
				memcpy(&Address,pTmp,sizeof(int));
				pTmp += sizeof(int);
				memcpy(&Length,pTmp,sizeof(int));

				hr = m_pDsp->DspReadEEPROM(Value,Address,Length);
				if(S_OK != hr) 
					bRet = FALSE;			
			}
			else
			{
				bRet = FALSE;
			}			
			break;

		case eDeviceReadValueType::DRVT_DUMPFW_EEPROM:
			hr = m_pDsp->DspDumpFW_EEProm((LPBYTE)pBuffer,nBufferSize);
			if(S_OK != hr) 
				bRet = FALSE;	
			break;

		case eDeviceReadValueType::DRVT_DUMPFW_USBOTP:
			hr = m_pDsp->DspDumpFW_USBOTP((LPBYTE)pBuffer,nBufferSize);
			if(S_OK != hr) 
				bRet = FALSE;	
			break;

		default:
			break;
		}
		if (!bRet)
		{
			uts.log.Error(_T("[%s]ReadValue Error. ReadType = %d, nBufferSize = %d"),
				UTS_FN, (int)eReadType, nBufferSize);
		}
		return bRet;
	}

    BOOL UVCDevice::GetBufferInfo(BUFFER_INFO &bufferInfo)
    {
        bufferInfo.nWidth = m_nWidth;
        bufferInfo.nHeight = m_nHeight;
        bufferInfo.dwBufferType = 0;
        bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_BMP;
        return TRUE;
    }

    BOOL UVCDevice::Recapture(TEST_BUFFER &testBuffer, int nDummyCount/* = 0*/, int nAvgCount/* = 1*/)
    {
        // 从现在开始数经过的帧数
        m_nFrameCount = 0;
        // 跳过dummy帧
        while (true)
        {
            if (m_nFrameCount >= nDummyCount)
            {
                break;
            }
            else
            {
                Sleep(1);
            }
        }
        // 平均Avg帧
        int nAvgCounted = 0;
        int nBufferSize = m_nWidth * m_nHeight * 3;
        int nLastFrame = m_nFrameCount - 1;
        short* pAvgBmpBuffer = new short[nBufferSize];
        memset(pAvgBmpBuffer, 0, sizeof(short) * nBufferSize);
        do
        {
            if (nLastFrame < m_nFrameCount)
            {
                for (int i = 0; i < nBufferSize; i++)
                {
                    pAvgBmpBuffer[i] += m_TestBuffer.pFrameBuffer[i];
                }
                nLastFrame = m_nFrameCount;
                nAvgCounted++;
            }
            Sleep(1);
        }
        while (nAvgCounted < nAvgCount);

        for (int i = 0; i < nBufferSize; i++)
        {
            pAvgBmpBuffer[i] /= nAvgCount;
            m_TestBuffer.pBmpBuffer[i] = (unsigned char)(pAvgBmpBuffer[i]);
        }
        RELEASE_ARRAY(pAvgBmpBuffer);

        memcpy(&testBuffer, &m_TestBuffer, sizeof(TEST_BUFFER));
        return TRUE;
    }

    void UVCDevice::ShowSettingDlg(void)
    {
        //AFX_MANAGE_STATE(AfxGetStaticModuleState());    // 切换为调用本DLL资源
        HINSTANCE save_hInstance = AfxGetResourceHandle();
        AfxSetResourceHandle(theApp.m_hInstance);

        CUVCDeviceSettingDlg dlg;
        dlg.DoModal();

        AfxSetResourceHandle(save_hInstance);
    }

    void UVCDevice::UpdateCurrentBuffer(unsigned char *pBmpBuffer)
    {
        m_dLowlevelFPS = m_lowLevelFpsCounter.GetFPS();
        memcpy(m_TestBuffer.pFrameBuffer, pBmpBuffer, m_nWidth * m_nHeight * 3);
        m_nFrameCount++;
    }

    BOOL UVCDevice::ReadUVCRef()
    {
        //------------------------------------------------------------------------------
        // 获取param
        CString strSection;
        if (!uts.dbCof.GetDeviceFilename(uts.info.nDeviceId, strSection))
        {
            uts.log.Error(_T("[%s]GetDeviceFilename Fail. nDeviceId = %d"), UTS_FN, uts.info.nDeviceId);
            return FALSE;
        }
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("ResolutionX"), m_uvcRef.nResolutionX, 640, _T("ResolutionX"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("ResolutionY"), m_uvcRef.nResolutionY, 480, _T("ResolutionY"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("DspType"), m_uvcRef.nDspType, 0, _T("0:Dummy, 1:Fullboom , 2:Realtek_5806 3:Realtek_5838"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("sensor_i2c_mode"), m_uvcRef.sensor_i2c_mode, 0, _T("sensor i2c mode 0:8_8 1:8_16 2:16_8 3:16_16"));
        
        return TRUE;
    }

	BOOL UVCDevice::RegisterSet(LPCTSTR lpSectionName)
	{
		CString strSectionName;
		strSectionName.Format(_T("[%s]"), lpSectionName);

		// 取得所有的Register
		vector<REGISTER_ITEM> vecRegister;
		if (!uts.dbCof.GetAllRegister(vecRegister))
		{
			uts.log.Error(_T("GetAllRegister Fail."));
			return FALSE;
		}

		// 找到指定Register的Data
		CString strData;
		for (size_t i = 0; i < vecRegister.size(); i++)
		{
			if (vecRegister[i].strName == strSectionName)
			{
				strData = vecRegister[i].strData;
				break;
			}
		}

		// 执行
		if (!ExecuteCommandList(strData))
		{
			uts.log.Error(_T("ExecuteCommandList Fail."));
			return FALSE;
		}

		return TRUE;
	}

	BOOL UVCDevice::ExecuteCommandList(LPCTSTR lpCommandList)
	{
		BOOL bRet = TRUE;
		CString strCommand;
		CString strCommandList = lpCommandList;
		strCommandList.AppendChar('\n');
		do 
		{
			int nSplitIndex = strCommandList.Find('\n');
			strCommand = strCommandList.Left(nSplitIndex);
			strCommandList = strCommandList.Right(strCommandList.GetLength() - nSplitIndex - 1);
			bRet = ExecuteCommand(strCommand);
			if (!bRet)
			{
				break;
			}
		} while (strCommandList.GetLength() > 0);
		return bRet;
	}

	BOOL UVCDevice::ExecuteCommand(LPCTSTR linebufIn)
	{
		int		tmp_idata = 0;
		int		GPIO_index = 0;   //0 ~ 3 for industrial, 0 ~ 7 for toolkit
		int     GPIO_value = 0;
		TCHAR	tmp_cmd[50] = {0};
		TCHAR	tmp_data[50] = {0};
		DWORD	dwRegAddr   = 0;
		DWORD   dwRegData   = 0;
		int		SleepTime = 10;
		HRESULT hr = S_OK;


		CString strLine = linebufIn;
		strLine.MakeUpper();

		if (strLine.Left(2) == _T("0X"))
		{
			if (_stscanf_s(linebufIn, _T("%x%x"), &dwRegAddr, &dwRegData) != 2)
			{
				uts.log.Error(_T("register format error. [Addr = 0x%x][Data = 0x%x]"), dwRegAddr, dwRegData);
				return FALSE;
			}
			
			hr = m_pDsp->DspSetSensorRegister((USHORT)dwRegData, dwRegAddr, m_uvcRef.sensor_i2c_mode);
			if (S_OK != hr)
			{
				return FALSE;
			}

			Sleep(SleepTime);
		}
		else if (strLine.Left(3) == _T("SLE"))
		{
			if (_stscanf_s(linebufIn, _T("%s%s"), tmp_cmd, _countof(tmp_cmd), tmp_data, _countof(tmp_data)) != 2)
			{
				uts.log.Error(_T("register format error. [Cmd = %s][Data = %s]"), tmp_cmd, tmp_data);
				return FALSE;
			}
			tmp_idata = _ttoi(tmp_data);
			Sleep(tmp_idata);
		}
	

		
	
	
		
		
		

		return TRUE;
	}


    //------------------------------------------------------------------------------
    BaseDevice* GetDevice(void)
    {
        return GetUVCDeviceInstance();
    }
    //------------------------------------------------------------------------------
}
