#include "StdAfx.h"
#include "BaseDevice.h"

namespace UTS
{
    BaseDevice::BaseDevice(void)
    {
        m_TestBuffer.pFrameBuffer = nullptr;
        m_TestBuffer.pBmpBuffer = nullptr;
        m_TestBuffer.pRaw8Buffer = nullptr;
        m_TestBuffer.pYuv24Buffer = nullptr;
        m_TestBuffer.pYBuffer = nullptr;
    }
    
    BaseDevice::~BaseDevice(void)
    {
    }

    BOOL BaseDevice::ReadValue(eDeviceReadValueType eReadType, void *pBuffer, int nBufferSize)
    {
        // 继承的Device如果不重写此方法，返回成功。
        return TRUE;
    }

    BOOL BaseDevice::WriteValue(eDeviceWriteValueType eWriteType, void *pBuffer, int nBufferSize)
    {
        // 继承的Device如果不重写此方法，返回成功。
        return TRUE;
    }

    void BaseDevice::GetCapturedBuffer(TEST_BUFFER &testBuffer)
    {
        memcpy(&testBuffer, &m_TestBuffer, sizeof(TEST_BUFFER));
    }

    void BaseDevice::DisplayImage(unsigned char *pBmpBuffer, const UI_MARK* pUiMark)
    {
        m_dDisplayFPS = m_displayFpsCounter.GetFPS();

        BITMAPINFO	bmpInfo;
        bmpInfo.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER) ;
        bmpInfo.bmiHeader.biWidth           = m_nWidth;
        bmpInfo.bmiHeader.biHeight          = m_nHeight;
        bmpInfo.bmiHeader.biPlanes          = 1;
        bmpInfo.bmiHeader.biBitCount        = 24;
        bmpInfo.bmiHeader.biCompression     = BI_RGB ;
        bmpInfo.bmiHeader.biSizeImage       = 0;
        bmpInfo.bmiHeader.biXPelsPerMeter   = 0;
        bmpInfo.bmiHeader.biYPelsPerMeter   = 0;
        bmpInfo.bmiHeader.biClrUsed         = 0;
        bmpInfo.bmiHeader.biClrImportant    = 0;

        CRect rcWindow;
        GetWindowRect(m_hDisplay, &rcWindow);

        CDC dc;
        HDC hdc = GetWindowDC(m_hDisplay);
        dc.Attach(hdc);

        CDC memdc;
        memdc.CreateCompatibleDC(NULL);
        CBitmap memBitmap;
        memBitmap.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
        CBitmap *pOldBit = memdc.SelectObject(&memBitmap);

        //------------------------------------------------------------------------------
        // draw memdc start
        //------------------------------------------------------------------------------
        memdc.SetStretchBltMode(COLORONCOLOR);
        StretchDIBits(
            memdc.m_hDC,
            0,
            0,
            rcWindow.Width(),
            rcWindow.Height(),
            0,
            0,
            m_nWidth,
            m_nHeight,
            pBmpBuffer,
            (LPBITMAPINFO)&bmpInfo,
            DIB_RGB_COLORS,
            SRCCOPY);

        double dScaleX = (double)(rcWindow.Width()) / (double)m_nWidth;
        double dScaleY = (double)(rcWindow.Height()) / (double)m_nHeight;
        //------------------------------------------------------------------------------
        // draw FPS
        if (uts.info.bShowFPS)
        {
            CString strFPS;
            strFPS.Format(_T("%dx%d LFPS: %.1f DFPS: %.1f"),
                m_nWidth, m_nHeight, m_dLowlevelFPS, m_dDisplayFPS);
            memdc.SetBkMode(TRANSPARENT);
            memdc.SetTextColor(COLOR_YELLOW);
            memdc.TextOut(0, 0, strFPS.GetBuffer(), strFPS.GetLength());
        }

        //------------------------------------------------------------------------------
        // draw UI Mark
        if (nullptr != pUiMark)
        {
            // draw line
            vector<UI_LINE>::const_iterator iterLine = pUiMark->vecUiLine.begin();
            for (; iterLine != pUiMark->vecUiLine.end(); iterLine++)
            {
                CPoint ptStart = (*iterLine).ptBegin;
                ptStart.x = DOUBLE2LONG(ptStart.x * dScaleX);
                ptStart.y = DOUBLE2LONG(ptStart.y * dScaleY);
                CPoint ptEnd = (*iterLine).ptEnd;
                ptEnd.x = DOUBLE2LONG(ptEnd.x * dScaleX);
                ptEnd.y = DOUBLE2LONG(ptEnd.y * dScaleY);

                CPen pen;
                pen.CreatePen(PS_SOLID, 1, (*iterLine).color);
                CPen *pOldpen = memdc.SelectObject(&pen);

                memdc.MoveTo(ptStart);
                memdc.LineTo(ptEnd);

                memdc.SelectObject(pOldpen);
                pen.DeleteObject();
            }
            // draw rect
            vector<UI_RECT>::const_iterator iterRect = pUiMark->vecUiRect.begin();
            for (; iterRect != pUiMark->vecUiRect.end(); iterRect++)
            {
                CRect rcRect = (*iterRect).rcPos;
                rcRect.left  = DOUBLE2LONG(rcRect.left  * dScaleX);
                rcRect.right = DOUBLE2LONG(rcRect.right * dScaleX);
                rcRect.top   = DOUBLE2LONG(rcRect.top   * dScaleY);
                rcRect.bottom= DOUBLE2LONG(rcRect.bottom* dScaleY);

                CPen pen;
                pen.CreatePen(PS_SOLID, 1, (*iterRect).color);
                CPen *pOldpen = memdc.SelectObject(&pen);

                HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                HBRUSH hOldBrush = (HBRUSH)(memdc.SelectObject(hBrush));

                memdc.Rectangle(&rcRect);

                memdc.SelectObject(hOldBrush);
                ::DeleteObject(hBrush);
                memdc.SelectObject(pOldpen);
                pen.DeleteObject();
            }
            // draw circle
			
            vector<UI_CIRCLE>::const_iterator iterCircle = pUiMark->vecUiCircle.begin();
            for (; iterCircle != pUiMark->vecUiCircle.end(); iterCircle++)
            {
                CRect rcRect;
                rcRect.left  = DOUBLE2LONG(((*iterCircle).ptCenter.x - (*iterCircle).dRadius) * dScaleX);
                rcRect.right = DOUBLE2LONG(((*iterCircle).ptCenter.x + (*iterCircle).dRadius) * dScaleX);
                rcRect.top   = DOUBLE2LONG(((*iterCircle).ptCenter.y - (*iterCircle).dRadius) * dScaleY);
                rcRect.bottom= DOUBLE2LONG(((*iterCircle).ptCenter.y + (*iterCircle).dRadius) * dScaleY);

                CPen pen;
                pen.CreatePen(PS_SOLID, 1, (*iterCircle).color);
                CPen *pOldpen = memdc.SelectObject(&pen);

                HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                HBRUSH hOldBrush = (HBRUSH)(memdc.SelectObject(hBrush));

                memdc.Ellipse(&rcRect);

                memdc.SelectObject(hOldBrush);
                ::DeleteObject(hBrush);
                memdc.SelectObject(pOldpen);
                pen.DeleteObject();
            }
			
            // draw text
			CFont fontSN;
			fontSN.CreatePointFont(180, _T("Arial Black"), NULL);
			CFont *pOldfont = memdc.SelectObject(&fontSN);
			memdc.SelectObject(&fontSN);
			fontSN.DeleteObject();

			memdc.SetBkMode(TRANSPARENT);
            vector<UI_TEXT>::const_iterator iterText = pUiMark->vecUIText.begin();
            for (; iterText != pUiMark->vecUIText.end(); iterText++)
            {
                CPoint pt = (*iterText).ptPos;
                pt.x = DOUBLE2LONG(pt.x * dScaleX);
                pt.y = DOUBLE2LONG(pt.y * dScaleY);
                memdc.SetTextColor((*iterText).color);
                memdc.TextOut(pt.x, pt.y, (*iterText).strText);
            }
			
            vector<UI_TEXT_REL>::const_iterator iterTextRel = pUiMark->vecUITextRel.begin();
            for (; iterTextRel != pUiMark->vecUITextRel.end(); iterTextRel++)
            {
                int x = DOUBLE2LONG((*iterTextRel).dWidthPersent * m_nWidth * dScaleX);
                int y = DOUBLE2LONG((*iterTextRel).dHeightPersent * m_nHeight * dScaleY);
                memdc.SetTextColor((*iterTextRel).color);
                memdc.TextOut(x, y, (*iterTextRel).strText);
            }

            // draw point
			
            vector<UI_POINT>::const_iterator iterPoint = pUiMark->vecUiPoint.begin();
            for (; iterPoint != pUiMark->vecUiPoint.end(); iterPoint++)
            {
                CPoint pt = (*iterPoint).ptPos;
                pt.x = DOUBLE2LONG(pt.x * dScaleX);
                pt.y = DOUBLE2LONG(pt.y * dScaleY);
                memdc.FillSolidRect(
                    pt.x - 1,
                    pt.y - 1,
                    3,
                    3,
                    (*iterPoint).color);
            }
			
        }
        //------------------------------------------------------------------------------
        // draw memdc end
        //------------------------------------------------------------------------------

        dc.BitBlt(
            0,
            0,
            rcWindow.Width(),
            rcWindow.Height(),
            &memdc,
            0,
            0,
            SRCCOPY);

        memdc.SelectObject(pOldBit);
        memdc.DeleteDC();
        memBitmap.DeleteObject();
        dc.Detach();
        ReleaseDC(m_hDisplay, hdc);
    }

    void BaseDevice::ShowSettingDlg(void)
    {
        AfxMessageBox(_T("No device setting need set."));
    }

    //-------------------------------------------------------------------------
    // fix 改善 #2: Snap功能
    void BaseDevice::SnapImage(void)
    {
        //------------------------------------------------------------------------------
        // 弹出选择单个文件的对话框
        TCHAR szFileName[MAX_PATH] = {0};
        OPENFILENAME SFN;
        memset(&SFN, 0, sizeof(OPENFILENAME));
        SFN.lStructSize = sizeof(OPENFILENAME);
        SFN.hwndOwner = uts.info.hMainWnd;
        SFN.lpstrFilter = _T("bmp file(*.bmp)\0*.bmp\0jpg file(*.jpg)\0*.jpg\0raw file(*.raw)\0*.raw\0yuvbmp file(*.yuvbmp)\0*.yuvbmp\0");
        SFN.lpstrFile = szFileName;
        SFN.nMaxFile = MAX_PATH;
        if (!GetSaveFileName(&SFN))
        {
            uts.log.Error(_T("Did not select file."));
            return;
        }
        if (OSUtil::IsFileExist(szFileName))
        {
            CString strMsg;
            strMsg.Format(_T("%s is already exists. Do you want replace it?"), szFileName);
            int nRet = AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION);
            if (nRet == IDNO)
            {
                return;
            }
        }
        
        CString strFileName = szFileName;
        if (SFN.nFilterIndex == 1) // SAVE BMP
        {
            if (nullptr != m_TestBuffer.pBmpBuffer)
            {
                uts.imgFile.SaveBmpFile(strFileName, m_TestBuffer.pBmpBuffer, m_nWidth, m_nHeight);
            }
        }
        else if (SFN.nFilterIndex == 2) // SAVE JPG
        {
            strFileName += _T(".jpg");
            if (nullptr != m_TestBuffer.pBmpBuffer)
            {
                uts.imgFile.SaveJPG(strFileName, m_TestBuffer.pBmpBuffer, m_nWidth, m_nHeight);
            }
        }
        else if (SFN.nFilterIndex == 3) // SAVE RAW
        {
            if (nullptr != m_TestBuffer.pRaw8Buffer)
            {
                uts.imgFile.SaveRawFile(strFileName, m_TestBuffer.pRaw8Buffer, m_nWidth, m_nHeight);
            }
        }
        else if (SFN.nFilterIndex == 4) // SAVE YUVBMP
        {
            if (nullptr != m_TestBuffer.pYuv24Buffer)
            {
                uts.imgFile.SaveYUVBmpFile(strFileName, m_TestBuffer.pYuv24Buffer, m_nWidth, m_nHeight);
            }
        }
    }
    //-------------------------------------------------------------------------

    void BaseDevice::CreateTestBufferMemory()
    {
        int nBufferSize = m_nWidth * m_nHeight;
        int nBufferSize3 = nBufferSize * 3;
        m_TestBuffer.pFrameBuffer = new unsigned char[nBufferSize3];
        m_TestBuffer.pBmpBuffer = new unsigned char[nBufferSize3];
        m_TestBuffer.pRaw8Buffer = new unsigned char[nBufferSize];
        m_TestBuffer.pYuv24Buffer = new unsigned char[nBufferSize3];
        m_TestBuffer.pYBuffer = new unsigned char[nBufferSize];
        ClearTestBufferMemory();
    }

    void BaseDevice::ClearTestBufferMemory()
    {
        int nBufferSize = m_nWidth * m_nHeight;
        int nBufferSize3 = nBufferSize * 3;
        if (nullptr != m_TestBuffer.pFrameBuffer)
        {
            memset(m_TestBuffer.pFrameBuffer, 0, nBufferSize3);
        }
        if (nullptr != m_TestBuffer.pBmpBuffer)
        {
            memset(m_TestBuffer.pBmpBuffer, 0, nBufferSize3);
        }
        if (nullptr != m_TestBuffer.pRaw8Buffer)
        {
            memset(m_TestBuffer.pRaw8Buffer, 0, nBufferSize);
        }
        if (nullptr != m_TestBuffer.pYuv24Buffer)
        {
            memset(m_TestBuffer.pYuv24Buffer, 0, nBufferSize3);
        }
        if (nullptr != m_TestBuffer.pYBuffer)
        {
            memset(m_TestBuffer.pYBuffer, 0, nBufferSize);
        }
    }
    
    void BaseDevice::ReleaseTestBufferMemory()
    {
        RELEASE_ARRAY(m_TestBuffer.pFrameBuffer);
        RELEASE_ARRAY(m_TestBuffer.pBmpBuffer);
        RELEASE_ARRAY(m_TestBuffer.pRaw8Buffer);
        RELEASE_ARRAY(m_TestBuffer.pYuv24Buffer);
        RELEASE_ARRAY(m_TestBuffer.pYBuffer);
    }

}
