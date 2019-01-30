#include "StdAfx.h"
#include "FileDevice.h"
#include "Algorithm.h"
#include "FileDeviceSettingDlg.h"
#include "UTDV_File.h"

#pragma comment(lib, "UTSAlgorithm.lib")

using namespace UTS::Algorithm::Image;

namespace UTS
{
    FileDevice::FileDevice(void)
    {
        m_fileType = eFileType::FT_BMP;
        m_batchDoing = FALSE;
    }

    FileDevice::~FileDevice(void)
    {
    }

    BOOL FileDevice::Initialize(HWND hDisplay)
    {
        m_dLowlevelFPS = m_lowLevelFpsCounter.GetFPS();
        m_dDisplayFPS = m_displayFpsCounter.GetFPS();
        m_hDisplay = hDisplay;

        BOOL bRet = ReadFileRef();
        if (!bRet)
        {
            uts.log.Error(_T("[%s]ReadFileRef error!"), UTS_FN);
            return FALSE;
        }

        m_nWidth = m_fileRef.nResolutionX;
        m_nHeight = m_fileRef.nResolutionY;

        //------------------------------------------------------------------------------
        // 分配空间
        CreateTestBufferMemory();

        return TRUE;
    }

    BOOL FileDevice::Start(LPCTSTR lpRegister)
    {
        UNREFERENCED_PARAMETER(lpRegister);

        // 如果分辨率变化，重新分配空间
        if (m_nWidth != m_fileRef.nResolutionX || m_nHeight != m_fileRef.nResolutionY)
        {
            ReleaseTestBufferMemory();
            m_nWidth = m_fileRef.nResolutionX;
            m_nHeight = m_fileRef.nResolutionY;
            CreateTestBufferMemory();
        }
        else
        {
            m_nWidth = m_fileRef.nResolutionX;
            m_nHeight = m_fileRef.nResolutionY;
        }

        CString strFileToHandle;
        if (m_fileRef.nBatchMode == 0)
        {
            //------------------------------------------------------------------------------
            // 弹出选择单个文件的对话框
            TCHAR szFileName[MAX_PATH] = {0};
            OPENFILENAME SFN;
            memset(&SFN, 0, sizeof(OPENFILENAME));
            SFN.lStructSize = sizeof(OPENFILENAME);
            SFN.hwndOwner = uts.info.hMainWnd;
            SFN.lpstrFilter = _T("Image File(*.bmp;*.raw;*.yuvbmp)\0*.bmp;*.raw;*.yuvbmp\0\0");
            SFN.lpstrFile = szFileName;
            SFN.nMaxFile = MAX_PATH;
            SFN.Flags = OFN_FILEMUSTEXIST;
            if (!GetOpenFileName(&SFN))
            {
                uts.log.Error(_T("Did not select file."));

                //-------------------------------------------------------------------------
                // fix bug #1: 批处理图片后不能退出
                uts.board.ShowMsg(_T("Press Space"));
                uts.keyboard.BeginListen();
                uts.keyboard.WaitSpace(&uts.info.bAppRunning);
                uts.keyboard.EndListen();
                uts.board.ShowMsg(EMPTY_STR);
                //-------------------------------------------------------------------------

                return FALSE;
            }
            strFileToHandle = szFileName;
        }
        else
        {
            if (!m_batchDoing)
            {
                //------------------------------------------------------------------------------
                // 弹出选择文件夹的对话框
                TCHAR szFolderPath[MAX_PATH] = {0};  
                BROWSEINFO bi;   
                bi.hwndOwner      = uts.info.hMainWnd;
                bi.pidlRoot       = nullptr;
                bi.pszDisplayName = nullptr;
                bi.lpszTitle      = TEXT("Please Select Folder:");
                bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
                bi.lpfn           = NULL;
                bi.lParam         = 0;
                bi.iImage         = 0;
                LPITEMIDLIST pidl = SHBrowseForFolder(&bi);  
                if (pidl == NULL)  
                {
                    uts.log.Error(_T("Did not select folder."));

                    //-------------------------------------------------------------------------
                    // fix bug #1: 批处理图片后不能退出
                    uts.board.ShowMsg(_T("Press Space"));
                    uts.keyboard.BeginListen();
                    uts.keyboard.WaitSpace(&uts.info.bAppRunning);
                    uts.keyboard.EndListen();
                    uts.board.ShowMsg(EMPTY_STR);
                    //-------------------------------------------------------------------------

                    return FALSE; 
                }

                if (!SHGetPathFromIDList(pidl, szFolderPath))
                {
                    uts.log.Error(_T("Get folder path failed."));
                    return FALSE; 
                }
                
                //------------------------------------------------------------------------------
                // 取得文件夹下的图片文件列表
                m_batchFileList.RemoveAll();
                CStringList fileList;
                UTS::OSUtil::GetFiles(szFolderPath, fileList);
                POSITION rPos = fileList.GetHeadPosition();
                while (rPos != nullptr)
                {
                    CString strFilePath = fileList.GetNext(rPos);
                    CString strFileExt = UTS::OSUtil::GetExtFileName(strFilePath).MakeUpper();
                    if (strFileExt == _T("BMP") || strFileExt == _T("RAW") || strFileExt == _T("YUV24"))
                    {
                        m_batchFileList.AddTail(strFilePath);
                    }
                }

                //------------------------------------------------------------------------------
                // 取得下一个文件名
                m_batchFileListPos = m_batchFileList.GetHeadPosition();
                if (m_batchFileListPos != nullptr)
                {
                    strFileToHandle = m_batchFileList.GetNext(m_batchFileListPos);
                }
                else
                {
                    return TRUE;
                }
                m_batchDoing = TRUE;
            }
            else
            {
                if (m_batchFileListPos != nullptr)
                {
                    strFileToHandle = m_batchFileList.GetNext(m_batchFileListPos);
                    if (m_batchFileListPos == nullptr)
                    {
                        m_batchDoing = FALSE;
                    }
                }
            }
        }

        //------------------------------------------------------------------------------
        // 读取图片
        if (!LoadImageFile(strFileToHandle))
        {
            uts.log.Error(_T("LoadImageFile Error. file = %s"), strFileToHandle);
            return FALSE;
        }
        else
        {
            uts.log.Info(_T("Load file [%s]"), strFileToHandle);
        }

        //------------------------------------------------------------------------------
        // 解析SN
        CString strSn = UTS::OSUtil::GetFileName(strFileToHandle);
#if 1
        strSn = strFileToHandle.Right(strSn.GetLength() - strSn.Find('-') - 1);
        strSn = strSn.Left(strSn.Find('-'));
#else
        strSn = strFileToHandle.Right(strSn.GetLength() - strSn.Find('_') - 1);
        strSn = strSn.Left(strSn.Find('.'));
#endif
        uts.info.strSN = strSn;

        return TRUE;
    }

    BOOL FileDevice::Stop()
    {
        //-------------------------------------------------------------------------
        // fix bug #5 产品点不亮但是preview后会出现画面
        ClearTestBufferMemory();
        //-------------------------------------------------------------------------
        return TRUE;
    }

    BOOL FileDevice::Close()
    {
        ReleaseTestBufferMemory();
        return TRUE;
    }
    
    BOOL FileDevice::ReadValue(eDeviceReadValueType eReadType, void *pBuffer, int nBufferSize)
    {
        assert(nullptr != pBuffer);
        assert(nBufferSize > 0);

        BOOL bRet = TRUE;
        int nValue = 0;
        switch (eReadType)
        {
        case eDeviceReadValueType::DRVT_SENSOR_OUT_MODE:
            if (nBufferSize >= sizeof(int))
            {
                nValue = m_fileRef.nBayerMode;
                memcpy(pBuffer, &nValue, sizeof(int));
            }
            else
            {
                bRet = FALSE;
            }
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

    BOOL FileDevice::GetBufferInfo(BUFFER_INFO &bufferInfo)
    {
        bufferInfo.nWidth = m_nWidth;
        bufferInfo.nHeight = m_nHeight;
         bufferInfo.dwBufferType = 0;

        switch (m_fileType)
        {
        case eFileType::FT_BMP:
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_BMP;
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_YUV24;
            break;
        case eFileType::FT_RAW:
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_RAW8;
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_BMP;
            //bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_YUV24;
            break;
        case eFileType::FT_YUVBMP:
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_YUV24;
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_BMP;
            break;
        default:
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_BMP;
            bufferInfo.dwBufferType |= BUFFER_TYPE_MASK_YUV24;
            break;
        }
        return TRUE;
    }

    BOOL FileDevice::Recapture(TEST_BUFFER &testBuffer, int nDummyCount/* = 0*/, int nAvgCount/* = 1*/)
    {
        UNREFERENCED_PARAMETER(nDummyCount);
        UNREFERENCED_PARAMETER(nAvgCount);

        memcpy(&testBuffer, &m_TestBuffer, sizeof(TEST_BUFFER));

        return TRUE;
    }

    void FileDevice::ShowSettingDlg(void)
    {
        //AFX_MANAGE_STATE(AfxGetStaticModuleState());    // 切换为调用本DLL资源
        HINSTANCE save_hInstance = AfxGetResourceHandle();
        AfxSetResourceHandle(theApp.m_hInstance);

        CFileDeviceSettingDlg dlg;
        dlg.DoModal();

        AfxSetResourceHandle(save_hInstance);
    }

    BOOL FileDevice::LoadImageFile(LPCTSTR lpFileName)
    {
        //------------------------------------------------------------------------------
        // 根据文件类型，读入Buffer
        CString strExt = UTS::OSUtil::GetExtFileName(lpFileName).MakeUpper();
        if (strExt == _T("BMP"))
        {
            int nWidth = m_nWidth;
            int nHeight = m_nHeight;
            m_fileType = eFileType::FT_BMP;
			uts.imgFile.LoadBmpFile(
			lpFileName,
			nWidth,
			nHeight,
			m_TestBuffer.pFrameBuffer,FALSE);

			m_nWidth = nWidth;
			m_nHeight = nHeight;

            memcpy(m_TestBuffer.pBmpBuffer, m_TestBuffer.pFrameBuffer, m_nWidth * m_nHeight * 3);
            
			CImageProc::GetInstance().BufferConv_BmpToYUV24(
                m_TestBuffer.pBmpBuffer,
                nWidth,
                nHeight,
                0,
                m_TestBuffer.pYuv24Buffer);
        }
        else if (strExt == _T("RAW"))
        {
            m_fileType = eFileType::FT_RAW;
            if (!uts.imgFile.LoadRawFile(
                lpFileName,
                m_nWidth*2,
                m_nHeight,
                m_TestBuffer.pFrameBuffer))
            {
                uts.log.Error(_T("LoadRawFile Error. file = %s"), lpFileName);
                return FALSE;
            }
            if (!CImageProc::GetInstance().GetRaw8AndBmpBuffer(
                m_TestBuffer.pFrameBuffer,
                m_nWidth,
                m_nHeight,
                eDataFormat::Raw10Bit,
                m_fileRef.nBayerMode,
                eMipiMode::Parallel,
                m_TestBuffer.pRaw8Buffer,
                m_TestBuffer.pBmpBuffer))
            {
                uts.log.Error(_T("GetRaw8AndBmpBuffer Error."));
                return FALSE;
            }
        }
        else if (strExt == _T("YUVBMP"))
        {
            m_fileType = eFileType::FT_YUVBMP;
            if (!uts.imgFile.LoadYUVBmpFile(
                lpFileName,
                m_nWidth,
                m_nHeight,
                m_TestBuffer.pFrameBuffer))
            {
                uts.log.Error(_T("LoadYUVFile Error. file = %s"), lpFileName);
                return FALSE;
            }
            memcpy(m_TestBuffer.pYuv24Buffer, m_TestBuffer.pFrameBuffer, m_nWidth * m_nHeight * 3);
            CImageProc::GetInstance().BufferConv_YUV24ToBmp(
                m_TestBuffer.pYuv24Buffer,
                m_nWidth,
                m_nHeight,
                0,
                m_TestBuffer.pBmpBuffer);

#if 0
            unsigned char *pY = new unsigned char[m_nWidth * m_nHeight];
            unsigned char *pU = new unsigned char[m_nWidth * m_nHeight];
            unsigned char *pV = new unsigned char[m_nWidth * m_nHeight];
            for (int i = 0; i < m_nWidth * m_nHeight; i++)
            {
                pV[i] = m_TestBuffer.pYuv24Buffer[3*i];
                pU[i] = m_TestBuffer.pYuv24Buffer[3*i+1];
                pY[i] = m_TestBuffer.pYuv24Buffer[3*i+2];
            }
            memcpy(m_TestBuffer.pYuv24Buffer, pY, m_nWidth * m_nHeight);
            memcpy(m_TestBuffer.pYuv24Buffer + m_nWidth * m_nHeight, pU, m_nWidth * m_nHeight);
            memcpy(m_TestBuffer.pYuv24Buffer + m_nWidth * m_nHeight * 2, pV, m_nWidth * m_nHeight);
            uts.imgFile.SaveYuv24File(CString(lpFileName) + L"_", m_TestBuffer.pYuv24Buffer,
                m_nWidth,
                m_nHeight);
            RELEASE_ARRAY(pY);
            RELEASE_ARRAY(pU);
            RELEASE_ARRAY(pV);
#endif
        }
        else
        {
            uts.log.Error(_T("File Type Error. file = %s"), lpFileName);
            return FALSE;
        }
        return TRUE;
    }

    BOOL FileDevice::ReadFileRef()
    {
        //------------------------------------------------------------------------------
        // 获取param
        CString strSection;
        if (!uts.dbCof.GetDeviceFilename(uts.info.nDeviceId, strSection))
        {
            uts.log.Error(_T("[%s]GetDeviceFilename Fail. nDeviceId = %d"), UTS_FN, uts.info.nDeviceId);
            return FALSE;
        }
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("ResolutionX"), m_fileRef.nResolutionX, 640, _T("ResolutionX"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("ResolutionY"), m_fileRef.nResolutionY, 480, _T("ResolutionY"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("BayerMode"), m_fileRef.nBayerMode, (int)eBayerOutputMode::BGGR, _T("1:BGGR  2:RGGB  3:GBRG  4:GRBG  5:BlackWhite"));
        uts.dbCof.GetDeviceSingleSetting(strSection, _T("BatchMode"), m_fileRef.nBatchMode, 0, _T("Read multiple file mode. 0: Close 1: Open"));
        return TRUE;
    }

    //------------------------------------------------------------------------------
    BaseDevice* GetDevice(void)
    {
        return (new FileDevice);
    }
    //------------------------------------------------------------------------------
}
