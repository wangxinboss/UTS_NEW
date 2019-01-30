#include "stdafx.h"
#include "HVSI2C.h"

namespace UTS
{
    BOOL CHVSI2C::IIC_Write_Proc(int mode, WORD wSlvAddr, DWORD dwRegAddr, DWORD dwRegData, int nBoardindex)
    {
        BYTE	addrBuf[10] = {0}; //if address size is big, try with it.
        BYTE	dataBuf[10] = {0};
        int		naddrLen, ndataLen;
        BOOL	bresult = TRUE;

        switch (mode)
        {
        case BIT8_BIT8:
            naddrLen = 1;
            ndataLen = 1;
            addrBuf[0] = (BYTE)dwRegAddr;
            dataBuf[0] = (BYTE)dwRegData;
            break;
        case BIT8_BIT16:
            naddrLen = 1;
            ndataLen = 2;
            addrBuf[0] = (BYTE)dwRegAddr;
            dataBuf[0] = (BYTE)(dwRegData >> 8);
            dataBuf[1] = (BYTE)(dwRegData & 0x00FF);
            break;
        case BIT16_BIT8:
            naddrLen = 2;
            ndataLen = 1;
            addrBuf[0] = (BYTE)(dwRegAddr >> 8);
            addrBuf[1] = (BYTE)(dwRegAddr & 0x00FF);
            dataBuf[0] = (BYTE)dwRegData;
            break;
        case BIT16_BIT16:
            naddrLen = 2;
            ndataLen = 2;
            addrBuf[0] = (BYTE)(dwRegAddr >> 8);
            addrBuf[1] = (BYTE)(dwRegAddr & 0x00FF);
            dataBuf[0] = (BYTE)(dwRegData >> 8);
            dataBuf[1] = (BYTE)(dwRegData & 0x00FF);
            break;
        default:
            uts.log.Error(_T("IIC mode error. mode = [%d]."), mode);
            bresult = FALSE;
            break;
        }

        if (bresult)
        {
            bresult = I2C_SYS_Write_Mul(
                nBoardindex,
                wSlvAddr << 1,
                naddrLen,
                dwRegAddr,
                ndataLen,
                dataBuf);
            if (!bresult)
            {
                uts.log.Error(_T("I2C_SYS_Write_Mul error."));
            }
        }

        return bresult;
    }

    BOOL CHVSI2C::IIC_Write_Proc2(int mode, WORD wSlvAddr, DWORD dwRegAddr, DWORD dwRegData, int nBoardindex)
    {
        if (!IIC_Write_Proc(mode, wSlvAddr, dwRegAddr, dwRegData, nBoardindex))
        {
            if (!IIC_Write_Proc(mode, wSlvAddr, dwRegAddr, dwRegData, nBoardindex))
            {
                uts.log.Error(_T("I2C Fail: [mode = %d][slvAddr = 0x%x][Addr = 0x%x][Data = 0x%x][Boardindex = %d]"),
                    mode, wSlvAddr, dwRegAddr, dwRegData, nBoardindex);
                return FALSE;
            }
        }
        return TRUE;
    }

    int	CHVSI2C::IIC_Read_Proc(int mode, WORD wSlvAddr, DWORD dwRegAddr, int nBoardindex)
    {
        int		wRegData = 0;
        int		naddrLen, ndataLen;
        BYTE	buf[256] = {0};
        BYTE	addrBuf[16] = {0};  //if address size is big, try with it.

        switch (mode)
        {
        case BIT8_BIT8:
            naddrLen = 1;
            ndataLen = 1;
            addrBuf[0] = (BYTE)dwRegAddr;
            break;
        case BIT8_BIT16:
            naddrLen = 1;
            ndataLen = 2;
            addrBuf[0] = (BYTE)dwRegAddr;
            break;
        case BIT16_BIT8:
            naddrLen = 2;
            ndataLen = 1;
            addrBuf[0] = (BYTE)(dwRegAddr >> 8);
            addrBuf[1] = (BYTE)(dwRegAddr & 0x00FF);
            break;
        case BIT16_BIT16:
            naddrLen = 2;
            ndataLen = 2;
            addrBuf[0] = (BYTE)(dwRegAddr >> 8);
            addrBuf[1] = (BYTE)(dwRegAddr & 0x00FF);
            break;
        default:
            uts.log.Error(_T("IIC mode error. mode = [%d]."), mode);
            break;
        }

        if (!I2C_SYS_Read_Mul(
            nBoardindex,
            wSlvAddr << 1,
            naddrLen,
            dwRegAddr,
            ndataLen,
            buf))
        {
            uts.log.Error(_T("[V5U API]I2C_SYS_Read_Mul error."));
            return -1;
        }

        if (ndataLen == 1)
        {
            wRegData = buf[0];
        }
        else if (ndataLen == 2)
        {
            wRegData = buf[0];
            wRegData = wRegData << 8;
            wRegData += buf[1];
        }

        return wRegData;	
    }

    int CHVSI2C::get_addr_len(int mode)
    {
        switch (mode)
        {
        case BIT8_BIT8:
        case BIT8_BIT16:
            return 1;
        case BIT16_BIT8:
        case BIT16_BIT16:
            return 2;
        default:
            return -1;
        }
    }
    int CHVSI2C::get_data_len(int mode)
    {
        switch (mode)
        {
        case BIT8_BIT8:
        case BIT16_BIT8:
            return 1;
        case BIT8_BIT16:
        case BIT16_BIT16:
            return 2;
        default:
            return -1;
        }
    }

    BOOL CHVSI2C::i2c_write(int board, int mode, int slave_addr, int start_addr, const char *data, int len)
    {
        int addr_len = get_addr_len(mode);

        return I2C_SYS_Write_Mul(board, slave_addr, addr_len, start_addr, len, (unsigned char*)data);
    }

    BOOL CHVSI2C::i2c_read(int board, int mode, int slave_addr, int start_addr, char *data, int len)
    {
        int addr_len = get_addr_len(mode);

        return I2C_SYS_Read_Mul(board, slave_addr, addr_len, start_addr, len, (unsigned char*)data);
    }
}

