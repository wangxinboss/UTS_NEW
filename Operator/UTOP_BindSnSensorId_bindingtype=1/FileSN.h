#pragma once

class FileSN
{
public:
    FileSN();
    virtual bool Initialize(void);
    virtual bool GetNextSn(CString &sn);

private:
    CString m_snFile;
};
