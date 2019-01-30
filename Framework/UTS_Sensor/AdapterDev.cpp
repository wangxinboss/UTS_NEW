#include "StdAfx.h"
#include "AdapterDev.h"
#include "CommonFunc.h"

#pragma comment(lib, "UTSFramework.lib")

namespace UTS
{

    AdapterDev::AdapterDev(void)
    {
    }


    AdapterDev::~AdapterDev(void)
    {
    }

    void AdapterDev::SetDevice(BaseDevice *pDevice)
    {
        m_pDevice = pDevice;
    }

    int AdapterDev::get_regval_size(int mode, int *regsize, int *valsize)
    {
        const struct regvalsize {
            int regsize, valsize;
        } size[] = {
            {1, 1},{1, 2},{2, 1},{2, 2},
        };
        if (mode < BIT8_BIT8 || mode > BIT16_BIT16) return -1;

        if (regsize) *regsize = size[mode].regsize;
        if (valsize) *valsize = size[mode].valsize;
        return 0;
    }

    int AdapterDev::i2c_read(int iic_mode, int start_addr, unsigned char* data, int len)
    {
        int nAddrLen = get_addr_len(iic_mode);

        int nSlavAddr = 0;
        if (!GetSlavAddr(nSlavAddr))
        {
            return -1;
        }
        if (!I2CRead((BYTE)nSlavAddr, nAddrLen, start_addr, len, data))
        {
            return -1;
        }
        return 0;
    }

	int AdapterDev::i2c_read(int iic_mode, int slav_addr, int start_addr, unsigned char* data, int len)
	{
		int nAddrLen = get_addr_len(iic_mode);

		int nSlavAddr = 0;
	
		if (!I2CRead(slav_addr, nAddrLen, start_addr, len, data))
		{
			return -1;
		}
		return 0;
	}
    
    int AdapterDev::i2c_read(int mode, u16 reg)
    {
        u8 buf[2];
        int reg_len, val_len;

        if (get_regval_size(mode, &reg_len, &val_len) < 0)
        {
            return -1;
        }
        if (i2c_read(mode, reg, buf, val_len) < 0)
        {
            return -1;
        }
        return get_be_val(buf, val_len);
    }
    
    int AdapterDev::i2c_write(int iic_mode, int start_addr, const char* data, int len)
    {
        int nAddrLen = get_addr_len(iic_mode);

        int nSlavAddr = 0;
        if (!GetSlavAddr(nSlavAddr))
        {
            return -1;
        }
        if (!I2CWrite((BYTE)nSlavAddr, nAddrLen, start_addr, len, (unsigned char*)data))
        {
            return -1;
        }
        return 0;
    }

	int AdapterDev::i2c_write(int iic_mode , int slav_addr,int start_addr, const char* data, int len)
	{
		int nAddrLen = get_addr_len(iic_mode);

		int nSlavAddr = 0;
		
		if (!I2CWrite(slav_addr, nAddrLen, start_addr, len, (unsigned char*)data))
		{
			return -1;
		}
		return 0;
	}

    BOOL AdapterDev::i2c_write(int mode, u16 reg, u16 val)
    {
        u8 buf[2];
        int reg_len, val_len;
        if (get_regval_size(mode, &reg_len, &val_len) < 0)
        {
            return FALSE;
        }
        put_be_val(val, buf, val_len);
        return (i2c_write(mode, reg, (const char *)buf, val_len) >= 0);
    }

    BOOL AdapterDev::write_sensor(WORD wRegAddr, WORD wRegData)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
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

        BOOL bRet = m_pDevice->WriteValue(
            eDeviceWriteValueType::DWVT_REG_WRITE,
            writeBuffer, nUseSize);
        return bRet;
    }

    int AdapterDev::get_addr_len(int mode)
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

    BOOL AdapterDev::write_sensor(WORD wRegAddr, const void *data, int len)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
        int i2c_mode = 0;
        if (!GetIICMode(i2c_mode))
        {
            return FALSE;
        }
        int nSlavAddr = 0;
        if (!GetSlavAddr(nSlavAddr))
        {
            return FALSE;
        }

        return I2CWrite(nSlavAddr, get_addr_len(i2c_mode), wRegAddr, len, (unsigned char *)data);
    }

    BOOL AdapterDev::read_sensor(WORD wRegAddr, WORD &wRegData)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
        unsigned char writeBuffer[256] = {0};
        int nUseSize = 0;

        /*
        id      type        supportDevice       vallue
        =================================================
        5       (*3)        V5U                 REG_READ
        (*3):
        Input  Format: [WORD|wRegAddr]
        Output Format: [WORD|wRegData]
        */
        memcpy(writeBuffer + nUseSize, &wRegAddr, sizeof(WORD));
        nUseSize += sizeof(WORD);

        BOOL bRet = m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_REG_READ,
            writeBuffer, nUseSize);
        memcpy(&wRegData, writeBuffer, sizeof(WORD));
        return bRet;
    }

    int AdapterDev::read_sensor(int addr)
    {
        WORD wRegData = 0;
        read_sensor((WORD)addr, wRegData);
        return wRegData;
    }

    int AdapterDev::read_sensor(int addr, void *data, int max_len)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
        int i2c_mode = 0;
        if (!GetIICMode(i2c_mode))
        {
            return FALSE;
        }
        int nSlavAddr = 0;
        if (!GetSlavAddr(nSlavAddr))
        {
            return FALSE;
        }

        return I2CRead(nSlavAddr, get_addr_len(i2c_mode), addr, max_len, (unsigned char *)data);
    }

    BOOL AdapterDev::GetIICMode(int &nIICMode)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
        /*
        id      type        supportDevice       vallue
        =================================================
        13      int         V5U                 IIC_MODE
        */
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_IIC_MODE,
            &nIICMode, sizeof(nIICMode)))
        {
            return FALSE;
        }
        return TRUE;
    }

    BOOL AdapterDev::SetIICMode(int nIICMode)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
        /*
        id      type        supportDevice       vallue
        =================================================
        12      int         V5U                 IIC_MODE
        */
        if (!m_pDevice->WriteValue(
            eDeviceWriteValueType::DWVT_IIC_MODE,
            &nIICMode, sizeof(nIICMode)))
        {
            return FALSE;
        }
        return TRUE;
    }

    BOOL AdapterDev::GetSlavAddr(int &nSlavAddr)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
        /*
        id      type        supportDevice       vallue
        =================================================
        14      int         V5U                 SLAV_ADDR
        */
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_SLAV_ADDR,
            &nSlavAddr, sizeof(nSlavAddr)))
        {
            return FALSE;
        }
        nSlavAddr = nSlavAddr << 1;
        return TRUE;
    }

    BOOL AdapterDev::I2CWrite(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
        unsigned char writeBuffer[512] = {0};
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

        return m_pDevice->WriteValue(eDeviceWriteValueType::DWVT_I2C_WRITE, writeBuffer, nUseSize);
    }

    BOOL AdapterDev::I2CRead(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
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

        if (!m_pDevice->ReadValue(eDeviceReadValueType::DRVT_I2C_READ, writeBuffer, nUseSize))
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

    BOOL AdapterDev::set_power_pin(int idx, float volt)
    {
        if (nullptr == m_pDevice)
        {
            return FALSE;
        }
        unsigned char writeBuffer[256] = {0};
        int nUseSize = 0;

        /*
        id      type        supportDevice       vallue
        =================================================
        13      (*4)        V5U                 SET_POWER_PIN
        13 (*4):
        Input Format: [int|idx][float|volt]
        */
        memcpy(writeBuffer + nUseSize, &idx, sizeof(int));
        nUseSize += sizeof(int);
        memcpy(writeBuffer + nUseSize, &volt, sizeof(float));
        nUseSize += sizeof(float);
        return m_pDevice->WriteValue(
            eDeviceWriteValueType::DWVT_SET_POWER_PIN,
            writeBuffer, nUseSize);
    }

    int AdapterDev::write_reg_array(int mode, const struct regval *vals, int valnum)
    {
        for (int i = 0; i < valnum; i++)
        {
            struct regval *val = (struct regval *)&vals[i];

            u16 attr = i2c_getattr(val->reg);
            switch (attr)
            {
            case I2CSLEEP: Sleep(val->value); break;
            case I2CREAD:  
                {
                    int ret = i2c_read(mode, i2creg(val->reg));
                    if (ret < 0) return -1;
                    val->value = ret;
                }
                break;
            case I2CREWRAND:
                {
                    int ret = i2c_read(mode, i2creg(val->reg));
                    if (ret < 0) return -1;
                    if (!i2c_write(mode, i2creg(val->reg), ret & (~val->value))) 
                        return -1;
                }
                break;
            case I2CREWROR:
                {
                    int ret = i2c_read(mode, i2creg(val->reg));
                    if (ret < 0) return -1;
                    if (!i2c_write(mode, i2creg(val->reg), ret | val->value)) 
                        return -1;
                }
                break;
            case I2CWRITE:
            default:
                if (!i2c_write(mode, i2creg(val->reg), val->value)) 
                    return -1;
                break;
            }
        }
        return 0;
    }

    int AdapterDev::GetOutOrder(void)
    {
        int nOutMode = 0;
        if (nullptr == m_pDevice)
        {
            return -1;
        }
        if (!m_pDevice->ReadValue(
            eDeviceReadValueType::DRVT_SENSOR_OUT_MODE,
            &nOutMode, sizeof(nOutMode)))
        {
            return -2;
        }
        return nOutMode;
    }

    int AdapterDev::GetImageSize(SIZE &size)
    {
        if (nullptr == m_pDevice)
        {
            return -1;
        }
        BUFFER_INFO bufferInfo;
        m_pDevice->GetBufferInfo(bufferInfo);
        size.cx = bufferInfo.nWidth;
        size.cy = bufferInfo.nHeight;
        return 0;
    }

}

