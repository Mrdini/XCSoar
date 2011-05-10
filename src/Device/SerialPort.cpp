/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2011 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Device/SerialPort.hpp"
#include "Dialogs/Message.hpp"
#include "Language/Language.hpp"
#include "Message.hpp"
#include "Asset.hpp"
#include "OS/Sleep.h"

#ifdef _WIN32_WCE
#include "Device/Widcomm.hpp"
#endif

#include <windows.h>

#include <assert.h>
#include <tchar.h>
#include <stdio.h>

static void
SerialPort_StatusMessage(unsigned type, const TCHAR *caption,
                      const TCHAR *fmt, ...)
{
  TCHAR tmp[127];
  va_list ap;

  va_start(ap, fmt);
  _vsntprintf(tmp, 127, fmt, ap);
  va_end(ap);

  if (caption)
    MessageBoxX(tmp, caption, type);
  else
    Message::AddMessage(tmp);
}

SerialPort::SerialPort(const TCHAR *path, unsigned _baud_rate, Handler &_handler)
  :Port(_handler), baud_rate(_baud_rate),
   hPort(INVALID_HANDLE_VALUE),
   buffer(NMEA_BUF_SIZE)
{
  assert(path != NULL);

  _tcscpy(sPortName, path);
}

SerialPort::~SerialPort()
{
  Close();
}

bool
SerialPort::Open()
{
#ifdef _WIN32_WCE
  is_widcomm = IsWidcommDevice(sPortName);
#endif

  DCB PortDCB;

  buffer.clear();

  // Open the serial port.
  hPort = CreateFile(sPortName,    // Pointer to the name of the port
                     GENERIC_READ | GENERIC_WRITE, // Access (read-write) mode
                     0,            // Share mode
                     NULL,         // Pointer to the security attribute
                     OPEN_EXISTING,// How to open the serial port
                     FILE_ATTRIBUTE_NORMAL, // Port attributes
                     NULL);        // Handle to port with attribute to copy

  // If it fails to open the port, return false.
  if (hPort == INVALID_HANDLE_VALUE) {
    // Could not open the port.
    SerialPort_StatusMessage(MB_OK | MB_ICONINFORMATION, NULL,
                          _("Unable to open port %s"), sPortName);

    return false;
  }

  PortDCB.DCBlength = sizeof(DCB);

  // Get the default port setting information.
  GetCommState(hPort, &PortDCB);

  // Change the DCB structure settings.
  PortDCB.BaudRate = baud_rate; // Current baud
  PortDCB.fBinary = true;               // Binary mode; no EOF check
  PortDCB.fParity = true;               // Enable parity checking
  PortDCB.fOutxCtsFlow = false;         // No CTS output flow control
  PortDCB.fOutxDsrFlow = false;         // No DSR output flow control
  PortDCB.fDtrControl = DTR_CONTROL_ENABLE; // DTR flow control type
  PortDCB.fDsrSensitivity = false;      // DSR sensitivity
  PortDCB.fTXContinueOnXoff = true;     // XOFF continues Tx
  PortDCB.fOutX = false;                // No XON/XOFF out flow control
  PortDCB.fInX = false;                 // No XON/XOFF in flow control
  PortDCB.fErrorChar = false;           // Disable error replacement
  PortDCB.fNull = false;                // Disable null removal
  PortDCB.fRtsControl = RTS_CONTROL_ENABLE; // RTS flow control
  PortDCB.fAbortOnError = true;         // JMW abort reads/writes on error
  PortDCB.ByteSize = 8;                 // Number of bits/byte, 4-8
  PortDCB.Parity = NOPARITY;            // 0-4=no,odd,even,mark,space
  PortDCB.StopBits = ONESTOPBIT;        // 0,1,2 = 1, 1.5, 2
  PortDCB.EvtChar = '\n';               // wait for end of line

  // Configure the port according to the specifications of the DCB structure.
  if (!SetCommState(hPort, &PortDCB)) {
    // Could not create the read thread.
    CloseHandle(hPort);
    hPort = INVALID_HANDLE_VALUE;

    // TODO code: SCOTT I18N - Fix this to sep the TEXT from PORT, TEXT can be
    // gettext(), port added on new line
    SerialPort_StatusMessage(MB_OK, _("Error"),
                          _("Unable to change settings on port %s"), sPortName);
    return false;
  }

  //  SetRxTimeout(10); // JMW20070515 wait a maximum of 10ms
  SetRxTimeout(0);

  SetupComm(hPort, 1024, 1024);

  // Direct the port to perform extended functions SETDTR and SETRTS
  // SETDTR: Sends the DTR (data-terminal-ready) signal.
  EscapeCommFunction(hPort, SETDTR);
  // SETRTS: Sends the RTS (request-to-send) signal.
  EscapeCommFunction(hPort, SETRTS);

  if (!StartRxThread()){
    CloseHandle(hPort);
    hPort = INVALID_HANDLE_VALUE;

    return false;
  }

  return true;
}

void
SerialPort::Flush(void)
{
  PurgeComm(hPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}

void
SerialPort::run()
{
  DWORD dwCommModemStatus, dwBytesTransferred;
  BYTE inbuf[1024];

  // JMW added purging of port on open to prevent overflow
  Flush();

  // Specify a set of events to be monitored for the port.
  if (is_widcomm)
    SetRxTimeout(180);
  else
    ::SetCommMask(hPort, EV_RXCHAR & EV_BREAK);

  while (!is_stopped()) {
    if (is_widcomm) {
      /* WaitCommEvent() doesn't work with the Widcomm Bluetooth
         driver, it blocks for 11 seconds, regardless whether data is
         received.  This workaround polls for input manually.
         Observed on an iPaq hx4700 with WM6. */
    } else {
      // Wait for an event to occur for the port.
      if (!::WaitCommEvent(hPort, &dwCommModemStatus, 0)) {
        // error reading from port
        Sleep(100);
        continue;
      }

      if ((dwCommModemStatus & EV_RXCHAR) == 0)
        /* no data available */
        continue;
    }

    // Read the data from the serial port.
    if (!ReadFile(hPort, inbuf, 1024, &dwBytesTransferred, NULL) ||
        dwBytesTransferred == 0) {
      Sleep(100);
      continue;
    }

    for (unsigned int j = 0; j < dwBytesTransferred; j++)
      ProcessChar(inbuf[j]);
  }

  Flush();
}

bool
SerialPort::Close()
{
  if (hPort != INVALID_HANDLE_VALUE) {
    StopRxThread();
    Sleep(100); // todo ...

    // Close the communication port.
    if (!CloseHandle(hPort)) {
      return false;
    } else {
      if (!is_embedded())
        Sleep(2000); // needed for windows bug

      hPort = INVALID_HANDLE_VALUE;
      return true;
    }
  }

  return false;
}

void
SerialPort::Write(const void *data, unsigned length)
{
  if (hPort == INVALID_HANDLE_VALUE)
    return;

  if (is_embedded())
    /* this is needed to work around a driver bug on the HP31x -
       without it, the second consecutive write without a task switch
       will hang the whole PNA; this Sleep() call enforces a task
       switch */
    Sleep(100);

  DWORD NumberOfBytesWritten;
  // lpNumberOfBytesWritten : This parameter can be NULL only when the lpOverlapped parameter is not NULL.
  ::WriteFile(hPort, data, length, &NumberOfBytesWritten, NULL);
}

bool
SerialPort::StopRxThread()
{
  // Make sure the thread isn't terminating itself
  assert(!Thread::inside());

  // Make sure the port is still open
  if (hPort == INVALID_HANDLE_VALUE)
    return false;

  // If the thread is not running, cancel the rest of the function
  if (!Thread::defined())
    return true;

  stop();

  Flush();

  /* this will cancel WaitCommEvent() */
  if (!is_widcomm)
    ::SetCommMask(hPort, 0);

  if (!Thread::join(2000)) {
    /* On Dell Axim x51v, the Bluetooth RFCOMM driver seems to be
       bugged: when the peer gets disconnected (e.g. switched off),
       the function WaitCommEvent() does not get cancelled by
       SetCommMask(), but it should be according to MSDN.  As a
       workaround, we force WaitCommEvent() to abort by destroying the
       handle.  That seems to do the trick. */
    ::CloseHandle(hPort);
    hPort = INVALID_HANDLE_VALUE;

    Thread::join();
  }

  return true;
}

bool
SerialPort::StartRxThread(void)
{
  // Make sure the thread isn't starting itself
  assert(!Thread::inside());

  // Make sure the port was opened correctly
  if (hPort == INVALID_HANDLE_VALUE)
    return false;

  // Start the receive thread
  StoppableThread::start();
  return true;
}

bool
SerialPort::SetRxTimeout(int Timeout)
{
  COMMTIMEOUTS CommTimeouts;

  if (hPort == INVALID_HANDLE_VALUE)
    return false;

  CommTimeouts.ReadIntervalTimeout = MAXDWORD;

  if (Timeout == 0) {
    // no total timeouts used
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.ReadTotalTimeoutConstant = 0;
  } else {
    // only total timeout used
    CommTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
    CommTimeouts.ReadTotalTimeoutConstant = Timeout;
  }

  CommTimeouts.WriteTotalTimeoutMultiplier = 10;
  CommTimeouts.WriteTotalTimeoutConstant = 1000;

  // Set the time-out parameters
  // for all read and write
  // operations on the port.
  if (!SetCommTimeouts(hPort, &CommTimeouts)) {
     // Could not create the read thread.
    CloseHandle(hPort);
    hPort = INVALID_HANDLE_VALUE;

    if (!is_embedded())
      Sleep(2000); // needed for windows bug

    SerialPort_StatusMessage(MB_OK, _("Error"),
                          _("Unable to set serial port timers %s"), sPortName);
    return false;
  }

  return true;
}

unsigned long
SerialPort::GetBaudrate() const
{
  if (hPort == INVALID_HANDLE_VALUE)
    return 0;

  DCB PortDCB;
  GetCommState(hPort, &PortDCB);

  return PortDCB.BaudRate;
}

unsigned long
SerialPort::SetBaudrate(unsigned long BaudRate)
{
  COMSTAT ComStat;
  DCB PortDCB;
  DWORD dwErrors;
  unsigned long result = 0;

  if (hPort == INVALID_HANDLE_VALUE)
    return result;

  do {
    ClearCommError(hPort, &dwErrors, &ComStat);
  } while (ComStat.cbOutQue > 0);

  Sleep(10);

  GetCommState(hPort, &PortDCB);

  result = PortDCB.BaudRate;
  PortDCB.BaudRate = BaudRate;

  if (!SetCommState(hPort, &PortDCB))
    return 0;

  return result;
}

int
SerialPort::Read(void *Buffer, size_t Size)
{
  DWORD dwBytesTransferred;

  if (hPort == INVALID_HANDLE_VALUE)
    return -1;

  if (ReadFile(hPort, Buffer, Size, &dwBytesTransferred, (OVERLAPPED *)NULL))
    return dwBytesTransferred;

  return -1;
}

void
SerialPort::ProcessChar(char c)
{
  FifoBuffer<char>::Range range = buffer.write();
  if (range.second == 0) {
    // overflow, so reset buffer
    buffer.clear();
    return;
  }

  if (c == '\n') {
    range.first[0] = _T('\0');
    buffer.append(1);

    range = buffer.read();
    handler.LineReceived(range.first);
    buffer.clear();
  } else if (c != '\r') {
    range.first[0] = c;
    buffer.append(1);
  }
}
