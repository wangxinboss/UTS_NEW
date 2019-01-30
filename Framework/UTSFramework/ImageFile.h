#pragma once
#include "Export.h"

namespace UTS
{
    //------------------------------------------------------------------------------
    class UTS_FRAMEWORK_API ImageFile
    {
    public:
        ImageFile();
        ~ImageFile();
        //-------------------------------------------------------------------------
        // Bin File
        BOOL SaveBufferToFile(
            __in LPCTSTR filename,
            __in const BYTE *pDataBuffer,
            __in int nBufferLen);
        BOOL LoadBufferFromFile(
            __in  LPCTSTR lpPath,
            __in  int nBufferLen,
            __out unsigned char *pDataBuffer);
        //-------------------------------------------------------------------------
        // RAW File
        BOOL SaveRawFile(
            __in LPCTSTR filename,
            __in const BYTE *pRawData,
            __in int dataWidth,
            __in int dataHeight);
        BOOL LoadRawFile(
            __in  LPCTSTR lpPath,
            __in  int nImageWidth,
            __in  int nImageHeight,
            __out unsigned char *pRawBuffer);
        //-------------------------------------------------------------------------
        // BMP File
        BOOL SaveBmpFile(
            __in LPCTSTR filename,
            __in const BYTE *pBmpData,
            __in int dataWidth,
            __in int dataHeight);
        BOOL LoadBmpFile(
            __in  LPCTSTR lpPath,
            __inout int &nImageWidth,
            __inout int &nImageHeight,
            __out unsigned char *pBmpBuffer,
            __in  BOOL bCheckImageSize = TRUE);
		bool WriteBMPImage(
			const wchar_t* const _fileName,
			const unsigned char* const _imageBuffer,
			const int _width,
			const int _height,
			const int _channelCount);
        //-------------------------------------------------------------------------
        // YUVBMP File
        BOOL SaveYUVBmpFile(
            __in LPCTSTR filename,
            __in const BYTE *pYUV24Buffer,
            __in int nWidth,
            __in int nHeight);
        BOOL LoadYUVBmpFile(
            __in  LPCTSTR lpPath,
            __out int &nImageWidth,
            __out int &nImageHeight,
            __out unsigned char *pYUV24Buffer);
        //-------------------------------------------------------------------------
        // JPG File
        void SaveJPG(
            __in LPCTSTR filename,
            __in const BYTE *pBmpData,
            __in int dataWidth,
            __in int dataHeight);

    };
}
