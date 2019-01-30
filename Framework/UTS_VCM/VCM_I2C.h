#include "BaseDevice.h"

namespace UTS
{
	BOOL RegWrite(WORD wRegAddr, WORD wRegData);
	BOOL I2CWrite(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf);
	BOOL I2CRead(BYTE slAddr, DWORD nAddrLen, DWORD nAddr, DWORD nCnt, unsigned char* buf);
}