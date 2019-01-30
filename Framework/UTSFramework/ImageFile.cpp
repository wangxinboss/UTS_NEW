#include "stdafx.h"
#include "ImageFile.h"
#include "Jpegfile.h"

namespace UTS
{
    ImageFile::ImageFile()
    {
    }

    ImageFile::~ImageFile()
    {
    }

    //-------------------------------------------------------------------------
    // Bin File
    BOOL ImageFile::SaveBufferToFile(
        __in LPCTSTR filename,
        __in const BYTE *pDataBuffer,
        __in int nBufferLen)
    {
        CString strFilename = filename;
        FILE *fp;
		//!!--- 2018.07.27 Chi-Jen.Liao
		//Old if ((fp = _tfopen(strFilename.GetBuffer(), _T("wb"))) == NULL)
        if ((fp = _tfopen(strFilename.GetBuffer(), _T("wb+"))) == NULL) 
        {
            return FALSE;
        }
        fwrite(pDataBuffer, sizeof(BYTE), nBufferLen, fp);
        fclose(fp);
        return TRUE;
    }

    BOOL ImageFile::LoadBufferFromFile(
        __in  LPCTSTR lpPath,
        __in  int nBufferLen,
        __out unsigned char *pDataBuffer)
    {
        FILE *fp = nullptr;
        USES_CONVERSION;
        CStringA strFilePathA = T2A(lpPath);

        errno_t etRet = fopen_s(&fp, strFilePathA.GetBuffer(), "rb");
        if (0 != etRet)
        {
            return FALSE;
        }
        fread(pDataBuffer, sizeof(BYTE), nBufferLen, fp);
        fclose(fp);

        return TRUE;
    }

    //-------------------------------------------------------------------------
    // RAW File
    BOOL ImageFile::SaveRawFile(
        __in LPCTSTR filename,
        __in const BYTE *pRawData,
        __in int dataWidth,
        __in int dataHeight)
    {
        CString strFilename = filename;
        strFilename.Append(_T(".Raw"));
        return SaveBufferToFile(strFilename, pRawData, dataHeight * dataWidth);
    }

    BOOL ImageFile::LoadRawFile(
        __in  LPCTSTR lpPath,
        __in  int nImageWidth,
        __in  int nImageHeight,
        __out unsigned char *pRawBuffer)
    {
        return LoadBufferFromFile(lpPath, nImageWidth * nImageHeight, pRawBuffer);
    }

    //-------------------------------------------------------------------------
    // BMP File
    BOOL ImageFile::SaveBmpFile(
        __in LPCTSTR filename,
        __in const BYTE *pBmpData,
        __in int dataWidth,
        __in int dataHeight)
    {
        int				 OffBits;
        BITMAPFILEHEADER bmpHeader; // Header for Bitmap file
        BITMAPINFO		 bmpInfo;

        OffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        DWORD dwSizeImage = dataWidth * dataHeight * 3;

        bmpHeader.bfType = ( (WORD)('M' << 8) | 'B' );  
        bmpHeader.bfSize = OffBits + dwSizeImage;
        bmpHeader.bfReserved1 = 0;
        bmpHeader.bfReserved2 = 0;
        bmpHeader.bfOffBits = OffBits;

        bmpInfo.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth           = dataWidth;
        bmpInfo.bmiHeader.biHeight          = dataHeight;
        bmpInfo.bmiHeader.biPlanes          = 1;
        bmpInfo.bmiHeader.biBitCount        = 24;
        bmpInfo.bmiHeader.biCompression     = BI_RGB;
        bmpInfo.bmiHeader.biSizeImage       = 0; 
        bmpInfo.bmiHeader.biXPelsPerMeter   = 0;
        bmpInfo.bmiHeader.biYPelsPerMeter   = 0;
        bmpInfo.bmiHeader.biClrUsed         = 0;
        bmpInfo.bmiHeader.biClrImportant    = 0;

        CString strFileName = filename;
        strFileName.Append(_T(".bmp"));

        FILE *fp = nullptr;
        errno_t nError = _tfopen_s(&fp, strFileName.GetBuffer(), _T("wb"));
        if (nError != 0)
        {
            return FALSE;
        }
        fwrite((LPSTR)&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite((LPSTR)&bmpInfo, sizeof(BITMAPINFOHEADER), 1, fp);
        fwrite((LPSTR)pBmpData, bmpHeader.bfSize - sizeof(bmpHeader) - sizeof(bmpInfo)+4, 1, fp);
        fclose(fp);

        return TRUE;	
    }

    BOOL ImageFile::LoadBmpFile(
        __in  LPCTSTR lpPath,
        __inout int &nImageWidth,
        __inout int &nImageHeight,
        __out unsigned char *pBmpBuffer,
        __in  BOOL bCheckImageSize)
    {
        BITMAPINFO bi = {0};
        BITMAPFILEHEADER bh = {0};
        LPBYTE	tmp;
        DWORD	FileSize, dwRead;

        HANDLE hFile = CreateFile(lpPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            // 文件不存在
            return FALSE;
        }

        FileSize = GetFileSize(hFile, NULL);
        tmp = (LPBYTE)malloc(FileSize);
        ReadFile(hFile, (LPBYTE)tmp, FileSize, &dwRead, NULL);
        CloseHandle(hFile);

        hFile = CreateFile(lpPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        ReadFile(hFile, (LPBYTE)(&bh), sizeof(BITMAPFILEHEADER), &dwRead, NULL);//FileSize
        CloseHandle(hFile);

        memcpy(&bi, (LPBYTE)tmp + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFO));

        if (bCheckImageSize)
        {
            if (nImageWidth != bi.bmiHeader.biWidth || nImageHeight != bi.bmiHeader.biHeight)
            {
                free(tmp);
                uts.log.Error(_T("Size Error. file = %s, nWidth = %d, nHeight = %d"),
                    lpPath, nImageWidth, nImageHeight);
                return FALSE;
            }
        }

        nImageWidth = bi.bmiHeader.biWidth;
        nImageHeight = bi.bmiHeader.biHeight;

        int bmpSize = nImageWidth * nImageHeight * 3 + nImageHeight * (nImageWidth % 4);
        memcpy(pBmpBuffer, tmp + bh.bfOffBits, bmpSize);

        free(tmp);

        return TRUE;
    }

    //-------------------------------------------------------------------------
    // YUVBMP File
    BOOL ImageFile::SaveYUVBmpFile(
        __in LPCTSTR filename,
        __in const BYTE *pYUV24Buffer,
        __in int nWidth,
        __in int nHeight)
    {
        assert(pYUV24Buffer != nullptr);

        int				 OffBits;
        BITMAPFILEHEADER bmpHeader; // Header for Bitmap file
        BITMAPINFO		 bmpInfo;

        OffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        int nSizeImage = nWidth * nHeight * 3;

        bmpHeader.bfType = ( (WORD)('M' << 8) | 'B' );  
        bmpHeader.bfSize = OffBits + nSizeImage;
        bmpHeader.bfReserved1 = 0;
        bmpHeader.bfReserved2 = 0;
        bmpHeader.bfOffBits = OffBits;

        bmpInfo.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth           = nWidth;
        bmpInfo.bmiHeader.biHeight          = nHeight;
        bmpInfo.bmiHeader.biPlanes          = 1;
        bmpInfo.bmiHeader.biBitCount        = 24;
        bmpInfo.bmiHeader.biCompression     = BI_RGB;
        bmpInfo.bmiHeader.biSizeImage       = 0; 
        bmpInfo.bmiHeader.biXPelsPerMeter   = 0;
        bmpInfo.bmiHeader.biYPelsPerMeter   = 0;
        bmpInfo.bmiHeader.biClrUsed         = 0;
        bmpInfo.bmiHeader.biClrImportant    = 0;

        CString strFileName = filename;
        strFileName.Append(_T(".yuvbmp"));

        FILE *fp = nullptr;
        errno_t nError = _tfopen_s(&fp, strFileName.GetBuffer(), _T("wb"));
        if (nError != 0)
        {
            return FALSE;
        }
        fwrite((LPSTR)&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite((LPSTR)&bmpInfo, sizeof(BITMAPINFOHEADER), 1, fp);

        BYTE *pYUVBmpBuffer = new BYTE[nSizeImage];
        // YUV = RGB => BGR = VUY
        for (int y = 0; y < nHeight; y++)
        {
            for (int x = 0; x < nWidth; x++)
            {
                int i = (y * nWidth + x) * 3;
                int j = ((nHeight - 1 - y) * nWidth + x) * 3;

                // B = V
                pYUVBmpBuffer[j + 0] = pYUV24Buffer[i + 2];
                // G = U
                pYUVBmpBuffer[j + 1] = pYUV24Buffer[i + 1];
                // R = Y
                pYUVBmpBuffer[j + 2] = pYUV24Buffer[i + 0];
            }
        }

        fwrite((LPSTR)pYUVBmpBuffer, bmpHeader.bfSize - sizeof(bmpHeader) - sizeof(bmpInfo) + 4, 1, fp);
        fclose(fp);

        RELEASE_ARRAY(pYUVBmpBuffer);
        return TRUE;	
    }

    BOOL ImageFile::LoadYUVBmpFile(
        __in  LPCTSTR lpPath,
        __out int &nWidth,
        __out int &nHeight,
        __out unsigned char *pYUV24Buffer)
    {
        BITMAPINFO bi = {0};
        BITMAPFILEHEADER bh = {0};
        LPBYTE	tmp;
        DWORD	FileSize, dwRead;

        HANDLE hFile = CreateFile(lpPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            // 文件不存在
            return FALSE;
        }

        FileSize = GetFileSize(hFile, NULL);
        tmp = (LPBYTE)malloc(FileSize);
        ReadFile(hFile, (LPBYTE)tmp, FileSize, &dwRead, NULL);
        CloseHandle(hFile);

        hFile = CreateFile(lpPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        ReadFile(hFile, (LPBYTE)(&bh), sizeof(BITMAPFILEHEADER), &dwRead, NULL);//FileSize
        CloseHandle(hFile);

        memcpy(&bi, (LPBYTE)tmp + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFO));

        nWidth = bi.bmiHeader.biWidth;
        nHeight = bi.bmiHeader.biHeight;

        int nSizeImage = nWidth * nHeight * 3;
        int bmpSize = nSizeImage + nHeight * (nWidth % 4);
        BYTE *pYUVBmpBuffer = new BYTE[nSizeImage];
        memcpy(pYUVBmpBuffer, tmp + bh.bfOffBits, bmpSize);

        // YUV = RGB => BGR = VUY
        for (int y = 0; y < nHeight; y++)
        {
            for (int x = 0; x < nWidth; x++)
            {
                int i = (y * nWidth + x) * 3;
                int j = ((nHeight - 1 - y) * nWidth + x) * 3;

                // V = B
                pYUV24Buffer[i + 2] = pYUVBmpBuffer[j + 0];
                // U = G
                pYUV24Buffer[i + 1] = pYUVBmpBuffer[j + 1];
                // Y = R
                pYUV24Buffer[i + 0] = pYUVBmpBuffer[j + 2];
            }
        }
        free(tmp);
        RELEASE_ARRAY(pYUVBmpBuffer);

        return TRUE;
    }

    //-------------------------------------------------------------------------
    // JPG File
    void ImageFile::SaveJPG(
        __in LPCTSTR filename,
        __in const BYTE *pBmpData,
        __in int dataWidth,
        __in int dataHeight)
    {
        JpegFile *pJpgFile = new JpegFile;
        BYTE *pTmpBuffer = new BYTE[dataWidth * dataHeight * 3];
        memcpy(pTmpBuffer, pBmpData, sizeof(BYTE) * dataWidth * dataHeight * 3);

        pJpgFile->BGRFromRGB(pTmpBuffer, dataHeight, dataWidth);
        pJpgFile->VertFlipBuf(pTmpBuffer, dataWidth*3, dataHeight);
        USES_CONVERSION;
        pJpgFile->RGBToJpegFile(T2A(filename), pTmpBuffer, dataWidth, dataHeight, TRUE, 90);

        RELEASE_ARRAY(pTmpBuffer);
        RELEASE_POINTER(pJpgFile);
    }

	bool ImageFile::WriteBMPImage(const wchar_t* const _fileName,const unsigned char* const _imageBuffer,const int _width,const int _height,const int _channelCount)
	{
		bool bRes = true;

		LONG lWidth = _width;
		LONG lHeight = _height;

		WORD wBitsPerPixel = _channelCount*8;
		int BitCount = _channelCount;
		int ScanLineSize = 0;
		double ModValue = int(fmod(double(_width*BitCount),4));
		if (ModValue == 0)
		{
			ScanLineSize = (_width)*BitCount;
		}
		else
		{
			ScanLineSize = (_width)*BitCount+int((ModValue == 0)? (0):(4-ModValue));
		}

		unsigned char* pBitmapBits = new unsigned char[ScanLineSize*(_height)];
		memset(pBitmapBits,0,ScanLineSize*(_height));

		for (int y=0;y<_height;y++)
		{
			for (int x=0;x<_width;x++)
			{
				for (int i=0;i<BitCount;i++)
				{
					pBitmapBits[(_height-1-y)*ScanLineSize+x*BitCount+i] = _imageBuffer[(y*_width+x)*(BitCount)+(BitCount-i-1)];
				}
			}
		}

		BITMAPINFOHEADER bmpInfoHeader = {0};
		// Set the size
		bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		// Bit count
		bmpInfoHeader.biBitCount = wBitsPerPixel;
		// Use all colors
		bmpInfoHeader.biClrImportant = 0;
			
		// Use as many colors according to bits per pixel
		bmpInfoHeader.biClrUsed = 0;
			
		// Store as Uncompressed
		bmpInfoHeader.biCompression = BI_RGB;
		// Set the height in pixels
		bmpInfoHeader.biHeight = lHeight;
		// Width of the Image in pixels
		bmpInfoHeader.biWidth = lWidth;
		// Default number of planes
		bmpInfoHeader.biPlanes = 1;
		// Calculate the image size in bytes
		bmpInfoHeader.biSizeImage = ScanLineSize*(_height); 

		BITMAPFILEHEADER bfh = {0};
		// This value should be values of BM letters i.e 0×4D42
		// 0×4D = M 0×42 = B storing in reverse order to match with endian
		bfh.bfType = 0x4D42;
		/* or
		bfh.bfType = ‘B’+(’M’ << 8);
		// <<8 used to shift ‘M’ to end
		*/
		if (bmpInfoHeader.biBitCount == 8)
		{
			// Offset to the RGBQUAD
			bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) +256 * sizeof(RGBQUAD);
			// Total size of image including size of headers
			//bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;
			bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSize + /*bmpInfoHeader.biClrUsed*/256 * sizeof(RGBQUAD) + bmpInfoHeader.biSizeImage; 
		}
		else
		{
			// Offset to the RGBQUAD
			bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) ;
			// Total size of image including size of headers
			//bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;
			bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSize  + bmpInfoHeader.biSizeImage; 
		}
		// Create the file in disk to write
		HANDLE hFile = CreateFile(_fileName,GENERIC_WRITE, 0,NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL); 

		if( !hFile ) // return if error opening file
		{
			bRes = false;
			delete[] pBitmapBits;
			pBitmapBits = 0;
			return bRes;
		} 

		DWORD dwWritten = 0;
		// Write the File header
		WriteFile( hFile, &bfh, sizeof(bfh), &dwWritten , NULL );
		// Write the bitmap info header
		WriteFile( hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwWritten, NULL );

		// Check for palettized bitmap
		if (bmpInfoHeader.biBitCount == 8)
		{
			RGBQUAD lpPalette[256] ={0};
				 
			for (int i=0;i<256;i++)
			{
				lpPalette[i].rgbBlue = BYTE(i);
				lpPalette[i].rgbGreen = BYTE(i);
				lpPalette[i].rgbRed = BYTE(i);
				lpPalette[i].rgbReserved = 0;
			}

			// Write palette info
			//BOOL bResult3 = ReadFile(hFile, m_lpPalette, sizeof(RGBQUAD)*256, &nBytesRead, NULL );
			WriteFile( hFile,lpPalette, sizeof(RGBQUAD)*256, &dwWritten, NULL );
			//fwrite(m_lpPalette, m_iPaletteEntries, sizeof(RGBQUAD), file);
		}

		// Write the RGB Data
		WriteFile( hFile, pBitmapBits, bmpInfoHeader.biSizeImage, &dwWritten, NULL );
		// Close the file handle
		CloseHandle( hFile );

		delete[] pBitmapBits;
		pBitmapBits = 0;

		return bRes;
	}
}
