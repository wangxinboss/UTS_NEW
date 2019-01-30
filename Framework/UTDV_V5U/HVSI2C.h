#pragma once

namespace UTS
{
    //------------------------------------------------------------------------------
    // IIC mode
    typedef enum
    {
        BIT8_BIT8 = 0,
        BIT8_BIT16 = 1,
        BIT16_BIT8 = 2,
        BIT16_BIT16 = 3,
    } eI2CMode;

    class CHVSI2C
    {
    public:
        BOOL IIC_Write_Proc(int mode, WORD wSlvAddr, DWORD dwRegAddr, DWORD dwRegData, int nBoardIndex);
        BOOL IIC_Write_Proc2(int mode, WORD wSlvAddr, DWORD dwRegAddr, DWORD dwRegData, int nBoardIndex);
        int	 IIC_Read_Proc(int mode, WORD wSlvAddr, DWORD dwRegAddr, int nBoardIndex);
        void SetDeviceID(WORD wDeviceID);

        BOOL i2c_write(int board, int mode, int slave_addr, int start_addr, const char *data, int len);
        BOOL i2c_read(int board, int mode, int slave_addr, int start_addr, char *data, int len);

    private:
        int get_addr_len(int mode);
        int get_data_len(int mode);
    };
}

