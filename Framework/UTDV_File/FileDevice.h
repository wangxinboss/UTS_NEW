#pragma once
#include "basedevice.h"

#ifdef FILEDEVICE_EXPORTS
#define FILEDEVICE_API __declspec(dllexport)
#else
#define FILEDEVICE_API __declspec(dllimport)
#endif

namespace UTS
{
    typedef enum _e_file_type_
    {
        FT_BMP = 0,
        FT_RAW,
        FT_YUVBMP,
    } eFileType;

    typedef struct _file_ref_
    {
        int nResolutionX;
        int nResolutionY;
        int nBayerMode;
        int nBatchMode;
    } FILE_REF;

    class FileDevice : public BaseDevice
    {
    public:
        FileDevice(void);
        ~FileDevice(void);

        virtual BOOL Initialize(HWND hDisplay);
        virtual BOOL Start(LPCTSTR lpRegister);
        virtual BOOL Stop();
        virtual BOOL Close();
        virtual BOOL ReadValue(eDeviceReadValueType eReadType, void *pBuffer, int nBufferSize);

        // image
        virtual BOOL GetBufferInfo(BUFFER_INFO &bufferInfo);
        virtual BOOL Recapture(TEST_BUFFER &testBuffer, int nDummyCount = 0, int nAvgCount = 1);

        // other
        virtual void ShowSettingDlg(void);

        BOOL ReadFileRef();

    private:
        BOOL LoadImageFile(LPCTSTR lpFileName);

        FILE_REF m_fileRef;
        eFileType m_fileType;

        //------------------------------------------------------------------------------
        // batch mode
        BOOL m_batchDoing;
        CStringList m_batchFileList;
        POSITION m_batchFileListPos;
    };

    extern "C"
    {
        FILEDEVICE_API BaseDevice* GetDevice(void);
    }
}
