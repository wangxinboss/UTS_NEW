/*
Module : SERIALPORT.CPP
Purpose: Implementation for an MFC wrapper class for serial ports
Created: PJN / 31-05-1999
History: PJN / 03-06-1999 1. Fixed problem with code using CancelIo which does not exist on 95.
                          2. Fixed leaks which can occur in sample app when an exception is thrown
         PJN / 16-06-1999 1. Fixed a bug whereby CString::ReleaseBuffer was not being called in 
                             CSerialException::GetErrorMessage
         PJN / 29-09-1999 1. Fixed a simple copy and paste bug in CSerialPort::SetDTR

Copyright (c) 1999 by PJ Naughter.  
All rights reserved.

*/

/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include "serialport.h"
#include "winerror.h"




///////////////////////////////// defines /////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




//////////////////////////////// Implementation ///////////////////////////////



//Class which handles CancelIo function which must be constructed at run time
//since it is not imeplemented on NT 3.51 or Windows 95. To avoid the loader
//bringing up a message such as "Failed to load due to missing export...", the
//function is constructed using GetProcAddress. The CSerialPort::CancelIo 
//function then checks to see if the function pointer is NULL and if it is it 
//throws an exception using the error code ERROR_CALL_NOT_IMPLEMENTED which
//is what 95 would have done if it had implemented a stub for it in the first
//place !!

class _SERIAL_PORT_DATA
{
public:
//Constructors /Destructors
  _SERIAL_PORT_DATA();
  ~_SERIAL_PORT_DATA();

  HINSTANCE m_hKernel32;
  typedef BOOL (CANCELIO)(HANDLE);
  typedef CANCELIO* LPCANCELIO;
  LPCANCELIO m_lpfnCancelIo;
};

_SERIAL_PORT_DATA::_SERIAL_PORT_DATA()
{
  m_hKernel32 = LoadLibrary(_T("KERNEL32.DLL"));
  VERIFY(m_hKernel32 != NULL);
  m_lpfnCancelIo = (LPCANCELIO) GetProcAddress(m_hKernel32, "CancelIo");
}

_SERIAL_PORT_DATA::~_SERIAL_PORT_DATA()
{
  FreeLibrary(m_hKernel32);
  m_hKernel32 = NULL;
}


//The local variable which handle the function pointers

_SERIAL_PORT_DATA _SerialPortData;




////////// Exception handling code

void AfxThrowSerialException(DWORD dwError /* = 0 */)
{
	if (dwError == 0)
		dwError = ::GetLastError();

	CSerialException* pException = new CSerialException(dwError);

	TRACE(_T("Warning: throwing CSerialException for error %d\n"), dwError);
	THROW(pException);
}

BOOL CSerialException::GetErrorMessage(LPTSTR pstrError, UINT nMaxError, PUINT pnHelpContext)
{
	ASSERT(pstrError != NULL && AfxIsValidString(pstrError, nMaxError));

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	LPTSTR lpBuffer;
	BOOL bRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			                      NULL,  m_dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			                      (LPTSTR) &lpBuffer, 0, NULL);

	if (bRet == FALSE)
		*pstrError = '\0';
	else
	{
		lstrcpyn(pstrError, lpBuffer, nMaxError);
		bRet = TRUE;

		LocalFree(lpBuffer);
	}

	return bRet;
}

CString CSerialException::GetErrorMessage()
{
  CString rVal;
  LPTSTR pstrError = rVal.GetBuffer(4096);
  GetErrorMessage(pstrError, 4096, NULL);
  rVal.ReleaseBuffer();
  return rVal;
}

CSerialException::CSerialException(DWORD dwError)
{
	m_dwError = dwError;
}

CSerialException::~CSerialException()
{
}

IMPLEMENT_DYNAMIC(CSerialException, CException)

#ifdef _DEBUG
void CSerialException::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "m_dwError = " << m_dwError;
}
#endif





////////// The actual serial port code

CSerialPort::CSerialPort()
{
  m_hComm = INVALID_HANDLE_VALUE;
  m_bOverlapped = FALSE;
}

CSerialPort::~CSerialPort()
{
  Close();
}

IMPLEMENT_DYNAMIC(CSerialPort, CObject)

#ifdef _DEBUG
void CSerialPort::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << _T("m_hComm = ") << m_hComm << _T("\n");
  dc << _T("m_bOverlapped = ") << m_bOverlapped;
}
#endif

void CSerialPort::Open(int nPort, DWORD dwBaud, Parity parity, BYTE DataBits, StopBits stopbits, FlowControl fc, BOOL bOverlapped)
{
  //Validate our parameters
  ASSERT(nPort>0 && nPort<=255);

  //Call CreateFile to open up the comms port
  CString sPort;
  sPort.Format(_T("\\\\.\\COM%d"), nPort);
  m_hComm = CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
  if (m_hComm == INVALID_HANDLE_VALUE)
  {
    TRACE(_T("Failed to open up the comms port\n"));
    AfxThrowSerialException();
  }
  
  m_bOverlapped = bOverlapped;

  //Get the current state prior to changing it
  DCB dcb;
  GetState(dcb);

  //Setup the baud rate
  dcb.BaudRate = dwBaud; 

  //Setup the Parity
  switch (parity)
  {
    case EvenParity:  dcb.Parity = EVENPARITY;  break;
    case MarkParity:  dcb.Parity = MARKPARITY;  break;
    case NoParity:    dcb.Parity = NOPARITY;    break;
    case OddParity:   dcb.Parity = ODDPARITY;   break;
    case SpaceParity: dcb.Parity = SPACEPARITY; break;
    default:          ASSERT(FALSE);            break;
  }

  //Setup the data bits
  dcb.ByteSize = DataBits;

  //Setup the stop bits
  switch (stopbits)
  {
    case OneStopBit:           dcb.StopBits = ONESTOPBIT;   break;
    case OnePointFiveStopBits: dcb.StopBits = ONE5STOPBITS; break;
    case TwoStopBits:          dcb.StopBits = TWOSTOPBITS;  break;
    default:                   ASSERT(FALSE);               break;
  }

  //Setup the flow control 
  dcb.fDsrSensitivity = FALSE;
  switch (fc)
  {
    case NoFlowControl:
    {
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case CtsRtsFlowControl:
    {
      dcb.fOutxCtsFlow = TRUE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case CtsDtrFlowControl:
    {
      dcb.fOutxCtsFlow = TRUE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case DsrRtsFlowControl:
    {
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = TRUE;
      dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case DsrDtrFlowControl:
    {
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = TRUE;
      dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      break;
    }
    case XonXoffFlowControl:
    {
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fOutX = TRUE;
      dcb.fInX = TRUE;
      dcb.XonChar = 0x11;
      dcb.XoffChar = 0x13;
      dcb.XoffLim = 100;
      dcb.XonLim = 100;
      break;
    }
    default:
    {
      ASSERT(FALSE);
      break;
    }
  }
  
  //Now that we have all the settings in place, make the changes
  SetState(dcb);
}

void CSerialPort::Close()
{
  if (IsOpen())
  {
    BOOL bSuccess = CloseHandle(m_hComm);
    m_hComm = INVALID_HANDLE_VALUE;
    if (!bSuccess)
      TRACE(_T("Failed to close up the comms port, GetLastError:%d\n"), GetLastError());
    m_bOverlapped = FALSE;
  }
}

void CSerialPort::Attach(HANDLE hComm)
{
  Close();
  m_hComm = hComm;  
}

HANDLE CSerialPort::Detach()
{
  HANDLE hrVal = m_hComm;
  m_hComm = INVALID_HANDLE_VALUE;
  return hrVal;
}

DWORD CSerialPort::Read(void* lpBuf, DWORD dwCount)
{
  ASSERT(IsOpen());
  ASSERT(!m_bOverlapped);

  DWORD dwBytesRead = 0;
  if (!ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, NULL))
  {
    TRACE(_T("Failed in call to ReadFile\n"));
    AfxThrowSerialException();
  }

  return dwBytesRead;
}

BOOL CSerialPort::Read(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped)
{
  ASSERT(IsOpen());
  ASSERT(m_bOverlapped);
  ASSERT(overlapped.hEvent);

  DWORD dwBytesRead = 0;
  BOOL bSuccess = ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, &overlapped);
  if (!bSuccess)
  {
    if (GetLastError() != ERROR_IO_PENDING)
    {
      TRACE(_T("Failed in call to ReadFile\n"));
      AfxThrowSerialException();
    }
  }
  return bSuccess;
}

DWORD CSerialPort::Write(const void* lpBuf, DWORD dwCount)
{
  ASSERT(IsOpen());
  ASSERT(!m_bOverlapped);

  DWORD dwBytesWritten = 0;
  if (!WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, NULL))
  {
    TRACE(_T("Failed in call to WriteFile\n"));
    AfxThrowSerialException();
  }

  return dwBytesWritten;
}

BOOL CSerialPort::Write(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped)
{
  ASSERT(IsOpen());
  ASSERT(m_bOverlapped);
  ASSERT(overlapped.hEvent);

  DWORD dwBytesWritten = 0;
  BOOL bSuccess = WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, &overlapped);
  if (!bSuccess)
  {
    if (GetLastError() != ERROR_IO_PENDING)
    {
      TRACE(_T("Failed in call to WriteFile\n"));
      AfxThrowSerialException();
    }
  }
  return bSuccess;
}

void CSerialPort::GetOverlappedResult(OVERLAPPED& overlapped, DWORD& dwBytesTransferred, BOOL bWait)
{
  ASSERT(IsOpen());
  ASSERT(m_bOverlapped);
  ASSERT(overlapped.hEvent);

  DWORD dwBytesWritten = 0;
  if (!::GetOverlappedResult(m_hComm, &overlapped, &dwBytesTransferred, bWait))
  {
    if (GetLastError() != ERROR_IO_PENDING)
    {
      TRACE(_T("Failed in call to GetOverlappedResult\n"));
      AfxThrowSerialException();
    }
  }
}

void CSerialPort::_OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped)
{
  //Validate our parameters
  ASSERT(lpOverlapped);

  //Convert back to the C++ world
  CSerialPort* pSerialPort = (CSerialPort*) lpOverlapped->hEvent;
  ASSERT(pSerialPort->IsKindOf(RUNTIME_CLASS(CSerialPort)));

  //Call the C++ function
  pSerialPort->OnCompletion(dwErrorCode, dwCount, lpOverlapped);
}

void CSerialPort::OnCompletion(DWORD /*dwErrorCode*/, DWORD /*dwCount*/, LPOVERLAPPED lpOverlapped)
{
  //Just free up the memory which was previously allocated for the OVERLAPPED structure
  delete lpOverlapped;

  //Your derived classes can do something useful in OnCompletion, but don't forget to
  //call CSerialPort::OnCompletion to ensure the memory is freed up
}

void CSerialPort::CancelIo()
{
  ASSERT(IsOpen());

  if (_SerialPortData.m_lpfnCancelIo == NULL)
  {
    TRACE(_T("CancelIo function is not supported on this OS. You need to be running at least NT 4 or Win 98 to use this function\n"));
    AfxThrowSerialException(ERROR_CALL_NOT_IMPLEMENTED);  
  }

  if (!::_SerialPortData.m_lpfnCancelIo(m_hComm))
  {
    TRACE(_T("Failed in call to CancelIO\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::WriteEx(const void* lpBuf, DWORD dwCount)
{
  ASSERT(IsOpen());

  OVERLAPPED* pOverlapped = new OVERLAPPED;
  ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
  pOverlapped->hEvent = (HANDLE) this;
  if (!WriteFileEx(m_hComm, lpBuf, dwCount, pOverlapped, _OnCompletion))
  {
    delete pOverlapped;
    TRACE(_T("Failed in call to WriteFileEx\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::ReadEx(void* lpBuf, DWORD dwCount)
{
  ASSERT(IsOpen());

  OVERLAPPED* pOverlapped = new OVERLAPPED;
  ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
  pOverlapped->hEvent = (HANDLE) this;
  if (!ReadFileEx(m_hComm, lpBuf, dwCount, pOverlapped, _OnCompletion))
  {
    delete pOverlapped;
    TRACE(_T("Failed in call to ReadFileEx\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::TransmitChar(char cChar)
{
  ASSERT(IsOpen());

  if (!TransmitCommChar(m_hComm, cChar))
  {
    TRACE(_T("Failed in call to TransmitCommChar\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::GetConfig(COMMCONFIG& config)
{
  ASSERT(IsOpen());

  DWORD dwSize = sizeof(COMMCONFIG);
  if (!GetCommConfig(m_hComm, &config, &dwSize))
  {
    TRACE(_T("Failed in call to GetCommConfig\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::SetConfig(COMMCONFIG& config)
{
  ASSERT(IsOpen());

  DWORD dwSize = sizeof(COMMCONFIG);
  if (!SetCommConfig(m_hComm, &config, dwSize))
  {
    TRACE(_T("Failed in call to SetCommConfig\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::SetBreak()
{
  ASSERT(IsOpen());

  if (!SetCommBreak(m_hComm))
  {
    TRACE(_T("Failed in call to SetCommBreak\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::ClearBreak()
{
  ASSERT(IsOpen());

  if (!ClearCommBreak(m_hComm))
  {
    TRACE(_T("Failed in call to SetCommBreak\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::ClearError(DWORD& dwErrors)
{
  ASSERT(IsOpen());

  if (!ClearCommError(m_hComm, &dwErrors, NULL))
  {
    TRACE(_T("Failed in call to ClearCommError\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::GetDefaultConfig(int nPort, COMMCONFIG& config)
{
  //Validate our parameters
  ASSERT(nPort>0 && nPort<=255);

  //Create the device name as a string
  CString sPort;
  sPort.Format(_T("COM%d"), nPort);

  DWORD dwSize = sizeof(COMMCONFIG);
  if (!GetDefaultCommConfig(sPort, &config, &dwSize))
  {
    TRACE(_T("Failed in call to GetDefaultCommConfig\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::SetDefaultConfig(int nPort, COMMCONFIG& config)
{
  //Validate our parameters
  ASSERT(nPort>0 && nPort<=255);

  //Create the device name as a string
  CString sPort;
  sPort.Format(_T("COM%d"), nPort);

  DWORD dwSize = sizeof(COMMCONFIG);
  if (!SetDefaultCommConfig(sPort, &config, dwSize))
  {
    TRACE(_T("Failed in call to GetDefaultCommConfig\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::GetStatus(COMSTAT& stat)
{
  ASSERT(IsOpen());

  DWORD dwErrors;
  if (!ClearCommError(m_hComm, &dwErrors, &stat))
  {
    TRACE(_T("Failed in call to ClearCommError\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::GetState(DCB& dcb)
{
  ASSERT(IsOpen());

  if (!GetCommState(m_hComm, &dcb))
  {
    TRACE(_T("Failed in call to GetCommState\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::SetState(DCB& dcb)
{
  ASSERT(IsOpen());

  if (!SetCommState(m_hComm, &dcb))
  {
    TRACE(_T("Failed in call to SetCommState\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::Escape(DWORD dwFunc)
{
  ASSERT(IsOpen());

  if (!EscapeCommFunction(m_hComm, dwFunc))
  {
    TRACE(_T("Failed in call to EscapeCommFunction\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::ClearDTR()
{
  Escape(CLRDTR);
}

void CSerialPort::ClearRTS()
{
  Escape(CLRRTS);
}

void CSerialPort::SetDTR()
{
  Escape(SETDTR);
}

void CSerialPort::SetRTS()
{
  Escape(SETRTS);
}

void CSerialPort::SetXOFF()
{
  Escape(SETXOFF);
}

void CSerialPort::SetXON()
{
  Escape(SETXON);
}

void CSerialPort::GetProperties(COMMPROP& properties)
{
  ASSERT(IsOpen());

  if (!GetCommProperties(m_hComm, &properties))
  {
    TRACE(_T("Failed in call to GetCommProperties\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::GetModemStatus(DWORD& dwModemStatus)
{
  ASSERT(IsOpen());

  if (!GetCommModemStatus(m_hComm, &dwModemStatus))
  {
    TRACE(_T("Failed in call to GetCommModemStatus\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::SetMask(DWORD dwMask)
{
  ASSERT(IsOpen());

  if (!SetCommMask(m_hComm, dwMask))
  {
    TRACE(_T("Failed in call to SetCommMask\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::GetMask(DWORD& dwMask)
{
  ASSERT(IsOpen());

  if (!GetCommMask(m_hComm, &dwMask))
  {
    TRACE(_T("Failed in call to GetCommMask\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::Flush()
{
  ASSERT(IsOpen());

  if (!FlushFileBuffers(m_hComm))
  {
    TRACE(_T("Failed in call to FlushFileBuffers\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::Purge(DWORD dwFlags)
{
  ASSERT(IsOpen());

  if (!PurgeComm(m_hComm, dwFlags))
  {
    TRACE(_T("Failed in call to PurgeComm\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::TerminateOutstandingWrites()
{
  Purge(PURGE_TXABORT);
}

void CSerialPort::TerminateOutstandingReads()
{
  Purge(PURGE_RXABORT);
}

void CSerialPort::ClearWriteBuffer()
{
  Purge(PURGE_TXCLEAR);
}

void CSerialPort::ClearReadBuffer()
{
  Purge(PURGE_RXCLEAR);
}

void CSerialPort::Setup(DWORD dwInQueue, DWORD dwOutQueue)
{
  ASSERT(IsOpen());

  if (!SetupComm(m_hComm, dwInQueue, dwOutQueue))
  {
    TRACE(_T("Failed in call to SetupComm\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::SetTimeouts(COMMTIMEOUTS& timeouts)
{
  ASSERT(IsOpen());

  if (!SetCommTimeouts(m_hComm, &timeouts))
  {
    TRACE(_T("Failed in call to SetCommTimeouts\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::GetTimeouts(COMMTIMEOUTS& timeouts)
{
  ASSERT(IsOpen());

  if (!GetCommTimeouts(m_hComm, &timeouts))
  {
    TRACE(_T("Failed in call to GetCommTimeouts\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::Set0Timeout()
{
  COMMTIMEOUTS Timeouts;
  ZeroMemory(&Timeouts, sizeof(COMMTIMEOUTS));
  Timeouts.ReadIntervalTimeout = MAXDWORD;
  Timeouts.ReadTotalTimeoutMultiplier = 0;
  Timeouts.ReadTotalTimeoutConstant = 0;
  Timeouts.WriteTotalTimeoutMultiplier = 0;
  Timeouts.WriteTotalTimeoutConstant = 0;
  SetTimeouts(Timeouts);
}

void CSerialPort::Set0WriteTimeout()
{
  COMMTIMEOUTS Timeouts;
  GetTimeouts(Timeouts);
  Timeouts.WriteTotalTimeoutMultiplier = 0;
  Timeouts.WriteTotalTimeoutConstant = 0;
  SetTimeouts(Timeouts);
}

void CSerialPort::Set0ReadTimeout()
{
  COMMTIMEOUTS Timeouts;
  GetTimeouts(Timeouts);
  Timeouts.ReadIntervalTimeout = MAXDWORD;
  Timeouts.ReadTotalTimeoutMultiplier = 0;
  Timeouts.ReadTotalTimeoutConstant = 0;
  SetTimeouts(Timeouts);
}

void CSerialPort::WaitEvent(DWORD& dwMask)
{
  ASSERT(IsOpen());
  ASSERT(!m_bOverlapped);

  if (!WaitCommEvent(m_hComm, &dwMask, NULL))
  {
    TRACE(_T("Failed in call to WaitCommEvent\n"));
    AfxThrowSerialException();
  }
}

void CSerialPort::WaitEvent(DWORD& dwMask, OVERLAPPED& overlapped)
{
  ASSERT(IsOpen());
  ASSERT(m_bOverlapped);
  ASSERT(overlapped.hEvent);

  if (!WaitCommEvent(m_hComm, &dwMask, &overlapped))
  {
    if (GetLastError() != ERROR_IO_PENDING)
    {
      TRACE(_T("Failed in call to WaitCommEvent\n"));
      AfxThrowSerialException();
    }
  }
}
