#include "stdafx.h"
#include "UTSDefine.h"
#include "VCM.h"
#include "VCM_I2C.h"

namespace UTS
{
	BOOL I2CWrite(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf)
	{
		unsigned char writeBuffer[256] = {0};
		int nUseSize = 0;

		/*
		id      type        supportDevice       vallue
		=================================================
		1       (*1)        V5U                 I2C_WRITE
		(*1):
			Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
			Output Format: [Not Use]
		*/
		memcpy(writeBuffer + nUseSize, &slAddr, sizeof(BYTE));
		nUseSize += sizeof(BYTE);
		memcpy(writeBuffer + nUseSize, &nAddrLen, sizeof(DWORD));
		nUseSize += sizeof(DWORD);
		memcpy(writeBuffer + nUseSize, &nAddr, sizeof(DWORD));
		nUseSize += sizeof(DWORD);
		memcpy(writeBuffer + nUseSize, &nCnt, sizeof(DWORD));
		nUseSize += sizeof(DWORD);
		memcpy(writeBuffer + nUseSize, buf, sizeof(unsigned char) * nCnt);
		nUseSize += sizeof(unsigned char) * nCnt;

		return g_pDevice->WriteValue(eDeviceWriteValueType::DWVT_I2C_WRITE, writeBuffer, nUseSize);
	}

	BOOL I2CRead(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf)
    {
        unsigned char writeBuffer[512] = {0};
        int nUseSize = 0;

        /*
        id      type        supportDevice       vallue
        =================================================
        15      (*5)        V5U                 I2C_READ
        15 (*5):
        Input  Format: [BYTE|slAddr][DWORD|nAddrLen][DWORD|nAddr][DWORD|nCnt][unsigned char[?]|buf]
        Output Format: [DWORD|nCnt][unsigned char[?]|buf]
        */
        memcpy(writeBuffer + nUseSize, &slAddr, sizeof(BYTE));
        nUseSize += sizeof(BYTE);
        memcpy(writeBuffer + nUseSize, &nAddrLen, sizeof(DWORD));
        nUseSize += sizeof(DWORD);
        memcpy(writeBuffer + nUseSize, &nAddr, sizeof(DWORD));
        nUseSize += sizeof(DWORD);
        memcpy(writeBuffer + nUseSize, &nCnt, sizeof(DWORD));
        nUseSize += sizeof(DWORD);
        memcpy(writeBuffer + nUseSize, buf, sizeof(unsigned char) * nCnt);
        nUseSize += sizeof(unsigned char) * nCnt;

        if (!g_pDevice->ReadValue(eDeviceReadValueType::DRVT_I2C_READ, writeBuffer, nUseSize))
        {
            return FALSE;
        }

        DWORD dwReadCnt = 0;
        memcpy(&dwReadCnt, writeBuffer, sizeof(DWORD));
        if (dwReadCnt != nCnt)
        {
            return FALSE;
        }
        memcpy(buf, writeBuffer + sizeof(DWORD), sizeof(unsigned char) * dwReadCnt);

        return TRUE;
    }


	BOOL RegWrite(WORD wRegAddr, WORD wRegData)
	{
		unsigned char writeBuffer[256] = {0};
		int nUseSize = 0;

		/*
		id      type        supportDevice       vallue
		=================================================
		2       (*2)        V5U                 REG_WRITE
		(*2):
			Input  Format: [WORD|wRegAddr][WORD|wRegData]
			Output Format: [Not Use]
			*/
		memcpy(writeBuffer + nUseSize, &wRegAddr, sizeof(WORD));
		nUseSize += sizeof(WORD);
		memcpy(writeBuffer + nUseSize, &wRegData, sizeof(WORD));
		nUseSize += sizeof(WORD);

		return g_pDevice->WriteValue(eDeviceWriteValueType::DWVT_REG_WRITE, writeBuffer, nUseSize);
	}
}