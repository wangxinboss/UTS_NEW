#include "StdAfx.h"
#include "V5UDevice.h"
#include "Algorithm.h"
#include "V5UDeviceSettingDlg.h"
#include "UTDV_V5U.h"

#pragma comment(lib, "UTSAlgorithm.lib")

using namespace UTS::Algorithm::Image;

namespace UTS
{
    V5UDevice::V5UDevice(void)
    {
        for (int i = 0; i < ARRAY_SIZE(m_bDvStarting); i++)
        {
            m_bDvStarting[i] = FALSE;
        }
    }

    V5UDevice::~V5UDevice(void)
    {
    }

    BOOL V5UDevice::Initialize(HWND hDisplay)
    {
        BOOL bRet = TRUE;

        m_dLowlevelFPS = m_lowLevelFpsCounter.GetFPS();
        m_dDisplayFPS = m_displayFpsCounter.GetFPS();
        m_hDisplay = hDisplay;

        bRet = ReadV5URef();
        if (!bRet)
        {
            uts.log.Error(_T("[%s]ReadV5URef error!"), UTS_FN);
            return FALSE;
        }

        m_nWidth = m_v5uRef.HVS_SENSOR_SIZE_X;
        m_nHeight = m_v5uRef.HVS_SENSOR_SIZE_Y;
        uts.info.nDeviceIndex = m_v5uRef.BOARD_SET_INDEX_NUM;

        //------------------------------------------------------------------------------
        // 分配空间
        CreateTestBufferMemory();

        //////////////////////////////////////////////////////////////////////
        //	USB3.0 Device Open
        //////////////////////////////////////////////////////////////////////
        int nDeviceCount = OpenDAQDevice();
        if (nDeviceCount <= 0)
        {
            uts.log.Error(_T("USB3.0 open fail!"));
            return FALSE;
        }
        for (int i = 0; i < m_v5uRef.DEVICE_COUNT; i++)
        {
            if (m_v5uRef.DEVICE_COUNT == 1) //只有一个device时，使用任意指定的Index
            {
                i = m_v5uRef.BOARD_SET_INDEX_NUM;
            }

            if (!LVDS_Init_Mul(i))
            {
                uts.log.Error(_T("[V5U API]LVDS_Init_Mul error.[DeviceId = %d]"), i);
                return FALSE;
            }
            int nFPGAVer, nFirmVer;
            LVDS_GetVersion_Mul(i, &nFPGAVer, &nFirmVer);
            uts.log.Info(_T("[Device%d]Ver-FPGA:%d, FWare:%d"), i, nFPGAVer, nFirmVer);	
        }

        return TRUE;
    }

	BOOL V5UDevice::OST(OST_CONFIG OSTConfig, OST_RESULT *OSTResult)
    {
		TCHAR HVSMipiPinName[40][8] = {
			__T("AVDD"),
			__T("DOVDD"),
			__T("DVDD"),
			__T("AFVDD"),
			__T("GND1"),
			__T("GND1"),
			__T("SCL"),
			__T("MD0P"),
			__T("SDA"),
			__T("MD0N"),
			__T("GND1"),
			__T("GND1"),
			__T("PWDN"),
			__T("MD1P"),
			__T("RESET"),
			__T("MD1N"),
			__T("GND1"),
			__T("GND1"),
			__T("IO0"),
			__T("MD2P"),
			__T("IO1"),
			__T("MD2N"),
			__T("GND1"),
			__T("GND1"),
			__T("IO2"),
			__T("MD3P"),
			__T("IO3"),
			__T("MD3N"),
			__T("GND1"),
			__T("GND1"),
			__T("NC"),
			__T("MCP"),
			__T("NC"),
			__T("MCN"),
			__T("MCLK"),
			__T("NC"),
			__T("NC"),
			__T("NC"),
			__T("VPP"),
			__T("NC"),
		};
		float IForceValue[40] = {0};
		float MeasureValue[40] = {0};
		int count, index, ret;

		//OSTest_StartM(gCamera[DevId].DevId, -1, FALSE, FALSE);	//OS Minus, CV, Floating off
		//OSTest_StartM(gCamera[DevId].DevId, 1, FALSE, FALSE);	//OS Plus, CV, Floating off
		OSTest_StartM(m_v5uRef.BOARD_SET_INDEX_NUM, -1, TRUE, FALSE);	//OS Minus, CC, Floating off
		//OSTest_StartM(gCamera[DevId].DevId, 1, TRUE, FALSE);	//OS Plus, CC, Floating off
		//OSTest_StartM(gCamera[DevId].DevId, -1, FALSE, TRUE);	//OS Minus, CV, Floating on
		//OSTest_StartM(gCamera[DevId].DevId, 1, FALSE, TRUE);	//OS plus, CV, Floating on
		//OSTest_StartM(gCamera[DevId].DevId, -1, TRUE, TRUE);	//OS Minus, CC, Floating on
		//OSTest_StartM(gCamera[DevId].DevId, 1, TRUE, TRUE);	//OS plus, CC, Floating on
		Sleep(300);
		for (count = 0; count < 40; count++) {
			for (index = 0; index < 64; index++) {
				if (0 == _tcsncmp(HVSMipiPinName[count], OSTConfig.PinName[index], 8)) {
					if (index < 32) {
						if (0 == (OSTConfig.OSTPinMask[0] & (1 << index))) {
							continue;
						}
					} else {
						if (0 == (OSTConfig.OSTPinMask[1] & (1 << (index - 32)))) {
							continue;
						}
					}

					IForceValue[count] = (float)OSTConfig.OSTCurrent[0] * (-1000);
					ret = OSTest_UnitPinM(count + 1, IForceValue[count], 1, &MeasureValue[count], FALSE, FALSE, m_v5uRef.BOARD_SET_INDEX_NUM);
					OSTResult->OSTMinusVoltage[index] = (INT32)(MeasureValue[count] * (-1000));

					if ((OSTResult->OSTMinusVoltage[index] >= OSTConfig.OSTShortSpec[0]) && (OSTResult->OSTMinusVoltage[index] <= OSTConfig.OSTOpenSpec[0])) {
						OSTResult->OSTResult[index] = FALSE;
					} else {
						OSTResult->OSTResult[index] = TRUE;
					}
					break;
				}
			}
		}
		OSTest_End(m_v5uRef.BOARD_SET_INDEX_NUM);

		index = 0;
		for (count = 0; count < 64; count++) {
			if (OSTResult->OSTResult[count]) {
				if (count < 32) {
					if (0 == (OSTConfig.OSTPinMask[0] & (1 << count))) {
						continue;
					}
				} else {
					if (0 == (OSTConfig.OSTPinMask[1] & (1 << (count - 32)))) {
						continue;
					}
				}

				if (OSTResult->OSTMinusVoltage[count] > OSTConfig.OSTOpenSpec[0]) {
					uts.log.Error(_T("%s Open, %d mV > %d mV(Spec).\n"), OSTConfig.PinName[count], OSTResult->OSTMinusVoltage[count], OSTConfig.OSTOpenSpec[0]);
				} else if (OSTResult->OSTMinusVoltage[count] < OSTConfig.OSTShortSpec[0]) {
					uts.log.Error(_T("%s Short, %d mV < %d mV(Spec).\n"), OSTConfig.PinName[count], OSTResult->OSTMinusVoltage[count], OSTConfig.OSTShortSpec[0]);
				}

				index++;
			}
		}

        if (index) {
			return FALSE;
		} else {
			return TRUE;
		}
    }

    BOOL V5UDevice::Start(LPCTSTR lpRegister)
    {
        //------------------------------------------------------------------------------
		uts.log.Info(_T("V5U Program_PowerOn"));

        BOOL bRet = Program_PowerOn();
        if (!bRet)
        {
            uts.log.Error(_T("Power on fail!"));
            return FALSE;
        }
		uts.log.Info(_T("V5U Oscillator_Set"));
        Oscillator_Set();
        Sleep(10);
		uts.log.Info(_T("V5U BoardControl"));
        BoardControl(); //Board control - RESET/ENB/VCLK/Turbo etc ...

		uts.log.Info(_T("V5U RegisterSet"));

        if (!RegisterSet(lpRegister))
        {
            uts.log.Error(_T("RegisterSet[%s] Error."), lpRegister);
            return FALSE;
        }
        else
        {
            uts.log.Info(_T("%d: PLAY"), m_v5uRef.BOARD_SET_INDEX_NUM);
        }

        if (m_v5uRef.HVS_SENSOR_MIPIMODE == eMipiMode::Parallel)
        {
            // 0:MIPI(default), 1:Parallel
            LVDS_SelectInput_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 1);
            switch (m_v5uRef.HVS_SENSOR_DATA_FORMAT)
            {
            case eDataFormat::YUV:      //DATA_FORMAT 0,1: YUV, 8bit bayer : 8bit data bus
            case eDataFormat::Raw8Bit:
                LVDS_SetDataMode_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 0);
				break;
			case eDataFormat::Raw2PD:
			case eDataFormat::Raw10Bit: //DATA_FORMAT   2: 10bit bayer     : 16bit data bus
                LVDS_SetDataMode_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 1);
                break;
            default:
                uts.log.Error(_T("Unsurrported data format. DataFormat = [%d]"), m_v5uRef.HVS_SENSOR_DATA_FORMAT);
                break;
            }
        }
        else
        {
            // 0:MIPI(default), 1:Parallel
            LVDS_SelectInput_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 0);
            switch (m_v5uRef.HVS_SENSOR_MIPIMODE)
            {
            case eMipiMode::Line1:
                LVDS_SetDataMode_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 0);  //0:8bit->1Lane, 1:16bit->2Lane, 3:32bit->4Lane
                break;
            case eMipiMode::Line2:
                LVDS_SetDataMode_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 1);
                break;
            case eMipiMode::Line4:
                LVDS_SetDataMode_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 3);
                break;
            default:
                uts.log.Error(_T("Unsurrported MIPI mode. MipiMode = [%d]"), m_v5uRef.HVS_SENSOR_MIPIMODE);
                break;
            }
        }
        uts.log.Debug(_T("%d:X:%d,Y:%d"), m_v5uRef.BOARD_SET_INDEX_NUM, m_nWidth, m_nHeight);

        if (LVDS_GetResolution_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &m_mipiX, &m_mipiY))
        {
            uts.log.Info(_T("MIPIXY1:%d, %d"), m_mipiX, m_mipiY);
        }
        else
        {
            uts.log.Warning(_T("MIPI Res. Fail"));
        }

        if (m_v5uRef.HVS_SENSOR_MIPIMODE == eMipiMode::Parallel)
        {
            // try 3 times
            int trycount = 0;
            for (; trycount < 3; trycount++)
            {
                if (m_v5uRef.HVS_SENSOR_DATA_FORMAT == eDataFormat::YUV)
                {
                    if (m_mipiX != 2 * m_nWidth ||  m_mipiY != m_nHeight)
                    {
                        uts.log.Error(_T("Parallel YUV LVDS_GetResolution_Mul Fail, trycount = %d"), trycount);
                        Sleep(500);
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if (m_mipiX != m_nWidth ||  m_mipiY != m_nHeight)
                    {
                        uts.log.Error(_T("Parallel LVDS_GetResolution_Mul Fail, trycount = %d"), trycount);
                        Sleep(500);
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                if (LVDS_GetResolution_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &m_mipiX, &m_mipiY))
                {
                    uts.log.Info(_T("MIPIXY%d:%d, %d"), trycount+1, m_mipiX, m_mipiY);
                }
                else
                {
                    uts.log.Warning(_T("MIPI Res. Fail"));
                }
            }
            if (trycount >= 3)
            {
                uts.log.Error(_T("Resolution de-serial Fail"));
                return FALSE;
            }
        }

        if (m_v5uRef.HVS_SENSOR_MIPIMODE != eMipiMode::Parallel)
        {
            int	idivider, mipidivider=4, countperpixel=1, pixelalign=5;
            int	remain = 0;
            switch (m_v5uRef.HVS_SENSOR_MIPIMODE)
            {
            case eMipiMode::Line1:
                idivider = 1;	//1Lane 8bit transfer
                break;
            case eMipiMode::Line2:
                idivider = 2;  //2Lane 16bit transfer
                break;
            case eMipiMode::Line4:
                idivider = 4;	//4Lane 32bit transfer
                break;
            default:
                uts.log.Error(_T("Unsurrported MIPI mode. MipiMode = [%d]"), m_v5uRef.HVS_SENSOR_MIPIMODE);
                break;
            }

            switch (m_v5uRef.HVS_SENSOR_DATA_FORMAT)
            {
            case eDataFormat::YUV: 
                pixelalign = 5;
                mipidivider = 5;   //8bit : 5/5
                countperpixel = 2; //twice byte size for YUV. 
                break;
            case eDataFormat::Raw8Bit:
                pixelalign = 5;
                mipidivider = 5;   //8bit : 5/5
                countperpixel = 1;
                break;
            case eDataFormat::Raw10Bit:
                pixelalign = 5;
                mipidivider = 4;   //10bit : 5/4
                countperpixel = 1;
                break;
			case eDataFormat::Raw2PD:
				pixelalign = 5;
				mipidivider = 4;   //10bit : 5/4
				countperpixel = 2;
				break;

            default:
                uts.log.Error(_T("Unsurrported data format. DataFormat = [%d]"), m_v5uRef.HVS_SENSOR_DATA_FORMAT);
                break;
            }

            if ((m_nWidth * 5 / mipidivider) % idivider)
            {
                remain = 1;  // not divided by integer ratio
            }
            else
            {
                remain = 0;
            }

            // Exception for not divided by 4 - 2013. 1/22
            if (m_mipiX != (DWORD)(countperpixel * (m_nWidth * pixelalign / mipidivider) / idivider + remain) || m_mipiY != (DWORD)m_nHeight)
            {
                Sleep(500);
                LVDS_GetResolution_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &m_mipiX, &m_mipiY);
                uts.log.Info(_T("MIPIXY2:%d, %d"), m_mipiX, m_mipiY);
                if (m_mipiX != (DWORD)(countperpixel * (m_nWidth * pixelalign / mipidivider) / idivider + remain) || m_mipiY != (DWORD)m_nHeight)
                {
                    Sleep(500);
                    LVDS_GetResolution_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &m_mipiX, &m_mipiY);
                    uts.log.Info(_T("MIPIXY3:%d, %d"), m_mipiX, m_mipiY);
                    if (m_mipiX != (DWORD)(countperpixel * (m_nWidth * pixelalign / mipidivider) / idivider + remain) || m_mipiY != (DWORD)m_nHeight)
                    {
                        uts.log.Error(_T("MIPI de-serial Fail"));
                        return FALSE;
                    }
                }
            }
        }
        if (!LVDS_Start_Mul(m_v5uRef.BOARD_SET_INDEX_NUM))
        {
            uts.log.Error(_T("[V5U API]LVDS_Start_Mul error."));
            return FALSE;
        }
        m_bDvStarting[m_v5uRef.BOARD_SET_INDEX_NUM] = TRUE;
        return TRUE;
    }

    BOOL V5UDevice::Stop()
    {
        if (m_bDvStarting[m_v5uRef.BOARD_SET_INDEX_NUM])
        {
            LVDS_Stop_Mul(m_v5uRef.BOARD_SET_INDEX_NUM);  //For exit duing Play status
            Sleep(300);

            Reset_Low();
            ENB_Low();
            CLK_Off_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, TRUE);

            Program_PowerOffAllM(m_v5uRef.BOARD_SET_INDEX_NUM);		//Power off

            m_bDvStarting[m_v5uRef.BOARD_SET_INDEX_NUM] = FALSE;
        }

        //-------------------------------------------------------------------------
        // fix bug #5 产品点不亮但是preview后会出现画面
        ClearTestBufferMemory();
        //-------------------------------------------------------------------------

        return TRUE;
    }

    BOOL V5UDevice::Close()
    {
        CloseDAQDevice();
        ReleaseTestBufferMemory();
        return TRUE;
    }

    BOOL V5UDevice::GetBufferInfo(BUFFER_INFO &bufferInfo)
    {
        bufferInfo.nWidth = m_nWidth;
        bufferInfo.nHeight = m_nHeight;
        bufferInfo.dwBufferType = 0;

        switch (m_v5uRef.HVS_SENSOR_DATA_FORMAT)
        {
        case eDataFormat::YUV:
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_YUV24;
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_BMP;
			break;
		case eDataFormat::Raw2PD:
        case eDataFormat::Raw8Bit:
        case eDataFormat::Raw10Bit:
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_RAW8;
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_BMP;
            break;
        default:
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_BMP;
            uts.log.Error(_T("Unsurrported data format. DataFormat = [%d]"), m_v5uRef.HVS_SENSOR_DATA_FORMAT);
            break;
        }
        return TRUE;
    }
#ifndef ALIGNED_SIZE
	//origin define at <align.h>, input alignment the less 1 number than you want it. if you want to 4 alignment, input to alignment '3'.
#define ALIGNED_SIZE(size, alignment) (((size) + (alignment)) & ~(alignment))
#endif


    DWORD V5UDevice::GetFrameSize()
    {
        int mipidivider = 8;
        switch (m_v5uRef.HVS_SENSOR_DATA_FORMAT)
        {
        case eDataFormat::Raw10Bit:
            if (m_v5uRef.HVS_SENSOR_MIPIMODE == eMipiMode::Parallel)
            {
                mipidivider = 5;
            }
            else
            {
                mipidivider = 8;
            }
            break;
        case eDataFormat::Raw8Bit:
            mipidivider = 10;
            break;
        case eDataFormat::YUV:
            mipidivider = 5;
			break;
		case eDataFormat::Raw2PD:
			mipidivider = 20;
			break;
        default:
            uts.log.Error(_T("Unsurrported data format. DataFormat = [%d]"), m_v5uRef.HVS_SENSOR_DATA_FORMAT);
        }

		DWORD dwCount = 0 ;

		if(m_v5uRef.HVS_SENSOR_DATA_FORMAT == eDataFormat::Raw2PD)
		{
			dwCount = ALIGNED_SIZE(((m_nWidth*mipidivider) >> 3), 3) * m_nHeight;
		}
		else
		{
			DWORD nWidth = (m_nWidth * 10 / mipidivider);
			if(0!= (nWidth%4))
			{
				nWidth +=2;
			}
			dwCount =  nWidth* m_nHeight;  // size define based on image size define
			DWORD nPacKet = dwCount / 16384;
			if ((dwCount % 16384) != 0)
			{
				nPacKet++;
			}

			dwCount	= nPacKet * 16384;
		}
		
        return dwCount;
    }

    BOOL V5UDevice::GetOneFrame(DWORD dwCount, int nTimeout)
    {
//         BOOL bRet = TRUE;
//         do 
//         {
//             if (LVDS_GetFrame_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &dwCount, m_TestBuffer.pFrameBuffer))
//             {
// 				if (LVDS_GetFrame_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &dwCount, m_TestBuffer.pFrameBuffer))
// 				{
// 					if (LVDS_GetFrame_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &dwCount, m_TestBuffer.pFrameBuffer))
// 					{
// 						if (LVDS_GetFrame_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &dwCount, m_TestBuffer.pFrameBuffer))
// 						{
// 							//if (LVDS_GetFrame_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &dwCount, m_TestBuffer.pFrameBuffer))
// 							{
// 								if (LVDS_GetFrame_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &dwCount, m_TestBuffer.pFrameBuffer))
// 								{
// 									m_dLowlevelFPS = m_lowLevelFpsCounter.GetFPS();
// 									break;
// 								}
// 								//m_dLowlevelFPS = m_lowLevelFpsCounter.GetFPS();
// 								break;
// 							}
// 							//break;
// 						}
// 						break;
// 					}
// 					break;
// 				}
// 				
// 				//m_dLowlevelFPS = m_lowLevelFpsCounter.GetFPS();
//                break;
//             }
//             nTimeout--;
//             //Sleep(20);
// 			UTS::Algorithm::Sleep(10);
// 
//         } while (nTimeout > 0);
//         if (nTimeout <= 0)
//         {
//             bRet = FALSE;
//         }

		BOOL bRet = TRUE;
		do 
		{
			if (LVDS_GetFrame_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, &dwCount, m_TestBuffer.pFrameBuffer))
			{
				m_dLowlevelFPS = m_lowLevelFpsCounter.GetFPS();
				break;
			}
			nTimeout--;
			//Sleep(20);
			UTS::Algorithm::Sleep(10);
		} while (nTimeout > 0);
		if (nTimeout <= 0)
		{
			bRet = FALSE;
		}


        return bRet;
    }

    BOOL V5UDevice::Recapture(TEST_BUFFER &testBuffer, int nDummyCount/* = 0*/, int nAvgCount/* = 1*/)
    {
        const int MIN_DUMMY_COUNT = 5;
        DWORD dwCount = GetFrameSize();

        //------------------------------------------------------------------------------
        // 扔掉不用的帧
//         if (nDummyCount > 0)
//         {
//             LVDS_BufferFlush_Mul(m_v5uRef.BOARD_SET_INDEX_NUM);
//             //if (nDummyCount < MIN_DUMMY_COUNT)
//             //{
//             //    nDummyCount = MIN_DUMMY_COUNT;
//             //}
//         }


        for (int i = 0; i < MIN_DUMMY_COUNT; i++)
        {
            if (!GetOneFrame(dwCount, uts.info.nGetFrameWaitTime))
            {
                uts.log.Error(_T("GetOneFrame Error."));
                return FALSE;
            }
        }


        if (nAvgCount > 1)
        {
            //------------------------------------------------------------------------------
            // 平均的帧
            int pixels = m_nWidth * m_nHeight;
            double *_RAWTemp = new double[pixels];
            double *_BMPTemp = new double[pixels * 3];
            double *_YUVTemp = new double[pixels * 3];
            memset(_RAWTemp, 0, sizeof(double) * pixels);
            memset(_BMPTemp, 0, sizeof(double) * pixels * 3);
            memset(_YUVTemp, 0, sizeof(double) * pixels * 3);
            for (int i = 0; i < nAvgCount; i++)
            {
				if (!GetOneFrame(dwCount, uts.info.nGetFrameWaitTime))
                {
                    RELEASE_ARRAY(_RAWTemp);
                    RELEASE_ARRAY(_BMPTemp);
                    RELEASE_ARRAY(_YUVTemp);
                    uts.log.Error(_T("GetOneFrame Error."));
                    return FALSE;
                }

                if (eDataFormat::YUV == m_v5uRef.HVS_SENSOR_DATA_FORMAT)
                {
                    CImageProc::GetInstance().BufferConv_YUVRawToYUV24(
                        m_TestBuffer.pFrameBuffer,
                        m_nWidth,
                        m_nHeight,
                        m_v5uRef.HVS_SENSOR_OUT_MODE,
                        m_TestBuffer.pYuv24Buffer);
                    CImageProc::GetInstance().BufferConv_YUV24ToBmp(
                        m_TestBuffer.pYuv24Buffer,
                        m_nWidth,
                        m_nHeight,
                        0,
                        m_TestBuffer.pBmpBuffer);
                }
                else
                {
                    if (!CImageProc::GetInstance().GetRaw8AndBmpBuffer(
                        m_TestBuffer.pFrameBuffer,
                        m_nWidth,
                        m_nHeight,
                        m_v5uRef.HVS_SENSOR_DATA_FORMAT,
                        m_v5uRef.HVS_SENSOR_OUT_MODE,
                        m_v5uRef.HVS_SENSOR_MIPIMODE,
                        m_TestBuffer.pRaw8Buffer,
                        m_TestBuffer.pBmpBuffer))
                    {
                        RELEASE_ARRAY(_RAWTemp);
                        RELEASE_ARRAY(_BMPTemp);
                        RELEASE_ARRAY(_YUVTemp);
                        uts.log.Error(_T("GetRaw8AndBmpBuffer Error."));
                        return FALSE;
                    }
                }
                //------------------------------------------------------------------------------
                // 叠加
                for (int j = 0; j < pixels; j++)
                {
                    _RAWTemp[j] += m_TestBuffer.pRaw8Buffer[j];
                    for (int k = 0; k < 3; k++)
                    {
                        _BMPTemp[j * 3 + k] += m_TestBuffer.pBmpBuffer[j * 3 + k];

                        if (eDataFormat::YUV == m_v5uRef.HVS_SENSOR_DATA_FORMAT)
                        {
                            _YUVTemp[j * 3 + k] += m_TestBuffer.pYuv24Buffer[j * 3 + k];
                        }
                    }
                }
            }
            //------------------------------------------------------------------------------
            // 平均
            for (int i = 0; i < pixels; i++)
            {
                m_TestBuffer.pRaw8Buffer[i] = (unsigned char)(_RAWTemp[i] / nAvgCount);

                for (int k = 0; k < 3; k++)
                {
                    m_TestBuffer.pBmpBuffer[i * 3 + k] = (unsigned char)(_BMPTemp[i * 3 + k] / nAvgCount);
                    m_TestBuffer.pYuv24Buffer[i * 3 + k] = (unsigned char)(_YUVTemp[i * 3 + k] / nAvgCount);
                }
            }

            RELEASE_ARRAY(_RAWTemp);
            RELEASE_ARRAY(_BMPTemp);
            RELEASE_ARRAY(_YUVTemp);
        }
        else
        {
			if (!GetOneFrame(dwCount, uts.info.nGetFrameWaitTime))
            {
                uts.log.Error(_T("GetOneFrame Error."));
                return FALSE;
            }

            if (eDataFormat::YUV == m_v5uRef.HVS_SENSOR_DATA_FORMAT)
            {
                CImageProc::GetInstance().BufferConv_YUVRawToYUV24(
                    m_TestBuffer.pFrameBuffer,
                    m_nWidth,
                    m_nHeight,
                    m_v5uRef.HVS_SENSOR_OUT_MODE,
                    m_TestBuffer.pYuv24Buffer);
                CImageProc::GetInstance().BufferConv_YUV24ToBmp(
                    m_TestBuffer.pYuv24Buffer,
                    m_nWidth,
                    m_nHeight,
                    0,
                    m_TestBuffer.pBmpBuffer);
            }
            else
            {
				if (!CImageProc::GetInstance().GetRaw8AndBmpBuffer(
					m_TestBuffer.pFrameBuffer,
					m_nWidth,
					m_nHeight,
					m_v5uRef.HVS_SENSOR_DATA_FORMAT,
					m_v5uRef.HVS_SENSOR_OUT_MODE,
					m_v5uRef.HVS_SENSOR_MIPIMODE,
					m_TestBuffer.pRaw8Buffer,
					m_TestBuffer.pBmpBuffer))
				{
					uts.log.Error(_T("GetRaw8AndBmpBuffer Error."));
					return FALSE;
				}
            }
        }

        memcpy(&testBuffer, &m_TestBuffer, sizeof(TEST_BUFFER));
        return TRUE;
    }

    void V5UDevice::ShowSettingDlg(void)
    {
        //AFX_MANAGE_STATE(AfxGetStaticModuleState());    // 切换为调用本DLL资源
        HINSTANCE save_hInstance = AfxGetResourceHandle();
        AfxSetResourceHandle(theApp.m_hInstance);

        CV5UDeviceSettingDlg dlg;
        dlg.DoModal();
        AfxSetResourceHandle(save_hInstance);
    }

    BOOL V5UDevice::ReadValue(eDeviceReadValueType eReadType, void *pBuffer, int nBufferSize)
    {
        assert(nullptr != pBuffer);
        assert(nBufferSize > 0);

        BOOL bRet = TRUE;
        int nValue = 0;
        switch (eReadType)
        {
        case eDeviceReadValueType::DRVT_I2C_READ:
            if (nBufferSize >= sizeof(BYTE) + sizeof(DWORD) * 3)
            {
                /*
                id      type        supportDevice       vallue
                =================================================
                15      (*5)        V5U                 I2C_READ
                15 (*5):
                    Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
                    Output Format: [DWORD|nCnt][unsigned char[?]|buf]
                */
                unsigned char *pTmp = (unsigned char *)pBuffer;
                BYTE slAddr = 0;
                DWORD nAddrLen = 0;
                DWORD nAddr = 0;
                DWORD nCnt = 0;
                memcpy(&slAddr, pTmp, sizeof(BYTE));
                pTmp += sizeof(BYTE);
                memcpy(&nAddrLen, pTmp, sizeof(DWORD));
                pTmp += sizeof(DWORD);
                memcpy(&nAddr, pTmp, sizeof(DWORD));
                pTmp += sizeof(DWORD);
                memcpy(&nCnt, pTmp, sizeof(DWORD));
                pTmp += sizeof(DWORD);
                if (nBufferSize >= (int)(sizeof(BYTE) + sizeof(DWORD) * 3 + sizeof(unsigned char) * nCnt))
                {
                    unsigned char dataBuf[256] = {0};
                    memcpy(&dataBuf, pTmp, sizeof(unsigned char) * nCnt);
                    bRet = I2C_SYS_Read_Mul(
                        m_v5uRef.BOARD_SET_INDEX_NUM,
                        slAddr,
                        nAddrLen,
                        nAddr,
                        nCnt,
                        dataBuf);
                    if (!bRet)
                    {
                        uts.log.Error(_T("[V5UDevice::ReadValue::DRVT_I2C_READ]I2C_SYS_Read_Mul failed."));
                        bRet = FALSE;
                    }
                    memset(pBuffer, 0, nBufferSize);
                    pTmp = (unsigned char *)pBuffer;
                    memcpy(pTmp, &nCnt, sizeof(DWORD));
                    pTmp += sizeof(DWORD);
                    memcpy(pTmp, dataBuf, sizeof(unsigned char) * nCnt);
                }
                else
                {
                    bRet = FALSE;
                }
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceReadValueType::DRVT_SLAV_ADDR:
            if (nBufferSize >= sizeof(int))
            {
                nValue = m_v5uRef.wHVS_SENSOR_SLAVEADDR;
                memcpy(pBuffer, &nValue, sizeof(int));
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceReadValueType::DRVT_IIC_MODE:
            if (nBufferSize >= sizeof(int))
            {
                nValue = m_v5uRef.HVS_SENSOR_I2CMODE;
                memcpy(pBuffer, &nValue, sizeof(int));
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceReadValueType::DRVT_SENSOR_OUT_MODE:
            if (nBufferSize >= sizeof(int))
            {
                nValue = m_v5uRef.HVS_SENSOR_OUT_MODE;
                memcpy(pBuffer, &nValue, sizeof(int));
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceReadValueType::DRVT_CURRENT_STANDBY_OFFSET:
            if (nBufferSize >= sizeof(int) * 6)
            {
                int nRet = StandbyOffset_EachPowerM(
                    (int*)pBuffer,
                    (float)(m_v5uRef.power_volt[0]),
                    (float)(m_v5uRef.power_volt[1]),
                    (float)(m_v5uRef.power_volt[2]),
                    (float)(m_v5uRef.power_volt[3]),
                    (float)(m_v5uRef.power_volt[4]),
                    (m_v5uRef.power_on[5] != 0),
                    (float)(m_v5uRef.power_volt[6]),
                    m_v5uRef.BOARD_SET_INDEX_NUM);
                if (1 == nRet)
                {
                    uts.log.Info(_T("Standby offset ok."));
                }
                else
                {
                    uts.log.Error(_T("Standby offset failed. Errorcode = %d"), nRet);
                    bRet = FALSE;
                }
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceReadValueType::DRVT_CURRENT_DYNAMIC_OFFSET:
            if (nBufferSize >= sizeof(int) * 6)
            {
                int nRet = DynamicOffset_EachPowerM(
                    (int*)pBuffer,
                    (float)(m_v5uRef.power_volt[0]),
                    (float)(m_v5uRef.power_volt[1]),
                    (float)(m_v5uRef.power_volt[2]),
                    (float)(m_v5uRef.power_volt[3]),
                    (float)(m_v5uRef.power_volt[4]),
                    (m_v5uRef.power_on[5] != 0),
                    (float)(m_v5uRef.power_volt[6]),
                    m_v5uRef.BOARD_SET_INDEX_NUM);
                if (1 == nRet)
                {
                    uts.log.Info(_T("Dynamic offset ok."));
                }
                else
                {
                    uts.log.Error(_T("Dynamic offset failed. Errorcode = %d"), nRet);
                    bRet = FALSE;
                }
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceReadValueType::DRVT_CURRENT_STANDBY_MEASURE:
            if (nBufferSize >= sizeof(int) * 2)
            {
                // Input  Format: [int|PinIndexBaseFrom1][int|OffsetValue]
                // Output Format: [double|MeasuredCurrentValue]
                unsigned char *pTmp = (unsigned char *)pBuffer;
                int nPinIndex = 0;
                int nOffsetValue = 0;
                float fMeasuredCurrentValue = 0.0f;
                memcpy(&nPinIndex, pTmp, sizeof(int));
                memcpy(&nOffsetValue, pTmp + sizeof(int), sizeof(int));
                int nRet = StandbyMeasureM(nPinIndex - 1, nOffsetValue, &fMeasuredCurrentValue, 3, m_v5uRef.BOARD_SET_INDEX_NUM);
                if (1 == nRet)
                {
                    double dValue = fMeasuredCurrentValue;
                    memcpy(pTmp, &dValue, sizeof(double));
                    uts.log.Info(_T("Standby measure ok."));
                }
                else
                {
                    uts.log.Error(_T("Standby measure failed. Errorcode = %d"), nRet);
                    bRet = FALSE;
                }
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceReadValueType::DRVT_CURRENT_DYNAMIC_MEASURE:
            if (nBufferSize >= sizeof(int) * 2)
            {
                // Input  Format: [int|PinIndexBaseFrom1][int|OffsetValue]
                // Output Format: [double|MeasuredCurrentValue]
                unsigned char *pTmp = (unsigned char *)pBuffer;
                int nPinIndex = 0;
                int nOffsetValue = 0;
                float fMeasuredCurrentValue = 0.0f;
                memcpy(&nPinIndex, pTmp, sizeof(int));
                memcpy(&nOffsetValue, pTmp + sizeof(int), sizeof(int));
                int nRet = DynamicMeasureM(nPinIndex - 1, nOffsetValue, &fMeasuredCurrentValue, 3, m_v5uRef.BOARD_SET_INDEX_NUM);
                if (1 == nRet)
                {
                    double dValue = fMeasuredCurrentValue;
                    memcpy(pTmp, &dValue, sizeof(double));
                    uts.log.Info(_T("Dynamic measure ok."));
                }
                else
                {
                    uts.log.Error(_T("Dynamic measure failed. Errorcode = %d"), nRet);
                    bRet = FALSE;
                }
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceReadValueType::DRVT_REG_READ:
            if (nBufferSize >= sizeof(WORD))
            {
                //Input  Format: [WORD|wRegAddr]
                //Output Format: [WORD|wRegData]
                unsigned char *pTmp = (unsigned char *)pBuffer;
                WORD wAddr = 0;
                memcpy(&wAddr, pTmp, sizeof(WORD));
                WORD wData = m_HVSI2C.IIC_Read_Proc(
                    m_v5uRef.HVS_SENSOR_I2CMODE,
                    m_v5uRef.wHVS_SENSOR_SLAVEADDR,
                    wAddr,
                    m_v5uRef.BOARD_SET_INDEX_NUM);
                memcpy(pTmp, &wData, sizeof(WORD));
            }
            else
            {
                bRet = FALSE;
            }
            break;
		case eDeviceReadValueType::DRVT_READ_GPIO:
			if (nBufferSize >= sizeof(int) )
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int GPIO_index = 0;
				int _OnOff = 0;
				memcpy(&GPIO_index, pTmp, sizeof(int));

				if (GPIO_index < 0 || GPIO_index > 3)     //OHSK_M
				{
					bRet = FALSE;
				}

				_OnOff = GPIO_Read(GPIO_index, m_v5uRef.BOARD_SET_INDEX_NUM);
				memcpy(pTmp, &_OnOff, sizeof(int));
			} else
			{
				bRet = FALSE;
			}
			break;

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

    BOOL V5UDevice::WriteValue(eDeviceWriteValueType eWriteType, void *pBuffer, int nBufferSize)
    {
        BOOL bRet = TRUE;
        const int BUFFER_SIZE = 1024;
        TCHAR szBuffer[BUFFER_SIZE] = {0};
        switch (eWriteType)
        {
        case eDeviceWriteValueType::DWVT_SET_POWER_PIN:
            if (nBufferSize >= sizeof(int) + sizeof(float))
            {
                /*
                id      type        supportDevice       vallue
                =================================================
                13      (*4)        V5U                 SET_POWER_PIN
                13 (*4):
                    Input Format: [int|idx][float|volt]
                */
                unsigned char *pTmp = (unsigned char *)pBuffer;
                int idx = 0;
				int setResult;
                float volt = 0.0f;
                memcpy(&idx, pTmp, sizeof(int));
                pTmp += sizeof(int);
                memcpy(&volt, pTmp, sizeof(float));
				if(volt > 0.1)
				{
					setResult = Program_PowerSetM(idx, volt, m_v5uRef.BOARD_SET_INDEX_NUM);
				}else
				{
					setResult = Program_PowerOffM(idx,m_v5uRef.BOARD_SET_INDEX_NUM);
				}
                if (setResult == -1)
                {
                    uts.log.Error(_T("Power set range over! channel :%d"), idx);
                    return FALSE;
                }
                else if (setResult == -2)
                {
                    uts.log.Error(_T("Power set fail of channel! channel :%d"), idx);
                    return FALSE;
                }
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceWriteValueType::DWVT_IIC_MODE:
            if (nBufferSize >= sizeof(int))
            {
                /*
                id      type        supportDevice       vallue
                =================================================
                12      int         V5U                 IIC_MODE
                */
                int nIICMode = 0;
                memcpy(&nIICMode, pBuffer, sizeof(int));
                m_v5uRef.HVS_SENSOR_I2CMODE = nIICMode;
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
                uts.log.Info(_T("Register %s"), szBuffer);
            }
            break;
        case eDeviceWriteValueType::DWVT_I2C_WRITE:
            if (nBufferSize >= sizeof(BYTE) + sizeof(DWORD) * 3)
            {
                /*
                id      type        supportDevice       vallue
                =================================================
                1       (*1)        V5U                 I2C_WRITE
                (*1):
                    Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
                    Output Format: [Not Use]
                */
                unsigned char *pTmp = (unsigned char *)pBuffer;
                BYTE slAddr = 0;
                DWORD nAddrLen = 0;
                DWORD nAddr = 0;
                DWORD nCnt = 0;
                memcpy(&slAddr, pTmp, sizeof(BYTE));
                pTmp += sizeof(BYTE);
                memcpy(&nAddrLen, pTmp, sizeof(DWORD));
                pTmp += sizeof(DWORD);
                memcpy(&nAddr, pTmp, sizeof(DWORD));
                pTmp += sizeof(DWORD);
                memcpy(&nCnt, pTmp, sizeof(DWORD));
                pTmp += sizeof(DWORD);

				//uts.log.Debug(_T("Slave : %x addr: %x  len:%d dataCount :%d"),slAddr,nAddr,nAddrLen,nCnt);

                if (nBufferSize >= (int)(sizeof(BYTE) + sizeof(DWORD) * 3 + sizeof(unsigned char) * nCnt))
                {
                    unsigned char dataBuf[256] = {0};
                    memcpy(&dataBuf, pTmp, sizeof(unsigned char) * nCnt);
                    bRet = I2C_SYS_Write_Mul(
                        m_v5uRef.BOARD_SET_INDEX_NUM,
                        slAddr,
                        nAddrLen,
                        nAddr,
                        nCnt,
                        dataBuf);
                }
                else
                {
                    bRet = FALSE;
                }
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceWriteValueType::DWVT_REG_WRITE:
            if (nBufferSize >= sizeof(WORD) * 2)
            {
                /*
                id      type        supportDevice       vallue
                =================================================
                2       (*2)        V5U                 REG_WRITE
                (*2):
                    Input  Format: [WORD|wRegAddr][WORD|wRegData]
                    Output Format: [Not Use]
                */
                unsigned char *pTmp = (unsigned char *)pBuffer;
                WORD wRegAddr = 0;
                WORD wRegData = 0;
                memcpy(&wRegAddr, pTmp, sizeof(WORD));
                memcpy(&wRegData, pTmp + sizeof(WORD), sizeof(WORD));
                m_HVSI2C.IIC_Write_Proc(
                    m_v5uRef.HVS_SENSOR_I2CMODE,
                    m_v5uRef.wHVS_SENSOR_SLAVEADDR,
                    wRegAddr,
                    wRegData,
                    m_v5uRef.BOARD_SET_INDEX_NUM);
            }
            else
            {
                bRet = FALSE;
            }
            break;
        case eDeviceWriteValueType::DWVT_SWITCH_DEVICE_INDEX:
            if (nBufferSize >= sizeof(int))
            {
                /*
                id      type        supportDevice       vallue
                =================================================
                3       int*        V5U                 SWITCH_DEVICE_INDEX
                */
                unsigned char *pTmp = (unsigned char *)pBuffer;
                int nDeviceIndex = 0;
                memcpy(&nDeviceIndex, pTmp, sizeof(int));
                m_v5uRef.BOARD_SET_INDEX_NUM = nDeviceIndex;
            }
            else
            {
                bRet = FALSE;
            }
            break;
		case eDeviceWriteValueType::DWVT_SET_GPIO:
			// IO level set
			IOlevel_SetM((float)(m_v5uRef.power_volt[6]), m_v5uRef.BOARD_SET_INDEX_NUM); 

			if(nBufferSize >= sizeof(int)+ sizeof(bool))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int GPIO_index = 0;
				bool _OnOff = false;
				memcpy(&GPIO_index, pTmp, sizeof(int));
				memcpy(&_OnOff, pTmp + sizeof(int), sizeof(bool));

				//uts.log.Debug(_T("GPIO_index:%d state:%d"),GPIO_index,_OnOff);

				if (GPIO_index < 0 || GPIO_index > 3)     //OHSK_M
				{
					bRet = FALSE;
				}

				if (_OnOff == false)
				{
					uts.log.Error(_T("GPIO_%d OFF"), GPIO_index);
					GPIOWrite_Low(GPIO_index, m_v5uRef.BOARD_SET_INDEX_NUM);		//0: Low
				}
				else
				{
					uts.log.Error(_T("GPIO_%d ON"), GPIO_index);
					GPIOWrite_High(GPIO_index, m_v5uRef.BOARD_SET_INDEX_NUM);	//1: High	
				}	
			}
			else
			{
				bRet = FALSE;
			}			
			break;
		
		case eDeviceWriteValueType::DWVT_SENSOR_OUT_MODE:
			if (nBufferSize >= sizeof(int))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int nSensorOutputMode = 0;
				memcpy(&nSensorOutputMode, pTmp, sizeof(int));
				m_v5uRef.HVS_SENSOR_OUT_MODE = nSensorOutputMode;
			}
			else
			{
				bRet = FALSE;
			}
			break;

		case eDeviceWriteValueType::DWVT_SENSOR_SLAV_ADDR:
			if (nBufferSize >= sizeof(int))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int nSensorSlaveAddr = 0;
				memcpy(&nSensorSlaveAddr, pTmp, sizeof(int));
				m_v5uRef.wHVS_SENSOR_SLAVEADDR = nSensorSlaveAddr;
			}
			else
			{
				bRet = FALSE;
			}
			break;

		case eDeviceWriteValueType::DWVT_SET_POWPERIN36:
			if(nBufferSize >= sizeof(bool))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				bool _OnOff = false;
				memcpy(&_OnOff, pTmp, sizeof(bool));
				if (_OnOff == true)
				{
					Program_PowerOnOff_Fix36M(TRUE, m_v5uRef.BOARD_SET_INDEX_NUM);
				}else
					Program_PowerOnOff_Fix36M(FALSE, m_v5uRef.BOARD_SET_INDEX_NUM);
			}else
			{
				bRet = FALSE;
			}
			break;
		case eDeviceWriteValueType::DWVT_RESET_POWER_PIN:
			if (nBufferSize >= 5*sizeof(double))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				double volt = 0;
				memcpy(&volt, pTmp, sizeof(double));
				m_v5uRef.power_volt[0] = volt;
				memcpy(&volt, pTmp + sizeof(double), sizeof(double));
				m_v5uRef.power_volt[1] = volt;
				memcpy(&volt, pTmp + sizeof(double)*2, sizeof(double));
				m_v5uRef.power_volt[2] = volt;
				memcpy(&volt, pTmp + sizeof(double)*3, sizeof(double));
				m_v5uRef.power_volt[3] = volt;
				memcpy(&volt, pTmp + sizeof(double)*4, sizeof(double));
				m_v5uRef.power_volt[4] = volt;
			}else
			{
				bRet = FALSE;
			}
			break;

		case DWVT_RESET_POWERSEQUENCE_PIN:
			if (nBufferSize >= 4*sizeof(int))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int nTemp = 0;
				memcpy(&nTemp, pTmp, sizeof(int));
				m_v5uRef. HVS_SENSOR_VCLK= nTemp;
				memcpy(&nTemp, pTmp + sizeof(int), sizeof(int));
				m_v5uRef.HVS_SENSOR_VSYNC= nTemp;
				memcpy(&nTemp, pTmp + 2*sizeof(int), sizeof(int));
				m_v5uRef.HVS_SENSOR_ENB= nTemp;
				memcpy(&nTemp, pTmp + 3*sizeof(int), sizeof(int));
				m_v5uRef.HVS_SENSOR_RESET= nTemp;
			}
			else
			{
				bRet = FALSE;
			}
			break;

		case DWVT_RESET_IMAGESIZE:
			if (nBufferSize >= 2*sizeof(int))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int nTemp = 0;
				memcpy(&nTemp, pTmp, sizeof(int));
				m_v5uRef. HVS_SENSOR_SIZE_X= nTemp;
				memcpy(&nTemp, pTmp + sizeof(int), sizeof(int));
				m_v5uRef.HVS_SENSOR_SIZE_Y= nTemp;

				m_nWidth = m_v5uRef.HVS_SENSOR_SIZE_X;
				m_nHeight = m_v5uRef.HVS_SENSOR_SIZE_Y;

			}else
			{
				bRet = FALSE;
			}

			break;

		case DWVT_RESET_DATAFORMAT:
			if (nBufferSize >= sizeof(int))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int nTemp = 0;
				memcpy(&nTemp, pTmp, sizeof(int));
				m_v5uRef. HVS_SENSOR_DATA_FORMAT= nTemp;
			}else
			{
				bRet = FALSE;
			}


			break;

		case DWVT_RESET_MIPIMode:
			if (nBufferSize >= sizeof(int))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int nTemp = 0;
				memcpy(&nTemp, pTmp, sizeof(int));
				m_v5uRef. HVS_SENSOR_MIPIMODE= nTemp;
			}else
			{
				bRet = FALSE;
			}
			break;

		case DWVT_RESET_I2CMode:
			if (nBufferSize >= sizeof(int))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int nTemp = 0;
				memcpy(&nTemp, pTmp, sizeof(int));
				m_v5uRef. HVS_SENSOR_I2CMODE= nTemp;
			}else
			{
				bRet = FALSE;
			}
			break;

		case DWVT_SET_MCLK:
			if (nBufferSize >= sizeof(int))
			{
				unsigned char *pTmp = (unsigned char *)pBuffer;
				int nTemp = 0;
				memcpy(&nTemp, pTmp, sizeof(int));
				m_v5uRef.HW_PGMFREQ = nTemp;
			}else
			{
				bRet = FALSE;
			}
		     break;
        default:
            break;
        }
        if (!bRet)
        {
            uts.log.Error(_T("[%s]WriteValue Error. WriteType = %d, nBufferSize = %d"),
                UTS_FN, (int)eWriteType, nBufferSize);
        }
        return bRet;
    }

    BOOL V5UDevice::ReadV5URef()
    {
        //------------------------------------------------------------------------------
        // 获取param
        CString strSection;
        if (!uts.dbCof.GetDeviceFilename(uts.info.nDeviceId, strSection))
        {
            uts.log.Error(_T("[%s]GetDeviceFilename Fail. nDeviceId = %d"), UTS_FN, uts.info.nDeviceId);
            return FALSE;
        }
        // BOARD_SET
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("DEVICE_COUNT"), m_v5uRef.DEVICE_COUNT, 1, _T("Total Device Count."));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("BOARD_SET_INDEX_NUM"), m_v5uRef.BOARD_SET_INDEX_NUM, 0, _T("Initial Device index."));
        // pin on
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin1_On"), m_v5uRef.power_on[0], 1, _T("1: on  0: off"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin2_On"), m_v5uRef.power_on[1], 1, _T("1: on  0: off"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin3_On"), m_v5uRef.power_on[2], 1, _T("1: on  0: off"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin4_On"), m_v5uRef.power_on[3], 1, _T("1: on  0: off"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin35_On"), m_v5uRef.power_on[4], 1, _T("1: on  0: off"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin36_On"), m_v5uRef.power_on[5], 1, _T("1: on  0: off"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPinIO_On"), m_v5uRef.power_on[6], 0, _T("1: on  0: off")); //io power.
        // pin volt
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin1_Volt"), m_v5uRef.power_volt[0], 2.80, _T("Volt of power pin 1"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin2_Volt"), m_v5uRef.power_volt[1], 2.80, _T("Volt of power pin 2"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin3_Volt"), m_v5uRef.power_volt[2], 2.80, _T("Volt of power pin 3"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin4_Volt"), m_v5uRef.power_volt[3], 2.80, _T("Volt of power pin 4"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin35_Volt"), m_v5uRef.power_volt[4], 2.80, _T("Volt of power pin 35"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPin36_Volt"), m_v5uRef.power_volt[5], 2.80, _T("Volt of power pin 36"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PowerPinIO_Volt"), m_v5uRef.power_volt[6], 2.80, _T("Volt of power pin IO"));
        if (0 == m_v5uRef.power_on[6])
        {
            m_v5uRef.power_volt[6] = 1.80;   // default sensor IO volt.
        }

        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_AVDDSlowUPEn"), m_v5uRef.HW_AVDDSlowUPEn, 0, _T("0: Disable, 1: Enable"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_PGMFREQ"), m_v5uRef.HW_PGMFREQ, 12, _T("MCLK, Unit MHz."));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HW_OSC_TYPE"), m_v5uRef.HW_OSC_TYPE, 1, _T("Freq output type, 1: inter 0: extern"));

        // sensor set
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_VCLK"), m_v5uRef.HVS_SENSOR_VCLK, 1, _T("1: High 0: low"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_RESET"), m_v5uRef.HVS_SENSOR_RESET, 1, _T("1: High 0: low"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_ENB"), m_v5uRef.HVS_SENSOR_ENB, 1, _T("1: High 0: low"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_VSYNC"), m_v5uRef.HVS_SENSOR_VSYNC, 1, _T("1: High 0: low"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_SLAVEADDR"), m_v5uRef.strHVS_SENSOR_SLAVEADDR, _T("0x00"), _T("IIC Address"));
        _stscanf_s(m_v5uRef.strHVS_SENSOR_SLAVEADDR, _T("%x"), &m_v5uRef.wHVS_SENSOR_SLAVEADDR);
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_SIZE_X"), m_v5uRef.HVS_SENSOR_SIZE_X, 640, _T("Resolution X"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_SIZE_Y"), m_v5uRef.HVS_SENSOR_SIZE_Y, 480, _T("Resolution Y"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_DATA_FORMAT"), m_v5uRef.HVS_SENSOR_DATA_FORMAT, eDataFormat::YUV, _T("0:YUV, 1:8bit Bayer 2: 10bit Bayer 12: 2PD"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_OUT_MODE"), m_v5uRef.HVS_SENSOR_OUT_MODE, eBayerOutputMode::BGGR, _T("if Bayer: 1:BGGR  2:RGGB  3:GBRG  4:GRBG  5:BlackWhite\nif YUV : 1 -YCbYCr, 2 - YCrYCb, 3 - CbYCrY,4 - CrYCbY"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_MIPIMODE"), m_v5uRef.HVS_SENSOR_MIPIMODE, eMipiMode::Line4, _T("0:1L, 1:2L, 7:4L, 9: parallel"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_I2CMODE"), m_v5uRef.HVS_SENSOR_I2CMODE, eI2CMode::BIT8_BIT8, _T("0:8_8, 1:8_16, 2:16_8, 3:16_16"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("HVS_SENSOR_SleepTime"), m_v5uRef.HVS_SENSOR_SleepTime, 0, _T("Interval between register step, unit ms."));
        return TRUE;
    }


#pragma region BoardControl

    // Program Power Setting
    BOOL V5UDevice::Program_PowerOn()
    {
        float voltage_avdd = 0.75;
        int setResult = 0;
        int slowup = m_v5uRef.HW_AVDDSlowUPEn;
        for (int inum = 0; inum < 5; inum++)    //PGM 5 channel Power
        {
            if ((inum == 0) && (slowup == 1))
            {
//               while (voltage_avdd < (float)m_v5uRef.power_volt[inum])
                {
//                     voltage_avdd += 0.25; 
//                     if (voltage_avdd > (float)m_v5uRef.power_volt[inum])
//                     {
//                         voltage_avdd = (float)m_v5uRef.power_volt[inum];
//                     }

					voltage_avdd = (float)m_v5uRef.power_volt[inum];
                    setResult = Program_PowerSetM(
                        inum,
                        voltage_avdd,
                        m_v5uRef.BOARD_SET_INDEX_NUM);
                }
            }
            if (m_v5uRef.power_on[inum] != 0)
            {
                setResult = Program_PowerSetM(
                    inum,
                    (float)m_v5uRef.power_volt[inum],
                    m_v5uRef.BOARD_SET_INDEX_NUM);
                if (setResult == -1)
                {
                    uts.log.Error(_T("Power set range over! channel :%d"), inum);
                    return FALSE;
                }
                else if (setResult == -2)
                {
                    uts.log.Error(_T("Power set fail of channel! channel :%d"), inum);
                    return FALSE;
                }
            }
        }

        if (m_v5uRef.power_on[5] != 0)
        {
            Program_PowerOnOff_Fix36M(TRUE, m_v5uRef.BOARD_SET_INDEX_NUM);
        }
        else
        {
            Program_PowerOnOff_Fix36M(FALSE, m_v5uRef.BOARD_SET_INDEX_NUM);
        }

        // IO level set
        IOlevel_SetM((float)(m_v5uRef.power_volt[6]), m_v5uRef.BOARD_SET_INDEX_NUM); 

        return TRUE;
    }

    void V5UDevice::Oscillator_Set()
    {
        if (m_v5uRef.HW_OSC_TYPE != 0)
        {
            //PGM MCLK 
            DWORD freqVal = m_v5uRef.HW_PGMFREQ * 1000000;
            if (!CLK_Off_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, FALSE))   // clock generation on
            {
                uts.log.Error(_T("[V5U API]CLK_Off_Mul error."));
            }
            if (!CLK_Set_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, freqVal))
            {
                uts.log.Error(_T("[V5U API]CLK_Set_Mul error."));
            }
            if (!CLK_Select_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 1))
            {
                uts.log.Error(_T("[V5U API]CLK_Select_Mul error."));
            }
        }
        else
        {
            if (!CLK_Off_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, FALSE))   // clock generation on
            {
                uts.log.Error(_T("[V5U API]CLK_Off_Mul error."));
            }
            if (!CLK_Select_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 0))
            {
                uts.log.Error(_T("[V5U API]CLK_Select_Mul error."));
            }
            uts.log.Info(_T("Fix OSC select"));
        }
    }

    void V5UDevice::BoardControl()
    {
        // Vsync - 0: bypass , 1: inverse.
        if (m_v5uRef.HVS_SENSOR_VSYNC == 0)
        {
            VSYNC_ByPass();
        }
        else
        {
            VSYNC_Inv();
        }

        // VCLK - 0: bypass , 1 : inverse
        if (m_v5uRef.HVS_SENSOR_VCLK == 0)
        {
            VCLK_ByPass();
        }
        else
        {
            VCLK_Inv();
        }

        // ENB low/high
        if (m_v5uRef.HVS_SENSOR_ENB == 1)
        {
            ENB_High(); //enb high
            Sleep(3);					
            ENB_Low();  //enb low
            Sleep(3);
            ENB_High(); //enb high
        }
        else
        {
            ENB_High(); //enb high
            Sleep(3);
            ENB_Low();  //enb low
            Sleep(3);
        }

        // RESET low/high
        if (m_v5uRef.HVS_SENSOR_RESET == 1)
        {
            Reset_High(); //reset high
            Sleep(3);
            Reset_Low();  //reset low
            Sleep(3);
            Reset_High(); //reset high
            Sleep(1);                    
        }
        else
        {
            Reset_Low();  //reset low
            Sleep(3);
            Reset_High(); //reset high
            Sleep(3);
            Reset_Low();  //reset low
        }
    }

    void V5UDevice::Reset_High()
    {
        if (!SEN_Reset_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, TRUE))
        {
            //UTS::log.Error(_T("[V5U API]SEN_Reset_Mul error."));
        }
    }
    void V5UDevice::Reset_Low()
    {
        if (!SEN_Reset_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, FALSE))
        {
            //UTS::log.Error(_T("[V5U API]SEN_Reset_Mul error."));
        }
    }
    void V5UDevice::ENB_High()
    {
        if (!SEN_Enable_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, TRUE))
        {
            //UTS::log.Error(_T("[V5U API]SEN_Enable_Mul error."));
        }
    }
    void V5UDevice::ENB_Low()
    {
        if (!SEN_Enable_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, FALSE))
        {
            //UTS::log.Error(_T("[V5U API]SEN_Enable_Mul error."));
        }		
    }
    // VCLK/VSYNC is for parallel sensor
    void V5UDevice::VCLK_ByPass()
    {
        if (!LVDS_PclkPol_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 0))
        {
            //UTS::log.Error(_T("[V5U API]LVDS_PclkPol_Mul error."));
        }
        //SetVCLKMode(0);			
    }
    void V5UDevice::VCLK_Inv()
    {
        if (!LVDS_PclkPol_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 1))
        {
            //UTS::log.Error(_T("[V5U API]LVDS_PclkPol_Mul error."));
        }
        //SetVCLKMode(1); //parameter feedback for read function			
    }

    void V5UDevice::VSYNC_ByPass()
    {
        if (!LVDS_VsyncPol_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 0))
        {
            //UTS::log.Error(_T("[V5U API]LVDS_VsyncPol_Mul error."));
        }
        //SetVSYNCMode(0);			
    }
    void V5UDevice::VSYNC_Inv()
    {
        if (!LVDS_VsyncPol_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, 1))
        {
            //UTS::log.Error(_T("[V5U API]LVDS_VsyncPol_Mul error."));
        }
        //SetVSYNCMode(1);			
    }
    void V5UDevice::MCLK_Off()
    {
        if (!CLK_Off_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, TRUE)) //PGM clock off
        {
            //UTS::log.Error(_T("[V5U API]CLK_Off_Mul error."));
        }
    }
#pragma endregion

#pragma region RegisterHandler

#if 0
    BOOL V5UDevice::RegisterSet(LPCTSTR lpSectionName)
    {
        if (!ParseRegisterFileBoard(lpSectionName, uts.info.strConfigPath + _T("CA034(OV8850).ini")))
        {
            uts.log.Error(_T("[%s]ParseRegisterFileBoard error."), UTS_FN);
            return FALSE;
        }

        return TRUE;
    }

    BOOL V5UDevice::ParseRegisterFileBoard(LPCTSTR lpSectionName, LPCTSTR lpFilename)
    {
        BOOL    bRet = TRUE;
        FILE	*fp = nullptr;
        errno_t err = 0;
        TCHAR	linebufIn[1024] = {0};
        TCHAR	tmp_cmd[50] = {0};
        BOOL	bwrite_start = FALSE;

        err = _tfopen_s(&fp, lpFilename, _T("r, ccs=UTF-8"));
        if (0 != err)
        {
            uts.log.Error(_T("[%s]V5U Ref file[%s] is NOT exist!"), UTS_FN, lpFilename);
            bRet = FALSE;
            goto end;
        }

        // Read line
        while (_fgetts(linebufIn, 1024, fp))
        {
            // get line first String
            memset(tmp_cmd, 0, sizeof(tmp_cmd));
            _stscanf_s(linebufIn, _T("%s"), tmp_cmd, _countof(tmp_cmd));

            if ((0 == _tcscmp(lpSectionName, tmp_cmd))) //mode
            {
                uts.log.Info(_T("%s"), tmp_cmd);
                if (!bwrite_start)
                {
                    bwrite_start = TRUE;
                }
            }
            else if (0 == _tcscmp(_T("[END]"), tmp_cmd) && bwrite_start)
            {
                bwrite_start = FALSE;
                break;
            }
            if (TRUE == bwrite_start)
            {
                bRet = ExecuteCommand(linebufIn);
                if (!bRet)
                {
                    goto end;
                }
            }   //end bWrite
            memset(linebufIn, 0, sizeof(linebufIn));
        }

end:
        if (nullptr != fp)
        {
            fclose(fp);
        }
        return bRet;
    }
#else
    BOOL V5UDevice::RegisterSet(LPCTSTR lpSectionName)
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

		vecRegister.clear();

        // 执行
        if (!ExecuteCommandList(strData))
        {
            uts.log.Error(_T("ExecuteCommandList Fail."));
            return FALSE;
        }

        return TRUE;
    }

    BOOL V5UDevice::ExecuteCommandList(LPCTSTR lpCommandList)
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
#endif

    BOOL V5UDevice::ExecuteCommand(LPCTSTR linebufIn)
    {
        int		tmp_idata = 0;
        int		GPIO_index = 0;   //0 ~ 3 for industrial, 0 ~ 7 for toolkit
        int     GPIO_value = 0;
        TCHAR	tmp_cmd[50] = {0};
        TCHAR	tmp_data[50] = {0};
        DWORD	dwRegAddr   = 0;
        DWORD   dwRegData   = 0;
        int     nBoardindex = m_v5uRef.BOARD_SET_INDEX_NUM;
        int		SleepTime = m_v5uRef.HVS_SENSOR_SleepTime;

        CString strLine = linebufIn;
        strLine.MakeUpper();

        if (strLine.Left(2) == _T("0X"))
        {
            if (_stscanf_s(linebufIn, _T("%x%x"), &dwRegAddr, &dwRegData) != 2)
            {
                uts.log.Error(_T("register format error. [Addr = 0x%x][Data = 0x%x]"), dwRegAddr, dwRegData);
                return FALSE;
            }
            if (!m_HVSI2C.IIC_Write_Proc2(m_v5uRef.HVS_SENSOR_I2CMODE, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData, nBoardindex))
            {
                uts.log.Error(_T("IIC_Write_Proc2 error. [I2CMode = %d][I2CAddr = 0x%x][Addr = 0x%x][Data = 0x%x]"),
                    m_v5uRef.HVS_SENSOR_I2CMODE, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData);
                return FALSE;
            }
            Sleep(SleepTime);
        }
        else if (strLine.Left(4) == _T("A8D8"))
        {
            if (_stscanf_s(linebufIn, _T("%s%x%x"), tmp_cmd, _countof(tmp_cmd), &dwRegAddr, &dwRegData) != 3)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][Addr = 0x%x][Data = 0x%x]"), tmp_cmd, dwRegAddr, dwRegData);
                return FALSE;
            }
            if (!m_HVSI2C.IIC_Write_Proc2(BIT8_BIT8, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData, nBoardindex))
            {
                uts.log.Error(_T("IIC_Write_Proc2 error. [I2CMode = %d][I2CAddr = 0x%x][Addr = 0x%x][Data = 0x%x]"),
                    BIT8_BIT8, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData);
                return FALSE;
            }
            Sleep(SleepTime);
        }
        else if (strLine.Left(5) == _T("A8D16"))
        {
            if (_stscanf_s(linebufIn, _T("%s%x%x"), tmp_cmd, _countof(tmp_cmd), &dwRegAddr, &dwRegData) != 3)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][Addr = 0x%x][Data = 0x%x]"), tmp_cmd, dwRegAddr, dwRegData);
                return FALSE;
            }
            if (!m_HVSI2C.IIC_Write_Proc2(BIT8_BIT16, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData, nBoardindex))
            {
                uts.log.Error(_T("IIC_Write_Proc2 error. [I2CMode = %d][I2CAddr = 0x%x][Addr = 0x%x][Data = 0x%x]"),
                    BIT8_BIT16, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData);
                return FALSE;
            }
            Sleep(SleepTime);
        }
        else if (strLine.Left(5) == _T("A16D8"))
        {
            if (_stscanf_s(linebufIn, _T("%s%x%x"), tmp_cmd, _countof(tmp_cmd), &dwRegAddr, &dwRegData) != 3)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][Addr = 0x%x][Data = 0x%x]"), tmp_cmd, dwRegAddr, dwRegData);
                return FALSE;
            }
            if (!m_HVSI2C.IIC_Write_Proc2(BIT16_BIT8, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData, nBoardindex))
            {
                uts.log.Error(_T("IIC_Write_Proc2 error. [I2CMode = %d][I2CAddr = 0x%x][Addr = 0x%x][Data = 0x%x]"),
                    BIT16_BIT8, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData);
                return FALSE;
            }
            Sleep(SleepTime);
        }
        else if (strLine.Left(6) == _T("A16D16"))
        {
            if (_stscanf_s(linebufIn, _T("%s%x%x"), tmp_cmd, _countof(tmp_cmd), &dwRegAddr, &dwRegData) != 3)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][Addr = 0x%x][Data = 0x%x]"), tmp_cmd, dwRegAddr, dwRegData);
                return FALSE;
            }
            if (!m_HVSI2C.IIC_Write_Proc2(BIT16_BIT16, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData, nBoardindex))
            {
                uts.log.Error(_T("IIC_Write_Proc2 error. [I2CMode = %d][I2CAddr = 0x%x][Addr = 0x%x][Data = 0x%x]"),
                    BIT16_BIT16, m_v5uRef.wHVS_SENSOR_SLAVEADDR, dwRegAddr, dwRegData);
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
        else if (strLine.Left(4) == _T("SLAV"))
        {
            if (_stscanf_s(linebufIn, _T("%s%x"), tmp_cmd, _countof(tmp_cmd), &dwRegData) != 2)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][Data = 0x%x]"), tmp_cmd, dwRegData);
                return FALSE;
            }
            m_v5uRef.wHVS_SENSOR_SLAVEADDR = (WORD)dwRegData;

            Sleep(10);
        }
        else if (strLine.Left(3) == _T("ENB"))
        {
            if (_stscanf_s(linebufIn, _T("%s%s"), tmp_cmd, _countof(tmp_cmd), tmp_data, _countof(tmp_data)) != 2)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][Data = %s]"), tmp_cmd, tmp_data);
                return FALSE;
            }
            tmp_idata = _ttoi(tmp_data);
            if (tmp_idata == 0)
            {
                ENB_Low();
            }
            else
            {
                ENB_High();
            }
            uts.log.Debug(_T("ENB : %d"), tmp_idata);
        }
        else if (strLine.Left(3) == _T("RES"))
        {
            if (_stscanf_s(linebufIn, _T("%s%s"), tmp_cmd, _countof(tmp_cmd), tmp_data, _countof(tmp_data)) != 2)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][Data = %s]"), tmp_cmd, tmp_data);
                return FALSE;
            }
            tmp_idata = _ttoi(tmp_data);
            if (tmp_idata == 0)
            {
                Reset_Low();
            }
            else
            {
                Reset_High();
            }
            uts.log.Debug(_T("RESET : %d"), tmp_idata);
        }
        else if (strLine.Left(3) == _T("MCL"))
        {
            if (_stscanf_s(linebufIn, _T("%s%s"), tmp_cmd, _countof(tmp_cmd), tmp_data, _countof(tmp_data)) != 2)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][Data = %s]"), tmp_cmd, tmp_data);
                return FALSE;
            }
            tmp_idata = _ttoi(tmp_data);
            if (tmp_idata == 0)
            {
                MCLK_Off();  //MCLK off
            }	
        }
        // GPIO_1 0
        else if (strLine.Left(3) == _T("GPI"))
        {
            if (_stscanf_s(linebufIn, _T("%s%d"), tmp_cmd, _countof(tmp_cmd), &GPIO_value) != 2)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][GPIO_value = %d]"), tmp_cmd, GPIO_value);
                return FALSE;
            }
            if (_stscanf_s(&linebufIn[5], _T("%d"), &GPIO_index) != 1)
            {
                uts.log.Error(_T("register format error. [GPIO_index = %d]"), GPIO_index);
                return FALSE;
            }
            if (GPIO_index < 0 || GPIO_index > 3)     //OHSK_M
            {
                uts.log.Error(_T("register format error, GPIO must be 0 ~ 3. [Cmd = %s][GPIO_value = %d][GPIO_index = %d]"),
                    tmp_cmd, GPIO_value, GPIO_index);
                return FALSE;
            }
            else
            {
                if (GPIO_value == 0)
                {
                    GPIOWrite_Low(GPIO_index, nBoardindex);	//0: Low
                }
                else
                {
                    GPIOWrite_High(GPIO_index, nBoardindex);	//1: High
                }
            }
        }
        else if (strLine.Left(4) == _T("I2CM"))
        {
            if (_stscanf_s(linebufIn, _T("%s%s"), tmp_cmd, _countof(tmp_cmd), tmp_data, _countof(tmp_data)) != 2)
            {
                uts.log.Error(_T("register format error. [Cmd = %s][Data = %s]"), tmp_cmd, tmp_data);
                return FALSE;
            }
            tmp_idata = _ttoi(tmp_data);
            m_v5uRef.HVS_SENSOR_I2CMODE = tmp_idata;
            switch (m_v5uRef.HVS_SENSOR_I2CMODE)
            {
            case BIT8_BIT8:
                uts.log.Debug(_T("I2C mode=%d (A:bit8 D:bit8)"), m_v5uRef.HVS_SENSOR_I2CMODE);
                break;
            case BIT8_BIT16:
                uts.log.Debug(_T("I2C mode=%d (A:bit8 D:bit16)"), m_v5uRef.HVS_SENSOR_I2CMODE);
                break;
            case BIT16_BIT8:
                uts.log.Debug(_T("I2C mode=%d (A:bit16 D:bit8)"), m_v5uRef.HVS_SENSOR_I2CMODE);
                break;
            case BIT16_BIT16:
                uts.log.Debug(_T("I2C mode=%d (A:bit16 D:bit16)"), m_v5uRef.HVS_SENSOR_I2CMODE);
                break;
            default:
                uts.log.Error(_T("Register I2C mode error. [mode = %d]"), m_v5uRef.HVS_SENSOR_I2CMODE);
                break;
            }
        }

        return TRUE;
    }

    int V5UDevice::IIC_Read_Proc(DWORD dwRegAddr)
    {
        return m_HVSI2C.IIC_Read_Proc(
            m_v5uRef.HVS_SENSOR_I2CMODE,
            m_v5uRef.wHVS_SENSOR_SLAVEADDR,
            dwRegAddr,
            m_v5uRef.BOARD_SET_INDEX_NUM);
    }

    BOOL V5UDevice::IIC_Write_Proc(DWORD dwRegAddr, DWORD dwRegData)
    {
        return m_HVSI2C.IIC_Write_Proc(
            m_v5uRef.HVS_SENSOR_I2CMODE,
            m_v5uRef.wHVS_SENSOR_SLAVEADDR,
            dwRegAddr,
            dwRegData,
            m_v5uRef.BOARD_SET_INDEX_NUM);
    }

    BOOL V5UDevice::i2c_write(int mode, int slave_addr, int start_addr, const char *data, int len)
    {
        return m_HVSI2C.i2c_write(
            m_v5uRef.BOARD_SET_INDEX_NUM,
            mode,
            slave_addr,
            start_addr,
            data,
            len);
    }

    BOOL V5UDevice::i2c_read(int mode, int slave_addr, int start_addr, char *data, int len)
    {
        return m_HVSI2C.i2c_read(
            m_v5uRef.BOARD_SET_INDEX_NUM,
            mode,
            slave_addr,
            start_addr,
            data,
            len);
    }

	BOOL V5UDevice::IIc_Write_Atom(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt,  unsigned char* buf)
	{
		return I2C_SYS_Write_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, slAddr<<1, nAddrLen, nAddr, nCnt, buf);

	}

	BOOL V5UDevice::IIc_Read_Atom(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf)
	{
		return I2C_SYS_Read_Mul(m_v5uRef.BOARD_SET_INDEX_NUM, slAddr<<1, nAddrLen, nAddr, nCnt, buf);
	}


    int V5UDevice::GetIICMode(void)
    {
        return m_v5uRef.HVS_SENSOR_I2CMODE;
    }

#pragma endregion

    //------------------------------------------------------------------------------
    BaseDevice* GetDevice(void)
    {
        return (new V5UDevice);
    }
    //------------------------------------------------------------------------------
}
